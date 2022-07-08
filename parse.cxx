#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>

using Grammar = std::map<std::string, std::vector<std::vector<std::string>>>;
using Sequence = std::vector<std::string>;

/*
Given a string, get all the position pairs of opening and closing braces
in the string. If the braces are inside double quotes it means that 
they are not part of the grammar syntax, they are a lexeme that needs to 
be parsed, so ignore them.
params:
    text: the string to get the position pairs of braces from
returns:
    a vector of position pairs of matching brackes(curly and square)
*/
std::vector<std::pair<int, int>> get_matching_braces_pos(
    const std::string& text) {
    std::vector<std::pair<int, int>> result;
    int open_curly_braces_pos = 0;
    int open_square_braces_pos = 0;
    bool in_quote = false;
    for (int i = 0; i < text.size(); ++i) {
        if (text[i] == '"') {
            in_quote = !in_quote;
        }
        if (text[i] == '{' && !in_quote) {
            open_curly_braces_pos = i;
        }
        if (text[i] == '}' && !in_quote) {
            result.push_back({open_curly_braces_pos, i});
        }
        if (text[i] == '[' && !in_quote) {
            open_square_braces_pos = i;
        }
        if (text[i] == ']' && !in_quote) {
            result.push_back({open_square_braces_pos, i});
        }
    }
    return result;
}

/*Check if the position of the delimiter passed to the 
split function is between curly braces or square braces.
We are supporting ebnf syntax. We cannot split a string
between curly braces and square braces before the parsing 
stage.
params:
    pos: current position of the delimiter in the string passed 
         to the split function
    text: string passed to the split function
returns:
    true if the position of the delimiter is between curly/square braces
    false otherwise*/
bool is_delim_in_braces(const int& pos, const std::string& text) {
    auto matching_braces_pos = get_matching_braces_pos(text);
    for (auto& pair : matching_braces_pos) {
        if (pair.first < pos && pos < pair.second) {
            return true;
        }
    }
    return false;
}


/*Utility function to split a string into a vector of strings based on the 
delimiter. Also removes the leading and trailing whitespaces from the
split tokens before returning. And removes empty/whitespace only tokens 
from the vector.
params:
    text: the string to split
    delim: the delimiter to split on
    [maxsplit]: the maximum number of splits to perform. If maxsplit is not 
        specified, the string is split on every instance of the delimiter.
        which is denoted by -1 in the optional parameter.
returns:
    a vector of strings split on the delimiter.*/
std::vector<std::string> split(const std::string& text, 
    const std::string& delimiter, int maxsplit=-1, bool ebnf=false) {
   std::vector<std::string> tokens;
    size_t pos = 0;
    size_t last_pos = 0;
    size_t ebnf_last_pos = 0;
    bool splittable = true;
    std::vector<std::pair<int, int>> ebnf_matching_braces_pos;
    {   
        while (((pos = text.find(delimiter, last_pos)) != std::string::npos) && 
                (maxsplit == -1 || tokens.size() < maxsplit)) {   
            if (ebnf)   {
                splittable = !is_delim_in_braces(pos, text);
            }
            if (splittable) {
                if (ebnf_last_pos != 0) {
                    tokens.push_back(text.substr(ebnf_last_pos, 
                        pos - ebnf_last_pos));
                    ebnf_last_pos = 0;
                } else {
                    tokens.push_back(text.substr(last_pos, pos - last_pos));
                }
            } else {
                if (ebnf_last_pos == 0) {
                    ebnf_last_pos = last_pos;
                }
            }
            last_pos = pos + delimiter.length(); 
        }

        if (ebnf_last_pos != 0) {
            tokens.push_back(text.substr(ebnf_last_pos));
        } else {
            tokens.push_back(text.substr(last_pos));
        }

        for (auto& token : tokens) {
            token.erase(0, token.find_first_not_of(" "));
            token.erase(token.find_last_not_of(" ") + 1);

            if (token.empty()) {
                tokens.erase(std::remove(tokens.begin(), tokens.end(), token), 
                    tokens.end());
            }
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
        auto parts = split(line, "::=", /*maxsplit=*/1, /*ebnf=*/false);
        auto lhs = parts[0];
        auto rhs = parts[1];
        auto alternatives = split(rhs, "|");
        for (const auto& alternative : alternatives) {
            G[lhs].push_back(split(alternative, " ", 
                /*maxsplit=*/-1, /*ebnf=*/true));
        }
    }
    return G; 
}


int main() {
    auto G = grammar(R"(
        interface_viewport_stmt ::= interface_viewport [scoped_identifier {, scoped_identifier}] | interface_viewport
    )"
        );

    for (const auto& [lhs, rhs] : G) {
        std::cout << lhs << " ::= ";
        for (const auto& production : rhs) {
            for (const auto& symbol : production) {
                std::cout << '"' << symbol << '"' << ", ";
            } 
            if (production != rhs.back()) {
                std::cout << " | ";
            }
        }
        std::cout << std::endl;
    }
}

