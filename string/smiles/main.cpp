#include <algorithm>
#include <string>
#include <vector>
#include <iostream>


std::string remove_smiles(const std::string& input)
{
    static const std::vector<std::string> tokens = {
        ":-)",
        ":-("
    };

    std::string result;
    result.resize(input.size());
    int output = 0;

    std::vector<int> positions;
    positions.resize(tokens.size(), 0);

    for (int pos = 0; pos < static_cast<int>(input.size()); ++pos) {
        result[output] = input[pos];

        for (int tokenIndex = 0; tokenIndex < static_cast<int>(tokens.size()); ++tokenIndex) {
            const auto& token = tokens[tokenIndex];
            auto& tokenPos = positions[tokenIndex];

            char nextChar = tokenPos >= token.size() ? token.back() : token[tokenPos];

            if (input[pos] == nextChar) {
                ++tokenPos;
                continue;
            }

            if (tokenPos >= static_cast<int>(token.size())) {
                output -= tokenPos;
                result[output] = input[pos];
                std::fill(positions.begin(), positions.end(), 0);
                break;
            } else {
                tokenPos = 0;
            }
        }
        ++output;
    }

    for (int tokenIndex = 0; tokenIndex < static_cast<int>(tokens.size()); ++tokenIndex) {
        const auto& token = tokens[tokenIndex];
        auto tokenPos = positions[tokenIndex];
        if (tokenPos >= token.size()) {
            output -= tokenPos;
            break;
        }
    }
    result.resize(output);
    return result;
}

int main()
{
    std::vector<std::string> inputs = {
        "Я работаю в Гугле :-)))",
        "везет :-) а я туда собеседование завалил:-((",
        "лол:)",
        "Ааааа!!!!! :-))(())",
    };

    for (const auto& input : inputs) {
        std::cout << "Original: '" << input << "'  processed: '" << remove_smiles(input) << "'" << std::endl;
    }
    return 0;
}
