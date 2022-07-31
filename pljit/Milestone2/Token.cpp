#include "Token.h"
#include <charconv>
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
Token::Token(SourceCodeReference  characters, TokenTypes type, unsigned long value): sourceCodeReference(std::move(characters)), type(type), value(value){}
//copy semantics
Token::Token(const Token& other) = default;
Token& Token::operator=(const Token& other){
    if(&other != this){
        sourceCodeReference = other.sourceCodeReference;
        type = other.type;
        value = other.value;
    }
    return *this;
}

TokenTypes Token::getType(){
    return type;
}
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
    const char* ptr = nullptr;
    size_t startingPos;

    for(const char&c: sourceCode){
        if(c != ' ' && c!='\t' && c!='\n'){
            if(current.empty()){
                //we save the location of the first non-whitespace char
                ptr = &c;
                startingPos = position;
            }
            if(!isValidChar(c)){
                //invalid character detected
                auto s = SourceCodeReference(&c, position,manager);
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
                    return {SourceCodeReference(ptr,startingPos,manager), TokenTypes::Dot};
                }
                case ';':{
                    return {SourceCodeReference(ptr,startingPos,manager), TokenTypes::Semicolon};
                }
                case ',':{
                    return {SourceCodeReference(ptr,startingPos,manager), TokenTypes::Comma};
                }
                case '+':{
                    return {SourceCodeReference(ptr,startingPos,manager), TokenTypes::PlusOperator};
                }
                case '-':{
                    return {SourceCodeReference(ptr,startingPos,manager),TokenTypes::MinusOperator};
                }
                case '/':{
                    return {SourceCodeReference(ptr,startingPos,manager),TokenTypes::DivOperator};
                }
                case '*':{
                    return {SourceCodeReference(ptr,startingPos,manager),TokenTypes::MulOperator};
                }
                case ':':{
                    break;
                }
                case '=':{
                    if(current == ":="){
                        return {SourceCodeReference(ptr,startingPos,manager,2),TokenTypes::AssignEquals};
                    } else{
                        return {SourceCodeReference(ptr,startingPos,manager),TokenTypes::InitEquals};
                    }
                }
                case '(':{
                    return {SourceCodeReference(ptr,startingPos,manager),TokenTypes::OpenBracket};
                }
                case ')':{
                    return {SourceCodeReference(ptr,startingPos, manager), TokenTypes::CloseBracket};
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
            return {SourceCodeReference(ptr,startingPos, manager, current.size()), TokenTypes::PARAM};
        } else if (current=="VAR"){
            return {SourceCodeReference(ptr,startingPos, manager, current.size()), TokenTypes::VAR};
        }
        else if (current=="CONST"){
            return {SourceCodeReference(ptr,startingPos, manager, current.size()), TokenTypes::CONST};
        }
        else if (current=="BEGIN"){
            return {SourceCodeReference(ptr,startingPos, manager, current.size()), TokenTypes::BEGIN};
        }
        else if (current=="END"){
            return {SourceCodeReference(ptr,startingPos, manager, current.size()), TokenTypes::END};
        }
        else if (current=="RETURN"){
            return {SourceCodeReference(ptr,startingPos, manager, current.size()), TokenTypes::RETURN};
        }else if(hasOnlyDigits(current)){
            //if there are only digit it has to be a literal
            unsigned long value;
            std::from_chars(current.data(),current.data()+current.size(),value);
            return {SourceCodeReference(ptr,startingPos, manager, current.size()), TokenTypes::Literal, value};
        } else{
            return {SourceCodeReference(ptr,startingPos, manager, current.size()), TokenTypes::Identifier};
        }
    }
    auto s = SourceCodeReference(ptr,position,manager);
    s.printContext("error: invalid character has been used!");
    return {s, TokenTypes::Invalid};
}
} // namespace lexer
