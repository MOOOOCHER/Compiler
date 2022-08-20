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
    //TODO: More token classes
/*
 * class for tokens
 */
class Token{
    friend class Tokenizer;
    SourceCodeReference sourceCodeReference;
    TokenTypes type;
    public:
    /*
     * constructs a "default" token pointing to the last character of the code
     */
    explicit Token(sourceCodeManagement::SourceCodeManager& manager);
    /*
     * constructs a token with a reference to the code with a token type
     */
    Token(SourceCodeReference  characters, TokenTypes type);

    TokenTypes getType() const;
    SourceCodeReference getSourceCodeReference() const{ return sourceCodeReference;}
};
/*
 * class for Tokenizing the source code
 */
class Tokenizer{
    size_t position = 0;
    sourceCodeManagement::SourceCodeManager& manager;
    /*
     * this function whether the token is invalid
     */
    static bool isValidToken(std::string tokenText);
    /*
     * this function parses the next token and returns whether it is valid (only has letters or only has digits) with the given substring
     */
    Token next(const std::string_view& sourceCode);
    public:
    explicit Tokenizer(sourceCodeManagement::SourceCodeManager& manager);
    /*
     * this function parses the next token and returns whether it is valid (only has letters or only has digits)
     */
    Token next();
    /*
     * this function returns, whether there is a next token to be evaluated (doesn't care about its validity)
     */
    bool hasNext() const;
    sourceCodeManagement::SourceCodeManager& getManager(){
        return manager;
    }
};
}//namespace lexer
#endif //PLJIT_TOKEN_H
