// Tiny hand-rolled lexer for one snippet:
//   int main() { int a = 5; std::cout << a << std::endl; }
// It splits the source into tokens and labels each one.

#include <cctype>
#include <iostream>
#include <string>
#include <vector>

// Words that are keywords (everything else made of letters is an identifier).
bool is_keyword(const std::string& word) {
    return word == "int" || word == "return" || word == "if" || word == "else";
}

struct Token {
    std::string kind;   // e.g. "keyword", "identifier", "int_literal", ...
    std::string text;   // the matched characters
};

std::vector<Token> lex(const std::string& src) {
    std::vector<Token> tokens;
    std::size_t i = 0;

    while (i < src.size()) {
        char c = src[i];

        // 1. Skip whitespace.
        if (std::isspace(static_cast<unsigned char>(c))) { ++i; continue; }

        // 2. Identifier or keyword: starts with a letter or '_'.
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            std::size_t start = i;
            while (i < src.size() &&
                   (std::isalnum(static_cast<unsigned char>(src[i])) || src[i] == '_'))
                ++i;
            std::string word = src.substr(start, i - start);
            tokens.push_back({is_keyword(word) ? "keyword" : "identifier", word});
            continue;
        }

        // 3. Integer literal: run of digits.
        if (std::isdigit(static_cast<unsigned char>(c))) {
            std::size_t start = i;
            while (i < src.size() && std::isdigit(static_cast<unsigned char>(src[i])))
                ++i;
            tokens.push_back({"int_literal", src.substr(start, i - start)});
            continue;
        }

        // 4. Two-character operators: ::  <<  >>  ==
        if (i + 1 < src.size()) {
            std::string two = src.substr(i, 2);
            if (two == "::") { tokens.push_back({"scope_op", two}); i += 2; continue; }
            if (two == "<<") { tokens.push_back({"insertion_op", two}); i += 2; continue; }
        }

        // 5. Single-character punctuation / operators.
        std::string one(1, c);
        switch (c) {
            case '(': tokens.push_back({"left_paren",  one}); break;
            case ')': tokens.push_back({"right_paren", one}); break;
            case '{': tokens.push_back({"left_brace",  one}); break;
            case '}': tokens.push_back({"right_brace", one}); break;
            case ';': tokens.push_back({"semicolon",   one}); break;
            case '=': tokens.push_back({"assign_op",   one}); break;
            default:  tokens.push_back({"unknown",     one}); break;
        }
        ++i;
    }
    return tokens;
}

int main() {
    std::string code = "int main() { int a = 5; std::cout << a << std::endl; }";

    std::cout << "[source] " << code << "\n\n";
    std::cout << "[tokens]\n";
    for (const auto& t : lex(code))
        std::cout << "  " << t.text << "  -->  " << t.kind << "\n";
}
