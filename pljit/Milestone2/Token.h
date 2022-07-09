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
    SourceCodeReference sourceCodeReference;
    /*
     * constructs a "default" token pointing to the last character of the code
     */
    explicit Token(sourceCodeManagement::SourceCodeManager& manager): sourceCodeReference(SourceCodeReference(manager)),type(TokenTypes::Invalid), textLength(0){};
    Token(const SourceCodeReference characters, TokenTypes type, size_t textLength): sourceCodeReference(characters), type(type), textLength(textLength){};
    Token(const Token& other): sourceCodeReference(other.sourceCodeReference), type(other.type), textLength(other.textLength){}
    Token operator=(const Token& other){
        if(&other != this){
            sourceCodeReference = other.sourceCodeReference;
            type = other.type;
            textLength = other.textLength;
        }
        return *this;
    }

    TokenTypes getType();
    std::string getText();
    private:
    TokenTypes type;
    size_t textLength = 0;
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
