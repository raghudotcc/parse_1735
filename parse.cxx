#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

using Grammar = std::map<std::string, std::vector<std::vector<std::string>>>;
using Sequence = std::vector<std::string>;

struct AST {
    std::string type;
    std::vector<AST*> children;
};


/*Utility function to split a string into a vector of strings based on the 
delimiter. Also removes the leading and trailing whitespaces from the
split tokens before returning. And removes empty tokens from the vector.
params:
    text: the string to split
    delim: the delimiter to split on
    [maxsplit]: the maximum number of splits to perform. If maxsplit is not 
        specified, the string is split on every instance of the delimiter.
        which is denoted by -1 in the optional parameter.
returns:
    a vector of strings split on the delimiter.*/
std::vector<std::string> split(const std::string& text, 
    const std::string& delimiter, int maxsplit=-1) 
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    size_t lastPos = 0;
    { 
        while (((pos = text.find(delimiter, lastPos)) != std::string::npos) && 
                (maxsplit == -1 || tokens.size() < maxsplit)) {
            tokens.push_back(text.substr(lastPos, pos - lastPos));
            lastPos = pos + delimiter.length();

        }
        tokens.push_back(text.substr(lastPos));

        tokens.erase(std::remove_if(tokens.begin(), tokens.end(), 
            [](const std::string& s) { return s.empty(); }), tokens.end());

        for (auto& token : tokens) {
            token.erase(0, token.find_first_not_of(" "));
            token.erase(token.find_last_not_of(" ") + 1);
        }
    }
    return tokens;
}

/*Utility function to sanitize the grammar by splitting it into 
a map of nonterminals to a vector of productions. Also replace all 
tabs with spaces.
params:
    description: the grammar to sanitize
    [whitespace]: how to handle whitespace between tokens. Default is \s*, 
    that is there can be any number of whitespaces between tokens.
returns:
    a map of nonterminals to a vector of productions.*/
Grammar grammar(const std::string& description, 
    const std::string& whitespace=R"(\s*)") {
    Grammar G;
    G[" "] = { { whitespace } };

    std::string description_ = description;
    std::replace(description_.begin(), description_.end(), '\t', ' ');
    for (const auto& line : split(description_, "\n")) {
        auto parts = split(line, " => ", 1);
        auto lhs = parts[0];
        auto rhs = parts[1];
        auto alternatives = split(rhs, " | ");
        for (const auto& alternative : alternatives) {
            G[lhs].push_back(split(alternative, " "));
        }
    }
    return G;
}

// AST parse(const std::string& start_symbol,
//      const std::string& text, const Grammar& grammar) {

//     auto parse_sequence = [](const Sequence sequence, 
//         const std::string& text) {

//     }
// }


int main() {
    auto G = grammar(R"(
        Exp => `Term [+-] Exp | Term
        Term => Factor [*/] Term | Factor
        Factor => [0-9]+ | ( Exp ))"
        );

    for (const auto& [lhs, rhs] : G) {
        std::cout << lhs << " => ";
        for (const auto& production : rhs) {
            for (const auto& symbol : production) {
                std::cout << symbol << ", ";
            }
        }
        std::cout << std::endl;
    }
}

