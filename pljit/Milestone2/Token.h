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
    /*
     * constructs a "default" token pointing to the last character of the code
     */
    explicit Token(sourceCodeManagement::SourceCodeManager& manager): sourceCodeReference(SourceCodeReference(manager)),type(TokenTypes::Invalid){};
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
    /*
     * this function whether the token is invalid
     */
    static bool isValidToken(std::string tokenText);
    size_t position = 0;
    sourceCodeManagement::SourceCodeManager& manager;
    public:
    explicit Tokenizer(sourceCodeManagement::SourceCodeManager& manager): manager(manager){};
    /*
     * this function parses the next token and returns whether it is valid (only has letters or only has digits)
     */
    Token next();
    Token next(const std::string_view& sourceCode);
    /*
     * this function parses the next token and returns whether it is valid (only has letters or only has digits)
     */
    bool hasNext();
    size_t getPosition(){
        return position;
    }
    sourceCodeManagement::SourceCodeManager& getManager(){
        return manager;
    }
};
}//namespace lexer
#endif //PLJIT_TOKEN_H
