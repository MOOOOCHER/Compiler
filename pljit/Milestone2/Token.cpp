#include "Token.h"
namespace lexer{
using SourceCodeReference = sourceCodeManagement::SourceCodeReference;
using SourceCodeManager=sourceCodeManagement::SourceCodeManager;
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
TokenTypes Token::getType(){
    return type;
}
std::string Token::getText(){
    return sourceCodeReference.getText();
}
//Tokenizer-------------------------------------------------------------------------------------------------------------------------
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
/*
 * this function returns the next token
 */
Token Tokenizer::next(){
    return next(manager.source.substr(position,manager.source.size()-position));
}
Token Tokenizer::next(const std::string_view& sourceCode) {
    std::string current;
    const char* ptr = nullptr;
    for(const char&c: sourceCode){
        if(c != ' ' && c!='\t' && c!='\n'){
            if(current.empty()){   //we save the location of the first non-whitespace char
                ptr = &c;
            }
            if(!isValidChar(c)){
                auto s = SourceCodeReference(ptr,manager,1);
                s.printContext("An invalid character has been used!");
                return Token(s,TokenTypes::Invalid);
            } else if(!isValidToken(current+c)){   //if not valid we need to save the token with the current text;
                break;
            }
            position++;
            current+=c;
            switch (c) {
                case '.':{
                    return Token(SourceCodeReference(ptr,manager,1), TokenTypes::Dot);
                }
                case ';':{
                    return Token(SourceCodeReference(ptr,manager,1), TokenTypes::Semicolon);
                }
                case ',':{
                    return Token(SourceCodeReference(ptr,manager,1), TokenTypes::Comma);
                }
                case '+':{
                    return Token(SourceCodeReference(ptr,manager,1), TokenTypes::PlusOperator);
                }
                case '-':{
                    return Token(SourceCodeReference(ptr,manager,1),TokenTypes::MinusOperator);
                }
                case '/':{
                    return Token(SourceCodeReference(ptr,manager,1),TokenTypes::DivOperator);
                }
                case '*':{
                    return Token(SourceCodeReference(ptr,manager,1),TokenTypes::MulOperator);
                }
                case ':':{
                    break;
                }
                case '=':{
                    if(current == ":="){
                        return Token(SourceCodeReference(ptr,manager,2),TokenTypes::AssignEquals);
                    } else{
                        return Token(SourceCodeReference(ptr,manager,1),TokenTypes::InitEquals);
                    }
                }
                case '(':{
                    return Token(SourceCodeReference(ptr,manager,1),TokenTypes::OpenBracket);
                }
                case ')':{
                    return Token(SourceCodeReference(ptr,manager,1),TokenTypes::CloseBracket);
                }
                default:{
                    if(isDigit(c)){
                        if(hasOnlyLetters(current)){
                            return Token(SourceCodeReference(ptr,manager,current.size()),TokenTypes::Identifier);
                        }
                    } else if( isLetter(c)){
                        if(hasOnlyDigits(current)){
                            return Token(SourceCodeReference(ptr,manager,current.size()),TokenTypes::Literal);
                        }
                    }
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
            return Token(SourceCodeReference(ptr, manager,current.size()), TokenTypes::PARAM);
        } else if (current=="VAR"){
            return Token(SourceCodeReference(ptr, manager,current.size()), TokenTypes::VAR);
        }
        else if (current=="CONST"){
            return Token(SourceCodeReference(ptr, manager,current.size()), TokenTypes::CONST);
        }
        else if (current=="BEGIN"){
            return Token(SourceCodeReference(ptr, manager,current.size()), TokenTypes::BEGIN);
        }
        else if (current=="END"){
            return Token(SourceCodeReference(ptr, manager,current.size()), TokenTypes::END);
        }
        else if (current=="RETURN"){
            return Token(SourceCodeReference(ptr, manager,current.size()), TokenTypes::RETURN);
        }else if(hasOnlyDigits(current)){
            //if there are only digit it has to be a literal
            return Token(SourceCodeReference(ptr, manager,current.size()), TokenTypes::Literal);
        } else{
            return Token(SourceCodeReference(ptr, manager,current.size()), TokenTypes::Identifier);
        }
    }
    auto s = SourceCodeReference(ptr,manager,1);
    s.printContext("An invalid character has been used!");
    return Token(s,TokenTypes::Invalid);
}
bool Tokenizer::hasNext() const{
    return position<manager.source.size();
}
} // namespace lexer
