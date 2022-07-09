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
        Invalid,
        Dot,
        Comma,
        Semicolon,
        InitEquals,
        AssignEquals,
        OpenBracket,
        CloseBracket,
        PlusOperator,
        MinusOperator,
        MulOperator,
        DivOperator,
        RETURN,
        VAR,
        PARAM,
        CONST,
        BEGIN,
        END,
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
    explicit Token(sourceCodeManagement::SourceCodeManager& manager): sourceCodeReference(SourceCodeReference(manager)),type(TokenTypes::Invalid){};
    Token(const SourceCodeReference characters, TokenTypes type): sourceCodeReference(characters), type(type){};
    Token(const SourceCodeReference characters, TokenTypes type, unsigned long value): sourceCodeReference(characters), type(type), value(value){};

    Token(const Token& other) = default;
    Token& operator=(const Token& other){
        if(&other != this){
            sourceCodeReference = other.sourceCodeReference;
            type = other.type;
            value = other.value;
        }
        return *this;
    }

    TokenTypes getType();
    unsigned long getValue() const{ return value;}
    private:
    TokenTypes type;
    unsigned long value = 0;
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
    bool hasNext() const;
    size_t getPosition() const{
        return position;
    }
    sourceCodeManagement::SourceCodeManager& getManager(){
        return manager;
    }
};
}//namespace lexer
#endif //PLJIT_TOKEN_H
