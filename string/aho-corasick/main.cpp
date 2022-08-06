#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <assert.h>
#include <deque>
#include <unordered_map>

struct TNode
{
    TNode() = default;

    // Disable copy
    TNode(const TNode&) = delete;
    TNode& operator=(const TNode& node) = delete;

    ~TNode()
    {
        for (auto [_, node] : Child) {
            delete node;
        }
    }

    TNode* FailureLink = nullptr;
    int Order = 0;
    std::unordered_map<char, TNode*> Child;
    std::optional<int> KeyWordIndex;
};

using PNode = std::unique_ptr<TNode>;

struct TKeywordTree
{
    std::vector<std::string> Keywords;
    PNode Root;
};

TKeywordTree MakeKeyWordTree(const std::vector<std::string>& keywords)
{
    PNode root = std::make_unique<TNode>();
    for (int index = 0; index < static_cast<int>(keywords.size()); ++index) {
        auto* current = root.get();

        for (char ch : keywords[index]) {
            auto& child = current->Child[ch];
            if (!child) {
                child = new TNode();
            }
            current = child;
        }

        assert(!current->KeyWordIndex);
        current->KeyWordIndex = index;
    }

    return TKeywordTree{.Keywords = keywords, .Root = std::move(root)};
}

TNode* FindFailureLink(char nextCharacter, TNode* current, TNode* parent)
{
    assert(parent->FailureLink);
    auto* node = parent->FailureLink;

    while (node != node->FailureLink) {
        auto it = node->Child.find(nextCharacter);
        if (it != node->Child.end()) {
            return it->second;
        }
        node = node->FailureLink;
    }

    return node;
}

void PopulateFailureLinks(const TKeywordTree& tree)
{
    struct TQueueItem
    {
        char Char = 0;
        TNode* Node = nullptr;
        TNode* Parent = nullptr;
    };

    auto* root = tree.Root.get();

    // Populate for root and schedule root's direct children
    root->FailureLink = root;
    std::deque<TQueueItem> queue;
    for (auto& [ch, child] : root->Child) {
        queue.push_back({ch, child, root});
    }

    // Iteratively set failure links for nodes.
    while (!queue.empty()) {
        auto item = queue.front();
        queue.pop_front();

        auto* current = item.Node;

        current->FailureLink = FindFailureLink(item.Char, current, item.Parent);

        for (auto& [ch, child] : current->Child) {
            queue.push_back({ch, child, current});
        }
    }
}

struct TMatch
{
    // End position of keyword in text
    // (last character index)
    int TextPosition = 0;
    int KeywordIndex = 0;
};

void ReportMatches(TNode* node, int currentPosition, std::vector<TMatch>& matches)
{
    while (node != node->FailureLink)
    {
        if (node->KeyWordIndex) {
            matches.push_back({currentPosition, *node->KeyWordIndex});
        }
        node = node->FailureLink;
    }
}

std::vector<TMatch> AhoCorasickSearch(const std::string& text, const std::vector<std::string>& keyWords)
{
    auto keyWordTree = MakeKeyWordTree(keyWords);
    PopulateFailureLinks(keyWordTree);

    // starting position of search in text
    int currentPosition = 0;
    TNode* node = keyWordTree.Root.get();

    std::vector<TMatch> matches;

    while (currentPosition < text.size()) {
        // Keep matching characters to tree nodes as long as we can.
        while (true) {
            auto it = node->Child.find(text[currentPosition]);
            if (it == node->Child.end()) {
                break;
            }
            node = it->second;
            ReportMatches(node, currentPosition, matches);
            ++currentPosition;
        }

        if (node != node->FailureLink) {
            node = node->FailureLink;
        } else {
            // Move to next character from root.
            ++currentPosition;
        }
    }

    return matches;
}

int main() {
    std::vector<std::string> keyWords = {
        "tobaco",
        "tattooo",
        "tat",
        "tatem",
        "tandem",
        "bitandem",
    };

    auto text = "bitandemtatem";

    // TODO: algirithm have to find tandem but id does not!
    auto matches = AhoCorasickSearch(text, keyWords);

    for (const auto& match : matches) {
        std::cout << "Find match ending at position:" << match.TextPosition << " keyword:" << keyWords[match.KeywordIndex] << std::endl;
    }
    return 0;
}
