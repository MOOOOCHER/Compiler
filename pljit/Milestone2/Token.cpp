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
//Token-----------------------------------------------------------------------------------------------------------------------------
TokenTypes Token::getType(){
    return type;
}
std::string Token::getText(){
    return text;
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
bool Tokenizer::next(size_t& position, const std::string_view& sourceCode, SourceCodeManager& manager) {
    std::string current;
    const char* ptr = nullptr;
    for(const char&c: sourceCode){
        if(c != ' ' && c!='\t' && c!='\n'){
            if(current.empty()){   //we save the location of the first non-whitespace char
                ptr = &c;
            }
            if(!isValidToken(current+c)){   //if not valid we need to save the token with the current text;
                break;
            }
            position++;
            current+=c;
            switch (c) {
                case '.':{
                    tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Separator,".");
                    return true;
                }
                case ';':{
                    tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Separator,";");
                    return true;
                }
                case ',':{
                    tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Separator,",");
                    return true;
                }
                case '+':{
                    tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Operator,"+");
                    return true;
                }
                case '-':{
                    tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Operator,"-");
                    return true;
                }
                case '/':{
                    tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Operator,"/");
                    return true;
                }
                case '*':{
                    tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Operator,"*");
                    return true;
                }
                case ':':{
                    break;
                }
                case '=':{
                    if(current == ":="){
                        tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Operator,":=");
                    } else{
                        tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Operator,"=");
                    }
                        return true;
                }
                case '(':{
                    tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Separator,"(");
                    return true;
                }
                case ')':{
                    tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Separator,")");
                    return true;
                }
                default:{
                    if(isDigit(c)){
                        if(hasOnlyLetters(current)){
                            tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Identifier,current);
                            return true;
                        }
                    } else if( isLetter(c)||c !='_'){
                        if(hasOnlyDigits(current)){
                            tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Literal,current);
                            return true;
                        }
                    } else {
                        //invalid char
                        auto s = SourceCodeReference(ptr,manager);
                        s.printContext("An invalid character has been used!", 1);
                        return false;
                    }
                }
            }
        } else if(!current.empty()){
            if(current =="PARAM"||current=="VAR"||current=="CONST"||current=="BEGIN"||current=="END"||current=="RETURN"){
                //the position of the last character of the string is the given argument (line,linePos)
                tokens.emplace_back(SourceCodeReference(ptr, manager), TokenTypes::Keyword,current);
            } else if(hasOnlyDigits(current)){
                //if there are only digit it has to be a literal
                tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Literal,current);
            } else{
                tokens.emplace_back(SourceCodeReference(ptr, manager), TokenTypes::Identifier,current);
            }
                position++;
                return true;
        } else {
            position++;
        }
    }
    if(!current.empty()){
        if(current =="PARAM"||current=="VAR"||current=="CONST"||current=="BEGIN"||current=="END"||current=="RETURN"){
            //the position of the last character of the string is the given argument (line,linePos)
            tokens.emplace_back(SourceCodeReference(ptr, manager), TokenTypes::Keyword,current);
            return true;
        } else if(hasOnlyDigits(current)){
            //if there are only digit it has to be a literal
            tokens.emplace_back(SourceCodeReference(ptr, manager), TokenTypes::Literal,current);
            return true;
        } else if(hasOnlyLetters(current)){
            tokens.emplace_back(SourceCodeReference(ptr,manager), TokenTypes::Identifier,current);
            return true;
        }
    }
    auto s = SourceCodeReference(ptr,manager);
    s.printContext("An invalid character has been used!", 1);
    return false;
}
void Tokenizer::parse(SourceCodeManager& sourceCode) {
    const std::string_view& string = sourceCode.source;
    for(size_t position = 0;position<string.size();){
        std::string_view substring = string.substr(position,string.size()-position);
        if(!next(position, substring, sourceCode)){
            tokens.clear();
            break;
        }
    }
}
std::vector<Token>& Tokenizer::getTokens(){
    return tokens;
}
} // namespace lexer
