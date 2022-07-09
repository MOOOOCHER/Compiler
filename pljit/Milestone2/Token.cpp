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
    std::string result ="";
    for(size_t i=0;i<textLength;i++){
        result+= *(sourceCodeReference.getLocation()+i);
    }
    return result;
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
                auto s = SourceCodeReference(ptr,manager);
                s.printContext("An invalid character has been used!", 1);
                return Token(s,TokenTypes::Invalid,0);
            } else if(!isValidToken(current+c)){   //if not valid we need to save the token with the current text;
                break;
            }
            position++;
            current+=c;
            switch (c) {
                case '.':{
                    return Token(SourceCodeReference(ptr,manager), TokenTypes::Separator,1);
                }
                case ';':{
                    return Token(SourceCodeReference(ptr,manager), TokenTypes::Separator,1);
                }
                case ',':{
                    return Token(SourceCodeReference(ptr,manager), TokenTypes::Separator,1);
                }
                case '+':{
                    return Token(SourceCodeReference(ptr,manager), TokenTypes::Operator,1);
                }
                case '-':{
                    return Token(SourceCodeReference(ptr,manager),TokenTypes::Operator,1);
                }
                case '/':{
                    return Token(SourceCodeReference(ptr,manager),TokenTypes::Operator,1);
                }
                case '*':{
                    return Token(SourceCodeReference(ptr,manager),TokenTypes::Operator,1);
                }
                case ':':{
                    break;
                }
                case '=':{
                    if(current == ":="){
                        return Token(SourceCodeReference(ptr,manager),TokenTypes::Operator,2);
                    } else{
                        return Token(SourceCodeReference(ptr,manager),TokenTypes::Operator,1);
                    }
                }
                case '(':{
                    return Token(SourceCodeReference(ptr,manager),TokenTypes::Separator,1);
                }
                case ')':{
                    return Token(SourceCodeReference(ptr,manager),TokenTypes::Separator,1);
                }
                default:{
                    if(isDigit(c)){
                        if(hasOnlyLetters(current)){
                            return Token(SourceCodeReference(ptr,manager),TokenTypes::Identifier,current.size());
                        }
                    } else if( isLetter(c)){
                        if(hasOnlyDigits(current)){
                            return Token(SourceCodeReference(ptr,manager),TokenTypes::Literal,current.size());
                        }
                    }
                }
            }
        } else if(!current.empty()){
            if(current =="PARAM"||current=="VAR"||current=="CONST"||current=="BEGIN"||current=="END"||current=="RETURN"){
                //the position of the last character of the string is the given argument (line,linePos)
                return Token(SourceCodeReference(ptr, manager), TokenTypes::Keyword,current.size());
            } else if(hasOnlyDigits(current)){
                //if there are only digit it has to be a literal
                position++;
                return Token(SourceCodeReference(ptr, manager), TokenTypes::Literal,current.size());
            } else{
                position++;
                return Token(SourceCodeReference(ptr, manager), TokenTypes::Identifier,current.size());
            }
        } else {
            position++;
        }
    }
    if(!current.empty()){
        if(current =="PARAM"||current=="VAR"||current=="CONST"||current=="BEGIN"||current=="END"||current=="RETURN"){
            //the position of the last character of the string is the given argument (line,linePos)
            return Token(SourceCodeReference(ptr, manager), TokenTypes::Keyword,current.size());
        } else if(hasOnlyDigits(current)){
            //if there are only digit it has to be a literal
            return Token(SourceCodeReference(ptr, manager), TokenTypes::Literal,current.size());
        } else{
            return Token(SourceCodeReference(ptr, manager), TokenTypes::Identifier,current.size());
        }
    }
    auto s = SourceCodeReference(ptr,manager);
    s.printContext("An invalid character has been used!", 1);
    return Token(s,TokenTypes::Invalid,0);
}
bool Tokenizer::hasNext(){
    return position<manager.source.size();
}
} // namespace lexer
