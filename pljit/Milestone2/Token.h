#ifndef PLJIT_TOKEN_H
#define PLJIT_TOKEN_H
#include "../Milestone1/SourceCodeManager.h"
#include <string>
#include <utility>
#include <vector>
namespace lexer {
using SourceCodeReference = sourceCodeManagement::SourceCodeReference;
    enum class TokenTypes {
        Identifier,
        Literal,
        Operator,
        Separator,
        Keyword,
        Invalid,
    };
/*
 * base class for tokens
 */
class Token{
    friend class Tokenizer;
    public:
    const SourceCodeReference sourceCodeReference;
    Token(const SourceCodeReference characters, TokenTypes type, std::string text): sourceCodeReference(characters), type(type), text(std::move(text)){};
    TokenTypes getType();
    std::string getText();
    protected:
    TokenTypes type;
    std::string text = "";
};
/*
 * class for Tokenizing the source code
 */
class Tokenizer{
    std::vector<Token> tokens;
    /*
     * this function parses the next token and returns whether it is valid (only has letters or only has digits)
     */
    bool next(size_t& position, const std::string_view& sourceCode, sourceCodeManagement::SourceCodeManager& manager);
    /*
     * this function wether the token is invalid
     */
    static bool isValidToken(std::string tokenText);
    public:
    /*
     * this function parses the source code into a vector of tokens
     */
     void parse(sourceCodeManagement::SourceCodeManager& sourceCode);
     std::vector<Token>& getTokens();
};
}//namespace lexer
#endif //PLJIT_TOKEN_H
