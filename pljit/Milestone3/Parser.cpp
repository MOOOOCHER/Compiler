#include "Parser.h"
#include <charconv>
#include <iostream>
namespace parser{
//Helper
void Parser::printDefaultErrorMsg(std::string_view msg){
    Token token = Token(tokenizer.getManager());
    token.sourceCodeReference.printContext(msg,1);
}
void Parser::printErrorMsg(Token& token, std::string_view msg){
    if(token.getType() != TokenTypes::Invalid){
        token.sourceCodeReference.printContext(msg,token.getText().size());
    } else {
        token.sourceCodeReference.printContext(msg,1);
    }
}
//Refactor--------------------------------------------------------------------------------------------------

std::unique_ptr<NonTerminalNode> Parser::refactorExpression(auto (Parser::*func1)(),auto (Parser::*func2)(), Node::Types nodeType, const std::string& operator1, const std::string& operator2){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: expected expression");
        return nullptr;
    }
    auto firstExpr = (this->*func1)();
    if(!firstExpr){
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,tokenizer.getManager());
    node->children.push_back(std::move(firstExpr));
    //check for optional
    if(!tokenizer.hasNext()){
        return node;
    }
    if(backtrackToken.getType() != TokenTypes::Operator){
        return node;
    } else if (backtrackToken.getText() != operator1 && backtrackToken.getText() != operator2){
        printErrorMsg(backtrackToken,"error: expected '"+operator1 +"' or '" + operator2 +"'!");
        return nullptr;
    }
    auto sndExpr = (this->*func2)();
    if(!sndExpr){
        return nullptr;
    }
    node->children.push_back(std::make_unique<GenericNode>(backtrackToken.getText(),tokenizer.getManager()));
    node->children.push_back(std::move(sndExpr));
    resetBacktrackToken();
    return node;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------

std::unique_ptr<IdentifierNode> Parser::expectIdentifierNode(){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg( "error: an identifier is expected!");
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == lexer::TokenTypes::Identifier){
        return std::make_unique<IdentifierNode>(token.getText(),tokenizer.getManager());
    } else {
        printErrorMsg(token, "error: an identifier is expected!");
     return nullptr;
 }
}

std::unique_ptr<LiteralNode> Parser::expectLiteralNode(){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: a literal is expected!");
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == lexer::TokenTypes::Literal){
        //get number from string
        unsigned long value;
        std::from_chars(token.getText().data(),token.getText().data()+token.getText().size(),value);
        return std::make_unique<LiteralNode>(value,tokenizer.getManager());
    } else {
        printErrorMsg(token, "error: a literal is expected!");
        return nullptr;
    }
}

std::unique_ptr<GenericNode> Parser::expectGenericNode(const std::string& c){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: expected '"+c+"'");
        return nullptr;
    }
    auto token = tokenizer.next();
    lexer::TokenTypes type = token.getType();
    if((type == lexer::TokenTypes::Operator || type == lexer::TokenTypes::Separator || type == lexer::TokenTypes::Keyword) && token.getText() == c){
        return std::make_unique<GenericNode>(token.getText(),tokenizer.getManager());
    }
    printErrorMsg(token,"error: expected '"+c+"'");
    return nullptr;
}
//----------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectFunctionDefinition(){
    if(!tokenizer.hasNext()){
        std::cout << "Please insert code!" << std::endl;
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::FunctionDefinition,tokenizer.getManager());
    auto token = tokenizer.next();
    if(token.getType() != TokenTypes::Keyword){
        printErrorMsg(token,"error: invalid keyword!");
        return nullptr;
    } else if (token.getText() == "PARAM"){
        auto paramDecl = expectParameterDeclaration();
        if(!paramDecl){
            return nullptr;
        } else if(!tokenizer.hasNext()){
            std::cout << "error: expected 'BEGIN'" << std::endl;
            return nullptr;
        }
        node->children.push_back(std::move(paramDecl));
        token = tokenizer.next();
    }
    if (token.getText() == "VAR"){
        auto varDecl = expectVariableDeclaration();
        if(!varDecl){
            return nullptr;
        }else if(!tokenizer.hasNext()){
            std::cout << "error: expected 'BEGIN'" << std::endl;
            return nullptr;
        }
        node->children.push_back(std::move(varDecl));
        token = tokenizer.next();
    }
    if (token.getText() == "CONST"){
        auto constDecl = expectConstantDeclaration();
        if(!constDecl){
            return nullptr;
        }else if(!tokenizer.hasNext()){
            std::cout << "error: expected 'BEGIN'" << std::endl;
            return nullptr;
        }
        node->children.push_back(std::move(constDecl));
        token = tokenizer.next();
    }
    if (token.getText() != "BEGIN"){
        printErrorMsg(token,"error: expected 'BEGIN'");
        return nullptr;
    }
    auto compStatement = expectCompoundStatement();
    if(!compStatement){
        return nullptr;
    }
    auto dot = expectGenericNode(".");
    if(!dot){
        return nullptr;
    }
    node->children.push_back(std::move(compStatement));
    node->children.push_back(std::move(dot));
    return node;
}
std::unique_ptr<NonTerminalNode> Parser::expectParameterDeclaration(){
    //we only get in here if the check was successful
    auto declList = expectDeclaratorList();
    if(!declList){
        return nullptr;
    }
    //we have already evaluated this token, but it was optional, so we need to evaluate it again
    if(backtrackToken.getType() == TokenTypes::Separator && backtrackToken.getText() == ";"){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::ParameterDeclaration,tokenizer.getManager());
        node->children.push_back(std::make_unique<GenericNode>("PARAM",tokenizer.getManager()));
        node->children.push_back(std::move(declList));
        node->children.push_back(std::make_unique<GenericNode>(";",tokenizer.getManager()));
        resetBacktrackToken();
        return node;
    }
    printErrorMsg(backtrackToken,"error: expected ';'");
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::expectVariableDeclaration(){
    //we only get in here if the check was successful
    auto declList = expectDeclaratorList();
    if(!declList){
        return nullptr;
    }
    //we have already evaluated this token, but it was optional, so we need to evaluate it again
    if(backtrackToken.getType() == TokenTypes::Separator && backtrackToken.getText() == ";"){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::VariableDeclaration,tokenizer.getManager());
        node->children.push_back(std::make_unique<GenericNode>("VAR",tokenizer.getManager()));
        node->children.push_back(std::move(declList));
        node->children.push_back(std::make_unique<GenericNode>(";",tokenizer.getManager()));
        resetBacktrackToken();
        return node;
    }
    printErrorMsg(backtrackToken,"error: expected ';'");
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::expectConstantDeclaration(){
    //we only get in here if the check was successful
    auto declList = expectInitDeclaratorList();
    if(!declList){
        return nullptr;
    }
    //we have already evaluated this token, but it was optional, so we need to evaluate it again
    if(backtrackToken.getType() == TokenTypes::Separator && backtrackToken.getText() == ";"){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::ConstantDeclaration,tokenizer.getManager());
        node->children.push_back(std::make_unique<GenericNode>("CONST",tokenizer.getManager()));
        node->children.push_back(std::move(declList));
        node->children.push_back(std::make_unique<GenericNode>(";",tokenizer.getManager()));
        resetBacktrackToken();
        return node;
    }
    printErrorMsg(backtrackToken,"error: expected ';'");
    return nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectDeclaratorList(){
    auto element = expectIdentifierNode();
    if(element){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::DeclaratorList,tokenizer.getManager());
        node->children.push_back(std::move(element));
        //check for more
        while (tokenizer.hasNext()) {
            auto separator = tokenizer.next();
            if(separator.getType() == TokenTypes::Invalid || separator.getType() == TokenTypes::Identifier){
                    printErrorMsg(separator,"error: expected ','");
                    return nullptr;
            } else if(separator.getText() == ";" || separator.getType()!=TokenTypes::Separator){
                backtrackToken = separator;
                return node;
            } else if(separator.getType()==TokenTypes::Separator && separator.getText() != ","){
                //in case of other separators
                printErrorMsg(separator,"error: expected ','");
                return nullptr;
            }
            element = expectIdentifierNode();
            if(!element) {
                return nullptr;
            }
            node->children.push_back(std::make_unique<GenericNode>(",",tokenizer.getManager()));
            node->children.push_back(std::move(element));
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::expectInitDeclaratorList(){
    auto element = expectInitDeclarator();
    if(element){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::InitDeclaratorList,tokenizer.getManager());
        node->children.push_back(std::move(element));
        //check for more
        while (tokenizer.hasNext()) {
            auto separator = tokenizer.next();
            if(separator.getType() == TokenTypes::Invalid || separator.getType() == TokenTypes::Identifier){
                printErrorMsg(separator,"error: expected ','");
                return nullptr;
            } else if(separator.getText() == ";" || separator.getType() !=TokenTypes::Separator){
                //";" can be the next token
                backtrackToken = separator;
                return node;
            } else if(separator.getType()==TokenTypes::Separator && separator.getText() != ","){
                //in case of other separators
                printErrorMsg(separator,"error: expected ','");
                return nullptr;
            }
            element = expectInitDeclarator();
            if(!element) {
                return nullptr;
            }
            node->children.push_back(std::make_unique<GenericNode>(",",tokenizer.getManager()));
            node->children.push_back(std::move(element));
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::expectInitDeclarator(){
    //return refactorAssignmentInit(&Parser::expectLiteralNode,Node::Types::InitDeclarator,"=");
    auto identifier = expectIdentifierNode();
    if(!identifier) {
        return nullptr;
    }
    auto equals = expectGenericNode("=");
    if(!equals){
        return nullptr;
    }
    auto expr = expectLiteralNode();
    if (!expr) {
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::InitDeclarator, tokenizer.getManager());
    node->children.push_back(std::move(identifier));
    node->children.push_back(std::move(equals));
    node->children.push_back(std::move(expr));
    return node;
}
//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectCompoundStatement(){
    //return refactorDeclaration(&Parser::expectStatementList,Node::Types::CompoundStatement, "BEGIN","END", false);
    auto statementList = expectStatementList();
    if(!statementList){
        return nullptr;
    }
    //we have already evaluated this token, but it was optional, so we need to evaluate it again
    if(backtrackToken.getType() == TokenTypes::Keyword && backtrackToken.getText() == "END"){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::CompoundStatement,tokenizer.getManager());
        node->children.push_back(std::make_unique<GenericNode>("BEGIN",tokenizer.getManager()));
        node->children.push_back(std::move(statementList));
        node->children.push_back(std::make_unique<GenericNode>("END",tokenizer.getManager()));
        resetBacktrackToken();
        return node;
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::expectStatementList(){
    auto element = expectStatement();
    if(element){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::StatementList,tokenizer.getManager());
        node->children.push_back(std::move(element));
        //check for more
        while (tokenizer.hasNext()) {
            Token separator = backtrackToken;
            if(backtrackToken.getType() == TokenTypes::Invalid){
               separator = tokenizer.next();
            }
            if(separator.getType() == TokenTypes::Invalid || separator.getType() == TokenTypes::Identifier || separator.getText() == "RETURN"){
                printErrorMsg(separator,"error: expected ';'");
                return nullptr;
            } else if(separator.getText() == "END" && separator.getType()!=TokenTypes::Separator){
                backtrackToken = separator;
                return node;
            } else if(separator.getType()==TokenTypes::Separator && separator.getText() != ";"){
                //in case of other separators
                printErrorMsg(separator,"error: expected ';'");
                return nullptr;
            }
            element = expectStatement();
            if(!element) {
                return nullptr;
            }
            node->children.push_back(std::make_unique<GenericNode>(";",tokenizer.getManager()));
            node->children.push_back(std::move(element));
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::expectStatement(){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: expected statement!");
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == TokenTypes::Keyword && token.getText() == "RETURN"){
        //return additive expression
        auto addExpr = expectAdditiveExpression();
        if(addExpr){
            std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::Statement,tokenizer.getManager());
            node->children.push_back(std::make_unique<GenericNode>("RETURN", tokenizer.getManager()));
            node->children.push_back(std::move(addExpr));
            return node;
        } else{
            return nullptr;
        }
    } else {
        backtrackToken = token;
        auto assignment = expectAssignmentExpression();
        if(assignment){
            std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::Statement,tokenizer.getManager());
            node->children.push_back(std::move(assignment));
            return node;
        } else{
            return nullptr;
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectAssignmentExpression(){
    if(backtrackToken.getType() != TokenTypes::Identifier) {
        printErrorMsg(backtrackToken,"error: expected identifier!");
        return nullptr;
    }
    auto equals = expectGenericNode(":=");
    if(!equals){
        return nullptr;
    }
    auto expr = expectAdditiveExpression();
    if (!expr) {
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::InitDeclarator, tokenizer.getManager());
    node->children.push_back(std::make_unique<IdentifierNode>(backtrackToken.getText(),tokenizer.getManager()));
    node->children.push_back(std::move(equals));
    node->children.push_back(std::move(expr));
    resetBacktrackToken();
    return node;
}

std::unique_ptr<NonTerminalNode> Parser::expectAdditiveExpression(){
    auto result = refactorExpression(&Parser::expectMultiplicativeExpression,&Parser::expectAdditiveExpression,Node::Types::AdditiveExpression,"+","-");
    return result;
}
std::unique_ptr<NonTerminalNode> Parser::expectMultiplicativeExpression(){
    auto result =  refactorExpression(&Parser::expectUnaryExpression,&Parser::expectMultiplicativeExpression,Node::Types::MultiplicativeExpression,"*","/");
    return result;
}
std::unique_ptr<NonTerminalNode> Parser::expectUnaryExpression(){
    //check for optional
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: expected expression!");
        return nullptr;
    }
    auto token = tokenizer.next();
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::UnaryExpression,tokenizer.getManager());

    if(token.getType() == TokenTypes::Operator){
        if(token.getText() == "+" && token.getText() == "-"){
            node->children.push_back(std::make_unique<GenericNode>(token.getText(), tokenizer.getManager()));
        } else {
            printDefaultErrorMsg("error: expected '+' or '-'!");
            return nullptr;
        }
    } else {
        backtrackToken = token;
    }
    auto primaryExpr = expectPrimaryExpression();
    if(primaryExpr){
        node->children.push_back(std::move(primaryExpr));
        return node;
    } else {
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectPrimaryExpression(){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: expected expression!");
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::PrimaryExpression,tokenizer.getManager());
    if(backtrackToken.getType() == TokenTypes::Identifier){
        node->children.push_back(std::make_unique<IdentifierNode>(backtrackToken.getText(),tokenizer.getManager()));
        resetBacktrackToken();
        return node;
    } else if(backtrackToken.getType() == TokenTypes::Literal){
        unsigned long value;
        std::from_chars(backtrackToken.getText().data(),backtrackToken.getText().data()+backtrackToken.getText().size(),value);
        node->children.push_back(std::make_unique<LiteralNode>(value,tokenizer.getManager()));
        resetBacktrackToken();
        return node;
    } else if (backtrackToken.getType() == TokenTypes::Separator){
        if(backtrackToken.getText() != "("){
            printErrorMsg(backtrackToken, "error: expected '('");
        }
        resetBacktrackToken();

        auto addExpr = expectAdditiveExpression();
        if(!addExpr){
            return nullptr;
        }
        auto endingBracket = expectGenericNode(")");
        if(!endingBracket){
            return nullptr;
        }
        node->children.push_back(std::make_unique<GenericNode>(backtrackToken.getText(), tokenizer.getManager()));
        node->children.push_back(std::move(addExpr));
        node->children.push_back(std::move(endingBracket));
        return node;
    }else{
        printErrorMsg(backtrackToken, "error: an expression is expected");
        return nullptr;
    }

}
} // namespace parser
