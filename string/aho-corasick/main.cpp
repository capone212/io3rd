#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <memory>
#include <assert.h>
#include <deque>
#include <unordered_set>
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

    // debug
    std::string Path;
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
                child->Path = current->Path + ch;
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

    while (true) {
        auto it = node->Child.find(nextCharacter);
        if (it != node->Child.end()) {
            return it->second;
        }

        if (node == node->FailureLink) {
            break;
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
    for (const auto [_, child] : root->Child) {
        child->FailureLink = root;
        for (const auto [ch, grandChild] : child->Child) {
            queue.push_back({ch, grandChild, child});
        }
    }

    // Iteratively set failure links for nodes.
    while (!queue.empty()) {
        auto item = queue.front();
        queue.pop_front();

        auto* current = item.Node;

        current->FailureLink = FindFailureLink(item.Char, current, item.Parent);

        // std::cout << "PopulateFailureLinks node: " << current->Path
        //     << " FailureLink: " << current->FailureLink->Path << std::endl;

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

    bool operator==(const TMatch& other) const
    {
        return std::make_tuple(TextPosition, KeywordIndex) == std::make_tuple(other.TextPosition, other.KeywordIndex);
    }

    bool operator<(const TMatch& other) const
    {
        return std::make_tuple(TextPosition, KeywordIndex) < std::make_tuple(other.TextPosition, other.KeywordIndex);
    }
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

std::vector<TMatch> BruteForceSearch(const std::string& text, const std::vector<std::string>& keywords)
{
    std::vector<TMatch> result;

    for (int keywordIndex = 0; keywordIndex < keywords.size(); ++keywordIndex) {
        const auto& keyword = keywords[keywordIndex];
        int index = 0;
        while ((index = text.find(keyword, index)) != std::string::npos) {
            result.push_back(TMatch{
                .TextPosition = static_cast<int>(index + keyword.size()) - 1,
                .KeywordIndex = keywordIndex,
            });
            index += 1;
        }
    }

    return result;
}

struct TTestCase
{
    std::vector<std::string> Keywords;
    std::string Text;
    std::vector<TMatch> Results;
};

bool CheckTestCase(const TTestCase& testCase)
{
    auto expected = BruteForceSearch(testCase.Text, testCase.Keywords);
    auto actual = testCase.Results;

    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());

    if (std::equal(expected.begin(), expected.end(), actual.begin(), actual.end())) {
        return true;
    }

    for (const auto& match : expected) {
        std::cout << "Expected match position:" << match.TextPosition
            << " keyword:" << testCase.Keywords[match.KeywordIndex] << std::endl;
    }

    std::cout << std::endl;

    for (const auto& match : actual) {
        std::cout << "Got match position:" << match.TextPosition
            << " keyword:" << testCase.Keywords[match.KeywordIndex] << std::endl;
    }

    return false;
}

std::string GetRandomString(int len, int characters) {
    static const char ALPHANUM[] = "abcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(len);

    characters = std::max<int>(characters, 1);
    characters = std::min<int>(characters, sizeof(ALPHANUM) - 1);

    for (int i = 0; i < len; ++i) {
        result.push_back(ALPHANUM[rand() % characters]);
    }

    return result;
}

std::vector<std::string> GenerateRandomKeywords(int count, int maxSize, int charactersCount)
{
    std::unordered_set<std::string> result;
    result.reserve(count);

    for (int index = 0; index < count; ++index) {
        int length = (rand() % maxSize) + 1;
        result.insert(GetRandomString(length, charactersCount));
    }

    return {result.begin(), result.end()};
}

int main() {
    TTestCase test{
        .Keywords = {
                "tat",
                "tatem",
                "tandem",
                "bitandem",
            },
        .Text = "bitandemtatem",
    };
    test.Results = AhoCorasickSearch(test.Text, test.Keywords);
    CheckTestCase(test);

    for (int i = 0; i < 100000; ++i)
    {
        TTestCase test{
            .Keywords = GenerateRandomKeywords(1000, 10, 20),
            .Text = GetRandomString(10000, 20),
        };
        test.Results = AhoCorasickSearch(test.Text, test.Keywords);
        if (!CheckTestCase(test)) {
            std::cout << "text: " << test.Text << std::endl;
            return 1;
        }
    }

    return 0;
}
