#include "Token.h"
#include <utility>
namespace lexer{
using SourceCodeReference = sourceCodeManagement::SourceCodeReference;
using SourceCodeManager=sourceCodeManagement::SourceCodeManager;
//helper function-------------------------------------------------------------------------------------------------------------------------
/*
 * this function checks whether the string only has digits
 */
static bool hasOnlyDigits(std::string& s){
    if(s.empty()){
        return false;
    }
    for(char& c: s){
        if(c<'0'||c>'9'){
            return false;
        }
    }
    return true;
}
static bool hasOnlyLetters(std::string& s){
    if(s.empty()){
        return false;
    }
    for(char& c: s){
        if((c<'A'||c>'Z') && (c<'a'||c>'z')){
            return false;
        }
    }
    return true;
}
static bool isDigit(const char& c){
    return c>='0'&&c<='9';
}
static bool isLetter(const char& c){
    return (c>='a' && c<='z')|| (c>='A'&&c<='Z');
}
static bool isValidGeneric(const char& c){
    return  c ==';' || c =='.'|| c ==',' || c =='+'|| c =='-'|| c =='*'|| c =='/'
        || c =='('|| c ==')'|| c ==':'|| c =='=';
}
static bool isValidChar(const char& c){
    return isDigit(c) || isLetter(c) || isValidGeneric(c);
}
//Token-----------------------------------------------------------------------------------------------------------------------------
Token::Token(sourceCodeManagement::SourceCodeManager& manager): sourceCodeReference(SourceCodeReference(manager)),type(TokenTypes::Invalid){}
Token::Token(SourceCodeReference  characters, TokenTypes type): sourceCodeReference(std::move(characters)), type(type){}
TokenTypes Token::getType() const{
    return type;
}
IdentifierToken::IdentifierToken(SourceCodeReference characters): Token(std::move(characters),TokenTypes::Identifier){}
LiteralToken::LiteralToken(SourceCodeReference characters): Token(std::move(characters),TokenTypes::Literal){}
SeparatorToken::SeparatorToken(SourceCodeReference characters, TokenTypes type): Token(std::move(characters),type){}
OperatorToken::OperatorToken(SourceCodeReference characters, TokenTypes type): Token(std::move(characters),type){}
KeywordToken::KeywordToken(SourceCodeReference characters, TokenTypes type): Token(std::move(characters),type){}
//Tokenizer-------------------------------------------------------------------------------------------------------------------------
Tokenizer::Tokenizer(sourceCodeManagement::SourceCodeManager& manager): manager(manager){}
bool Tokenizer::isValidToken(std::string tokenText){
     if(tokenText.empty() || tokenText ==";" || tokenText =="."|| tokenText =="," || tokenText =="+"|| tokenText =="-"|| tokenText =="*"|| tokenText =="/"
               || tokenText =="("|| tokenText ==")"|| tokenText ==":"|| tokenText =="=" ||tokenText == ":="){
        return true;
    }
    if(hasOnlyDigits(tokenText) ||hasOnlyLetters(tokenText)){
        return true;
    }
    return false;
}
bool Tokenizer::hasNext() const{
    return position<manager.source.size();
}
/*
 * this function returns the next token
 */
Token Tokenizer::next(){
    return next(manager.source.substr(position,manager.source.size()-position));
}
Token Tokenizer::next(const std::string_view& sourceCode) {
    std::string current;
    size_t startingPos = 0;

    for(const char&c: sourceCode){
        if(c != ' ' && c!='\t' && c!='\n'){
            if(current.empty()){
                //we save the location of the first non-whitespace char
                startingPos = position;
            }
            if(!isValidChar(c)){
                //invalid character detected
                auto s = SourceCodeReference(position,manager);
                s.printContext("error: invalid character has been used!");
                return {s,TokenTypes::Invalid};
            } else if(!isValidToken(current+c)){
                //if not valid we need to save the token with the current text;
                //current char will we review in the next "next()" call
                break;
            }
            position++;
            current+=c;
            switch (c) {
                case '.':{
                    return SeparatorToken{SourceCodeReference(startingPos,manager), TokenTypes::Dot};
                }
                case ';':{
                    return SeparatorToken{SourceCodeReference(startingPos,manager), TokenTypes::Semicolon};
                }
                case ',':{
                    return SeparatorToken{SourceCodeReference(startingPos,manager), TokenTypes::Comma};
                }
                case '+':{
                    return OperatorToken{SourceCodeReference(startingPos,manager), TokenTypes::PlusOperator};
                }
                case '-':{
                    return OperatorToken{SourceCodeReference(startingPos,manager),TokenTypes::MinusOperator};
                }
                case '/':{
                    return OperatorToken{SourceCodeReference(startingPos,manager),TokenTypes::DivOperator};
                }
                case '*':{
                    return OperatorToken{SourceCodeReference(startingPos,manager),TokenTypes::MulOperator};
                }
                case ':':{
                    break;
                }
                case '=':{
                    if(current == ":="){
                        return OperatorToken{SourceCodeReference(startingPos,manager,2),TokenTypes::AssignEquals};
                    } else{
                        return OperatorToken{SourceCodeReference(startingPos,manager),TokenTypes::InitEquals};
                    }
                }
                case '(':{
                    return SeparatorToken{SourceCodeReference(startingPos,manager),TokenTypes::OpenBracket};
                }
                case ')':{
                    return SeparatorToken{SourceCodeReference(startingPos, manager), TokenTypes::CloseBracket};
                }
                default:{
                   continue;
                }
            }
        } else if(!current.empty()){
           break;
        } else {
            position++;
        }
    }
    if(!current.empty()){
        if(current =="PARAM"){
            //the position of the last character of the string is the given argument (line,linePos)
            return KeywordToken{SourceCodeReference(startingPos, manager, current.size()), TokenTypes::PARAM};
        } else if (current=="VAR"){
            return KeywordToken{SourceCodeReference(startingPos, manager, current.size()), TokenTypes::VAR};
        }
        else if (current=="CONST"){
            return KeywordToken{SourceCodeReference(startingPos, manager, current.size()), TokenTypes::CONST};
        }
        else if (current=="BEGIN"){
            return KeywordToken{SourceCodeReference(startingPos, manager, current.size()), TokenTypes::BEGIN};
        }
        else if (current=="END"){
            return KeywordToken{SourceCodeReference(startingPos, manager, current.size()), TokenTypes::END};
        }
        else if (current=="RETURN"){
            return KeywordToken{SourceCodeReference(startingPos, manager, current.size()), TokenTypes::RETURN};
        }else if(hasOnlyDigits(current)){
            //if there are only digit it has to be a literal
            return LiteralToken{SourceCodeReference(startingPos, manager, current.size())};
        } else if(hasOnlyLetters(current)){
            return IdentifierToken{SourceCodeReference(startingPos, manager, current.size())};
        }
    }
    auto s = SourceCodeReference(startingPos,manager);
    s.printContext("error: invalid character has been used!");
    return {s, TokenTypes::Invalid};
}
} // namespace lexer
