#include <iostream>

struct WordStat {
    std::string Word;
    int Position = -1;
};

int main() {
    WordStat w{.Word = "hello"};
    std::cout << w.Position << std::endl;
}
