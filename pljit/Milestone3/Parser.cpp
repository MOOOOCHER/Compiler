#include "Parser.h"
#include <iostream>
namespace parser{
//Helper
void Parser::printDefaultErrorMsg(std::string_view msg){
    Token token = Token(tokenizer.getManager());
    token.sourceCodeReference.printContext(msg);
}
void Parser::printErrorMsg(Token& token, std::string_view msg){
    if(token.getType() != TokenTypes::Invalid){
        token.sourceCodeReference.printContext(msg);
    } else {
        token.sourceCodeReference.printContext(msg);
    }
}
static std::string returnCorrectGenericForErrorMsg(TokenTypes type){
    switch(type){
        case lexer::TokenTypes::Dot: return "'.'";
        case TokenTypes::Identifier: return "an identifier";
        case TokenTypes::Literal: return "a literal";
        case TokenTypes::Invalid: return "";
        case TokenTypes::Comma: return "','";
        case TokenTypes::Semicolon: return "';'";
        case TokenTypes::InitEquals: return "'='";
        case TokenTypes::AssignEquals: return "':='";
        case TokenTypes::OpenBracket: return "'('";
        case TokenTypes::CloseBracket: return "')'";
        case TokenTypes::PlusOperator: return "'+'";
        case TokenTypes::MinusOperator: return "'-'";
        case TokenTypes::MulOperator: return "'*'";
        case TokenTypes::DivOperator: return "'/'";
        case TokenTypes::RETURN: return "'RETURN'";
        case TokenTypes::VAR: return "'VAR'";
        case TokenTypes::PARAM: return "'PARAM'";
        case TokenTypes::CONST: return "'CONST'";
        case TokenTypes::BEGIN: return "'BEGIN'";
        case TokenTypes::END: return "'END'";
        default: return "";
    }
}
static Node::Types getNodeTypeFromTokenType(TokenTypes type){
    switch(type){
        case lexer::TokenTypes::Dot: return Node::Dot;
        case TokenTypes::Comma: return Node::Comma;
        case TokenTypes::Semicolon: return Node::Semicolon;
        case TokenTypes::InitEquals: return Node::InitEquals;
        case TokenTypes::AssignEquals: return Node::AssignEquals;
        case TokenTypes::OpenBracket: return Node::OpenBracket;
        case TokenTypes::CloseBracket: return Node::CloseBracket;
        case TokenTypes::PlusOperator: return Node::PlusOperator;
        case TokenTypes::MinusOperator: return Node::MinusOperator;
        case TokenTypes::MulOperator: return Node::MulOperator;
        case TokenTypes::DivOperator: return Node::DivOperator;
        case TokenTypes::RETURN: return Node::RETURN;
        case TokenTypes::VAR: return Node::VAR;
        case TokenTypes::PARAM: return Node::PARAM;
        case TokenTypes::CONST: return Node::CONST;
        case TokenTypes::BEGIN: return Node::BEGIN;
        case TokenTypes::END: return Node::END;
        default: return Node::Invalid;
    }
}
//Refactor--------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::refactorDeclList(auto (Parser::*func)(), Node::Types nodeType){
    auto element = (this->*func)();
    if(element){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,tokenizer.getManager());
        node->children.push_back(std::move(element));
        //check for more
        while (tokenizer.hasNext()) {
            auto separator = tokenizer.next();
            if(separator.getType() == TokenTypes::Invalid ){
                return nullptr;
            } else if(separator.getType() == TokenTypes::Identifier ||separator.getType()==TokenTypes::Dot){
                printErrorMsg(separator,"error: expected ','");
                return nullptr;
            } else if(separator.getType() != lexer::TokenTypes::Comma){
                backtrackToken = separator;
                return node;
            }
            element = (this->*func)();
            if(!element) {
                return nullptr;
            }
            node->children.push_back(std::make_unique<GenericNode>(separator.sourceCodeReference,tokenizer.getManager(),Node::Types::Comma));
            node->children.push_back(std::move(element));
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::refactorDeclaration(auto (Parser::*func)(), Node::Types startingKeyword, TokenTypes endingKeyword, Node::Types nodeType){
    //we only get in here if the check was successful
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,tokenizer.getManager());
    node->children.push_back(std::make_unique<GenericNode>(backtrackToken.sourceCodeReference,tokenizer.getManager(),startingKeyword));
    resetBacktrackToken();
    auto declList = (this->*func)();
    if(!declList){
        return nullptr;
    }
    //we have already evaluated this token, but it was optional, so we may need to evaluate it again
    if(backtrackToken.getType() == endingKeyword){
        node->children.push_back(std::move(declList));
        node->children.push_back(std::make_unique<GenericNode>(backtrackToken.sourceCodeReference,tokenizer.getManager(), getNodeTypeFromTokenType(endingKeyword)));
        resetBacktrackToken();
        return node;
    }
    printErrorMsg(backtrackToken,"error: expected "+returnCorrectGenericForErrorMsg(endingKeyword)+"!");
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::refactorExpression(auto (Parser::*func1)(),auto (Parser::*func2)(), Node::Types nodeType, const TokenTypes operator1, const TokenTypes operator2){
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
    Token token = backtrackToken;
    if(backtrackToken.getType() == TokenTypes::Invalid){
        if(!tokenizer.hasNext()){
            return node;
        }
        token = tokenizer.next();
        if(token.getType() == TokenTypes::Invalid ){
            return nullptr;
        }
    }
    if (token.getType() != operator1 && token.getType() != operator2){
        //no optional component at this level
        backtrackToken = token;
        return node;
    }
    auto sndExpr = (this->*func2)();
    if(!sndExpr){
        return nullptr;
    }
    node->children.push_back(std::make_unique<GenericNode>(token.sourceCodeReference,tokenizer.getManager(), getNodeTypeFromTokenType(token.getType())));
    node->children.push_back(std::move(sndExpr));
    return node;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------

std::unique_ptr<IdentifierNode> Parser::expectIdentifierNode(){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg( "error: an identifier is expected!");
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == TokenTypes::Invalid ){
        return nullptr;
    } else if(token.getType() == lexer::TokenTypes::Identifier){
        return std::make_unique<IdentifierNode>(token.sourceCodeReference,tokenizer.getManager());
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
    if(token.getType() == TokenTypes::Invalid ){
        return nullptr;
    } else if(token.getType() == lexer::TokenTypes::Literal){
        //get number from string
        return std::make_unique<LiteralNode>(token.sourceCodeReference,tokenizer.getManager(),token.getValue());
    } else {
        printErrorMsg(token, "error: a literal is expected!");
        return nullptr;
    }
}

std::unique_ptr<GenericNode> Parser::expectGenericNode(TokenTypes type){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: expected"+returnCorrectGenericForErrorMsg(type));
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == TokenTypes::Invalid ){
        return nullptr;
    } else if(type == token.getType()){
        return std::make_unique<GenericNode>(token.sourceCodeReference,tokenizer.getManager(), getNodeTypeFromTokenType(type));
    }
    printErrorMsg(token,"error: expected"+returnCorrectGenericForErrorMsg(type));
    return nullptr;
}
//----------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectFunctionDefinition(){
    if(!tokenizer.hasNext()){
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::FunctionDefinition,tokenizer.getManager());
    auto token = tokenizer.next();
    if(token.getType() == TokenTypes::Invalid ){
        return nullptr;
    } else if (token.getType() == TokenTypes::PARAM){
        backtrackToken = token;
        auto paramDecl = expectParameterDeclaration();
        if(!paramDecl ||!tokenizer.hasNext()){
            return nullptr;
        }
        node->children.push_back(std::move(paramDecl));
        token = tokenizer.next();
        if(token.getType() == TokenTypes::Invalid ){
            return nullptr;
        }
    }
    if (token.getType() == TokenTypes::VAR){
        backtrackToken = token;
        auto varDecl = expectVariableDeclaration();
        if(!varDecl || !tokenizer.hasNext()){
            return nullptr;
        }
        node->children.push_back(std::move(varDecl));
        token = tokenizer.next();
        if(token.getType() == TokenTypes::Invalid ){
            return nullptr;
        }
    }
    if (token.getType() == TokenTypes::CONST){
        backtrackToken = token;
        auto constDecl = expectConstantDeclaration();
        if(!constDecl || !tokenizer.hasNext()){
            return nullptr;
        }
        node->children.push_back(std::move(constDecl));
        token = tokenizer.next();
        if(token.getType() == TokenTypes::Invalid ){
            return nullptr;
        }
    }
    if (token.getType() != TokenTypes::BEGIN){
        if(token.getType() == lexer::TokenTypes::Identifier){
            printErrorMsg(token,"error: unknown keyword!");
        }
        else if(token.getType() != lexer::TokenTypes::VAR && token.getType() != lexer::TokenTypes::CONST && token.getType() != lexer::TokenTypes::PARAM){
            printErrorMsg(token,"error: expected 'BEGIN'!");
        } else {
            printErrorMsg(token,"error: wrong order of keywords!");
        }
        return nullptr;
    }
    backtrackToken = token;
    auto compStatement = expectCompoundStatement();
    if(!compStatement){
        return nullptr;
    }
    auto dot = expectGenericNode(TokenTypes::Dot);
    if(!dot){
        return nullptr;
    }
    node->children.push_back(std::move(compStatement));
    node->children.push_back(std::move(dot));
    if(tokenizer.hasNext()){
        auto invalid = tokenizer.next();
        printErrorMsg(invalid,"error: Invalid token after function definition!");
        return nullptr;
    }
    return node;
}
std::unique_ptr<NonTerminalNode> Parser::expectParameterDeclaration(){
    return refactorDeclaration(&Parser::expectDeclaratorList, Node::Types::PARAM, TokenTypes::Semicolon, Node::Types::ParameterDeclaration);
}
std::unique_ptr<NonTerminalNode> Parser::expectVariableDeclaration(){
    return refactorDeclaration(&Parser::expectDeclaratorList, Node::Types::VAR,TokenTypes::Semicolon, Node::Types::VariableDeclaration);
}
std::unique_ptr<NonTerminalNode> Parser::expectConstantDeclaration(){
    return refactorDeclaration(&Parser::expectInitDeclaratorList, Node::Types::CONST,TokenTypes::Semicolon, Node::Types::ConstantDeclaration);
}

//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectDeclaratorList(){
    return refactorDeclList(&Parser::expectIdentifierNode,Node::DeclaratorList);
}
std::unique_ptr<NonTerminalNode> Parser::expectInitDeclaratorList(){
    return refactorDeclList(&Parser::expectInitDeclarator,Node::InitDeclaratorList);
}
std::unique_ptr<NonTerminalNode> Parser::expectInitDeclarator(){
    auto identifier = expectIdentifierNode();
    if(!identifier) {
        return nullptr;
    }
    auto equals = expectGenericNode(lexer::TokenTypes::InitEquals);
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
    return refactorDeclaration(&Parser::expectStatementList, Node::Types::BEGIN,TokenTypes::END, Node::Types::CompoundStatement);
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
               if(separator.getType() == TokenTypes::Invalid ){
                   return nullptr;
               }
            }
            if(separator.getType() == lexer::TokenTypes::END) {
                backtrackToken = separator;
                return node;
            } else if(separator.getType()!=TokenTypes::Semicolon){
                printErrorMsg(separator,"error: expected ';'");
                return nullptr;
            }
            element = expectStatement();
            if(!element) {
                return nullptr;
            }
            node->children.push_back(std::make_unique<GenericNode>(separator.sourceCodeReference,tokenizer.getManager(),Node::Types::Semicolon));
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
    if(token.getType() == TokenTypes::Invalid ){
        return nullptr;
    } else if(token.getType() == TokenTypes::RETURN){
        //return additive expression
        auto addExpr = expectAdditiveExpression();
        if(addExpr){
            std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::Statement,tokenizer.getManager());
            node->children.push_back(std::make_unique<GenericNode>(token.sourceCodeReference, tokenizer.getManager(),Node::Types::RETURN));
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
        printErrorMsg(backtrackToken,"error: expected identifier or 'RETURN' statement!");
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::AssignmentExpression, tokenizer.getManager());
    node->children.push_back(std::make_unique<IdentifierNode>(backtrackToken.sourceCodeReference,tokenizer.getManager()));
    resetBacktrackToken();

    auto equals = expectGenericNode(lexer::TokenTypes::AssignEquals);
    if(!equals){
        return nullptr;
    }
    auto expr = expectAdditiveExpression();
    if (!expr) {
        return nullptr;
    }
    node->children.push_back(std::move(equals));
    node->children.push_back(std::move(expr));
    return node;
}

std::unique_ptr<NonTerminalNode> Parser::expectAdditiveExpression(){
    return refactorExpression(&Parser::expectMultiplicativeExpression,&Parser::expectAdditiveExpression,Node::Types::AdditiveExpression, TokenTypes::PlusOperator,TokenTypes::MinusOperator);
}
std::unique_ptr<NonTerminalNode> Parser::expectMultiplicativeExpression(){
    return refactorExpression(&Parser::expectUnaryExpression,&Parser::expectMultiplicativeExpression,Node::Types::MultiplicativeExpression, TokenTypes::MulOperator,TokenTypes::DivOperator);
}

std::unique_ptr<NonTerminalNode> Parser::expectUnaryExpression(){
    //check for optional
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: expected expression!");
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == TokenTypes::Invalid ){
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::UnaryExpression,tokenizer.getManager());

    if(token.getType() == lexer::TokenTypes::PlusOperator || token.getType() == lexer::TokenTypes::MinusOperator){
        node->children.push_back(std::make_unique<GenericNode>(token.sourceCodeReference, tokenizer.getManager(), getNodeTypeFromTokenType(token.getType())));
        resetBacktrackToken();
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
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::PrimaryExpression,tokenizer.getManager());
    Token token = backtrackToken;
    if(backtrackToken.getType() == TokenTypes::Invalid){
        if(!tokenizer.hasNext()){
            printDefaultErrorMsg("error: expected expression!");
            return nullptr;
        }
        token = tokenizer.next();
        if(token.getType() == TokenTypes::Invalid ){
            return nullptr;
        }
    }
    if(token.getType() == TokenTypes::Identifier){
        node->children.push_back(std::make_unique<IdentifierNode>(token.sourceCodeReference,tokenizer.getManager()));
        resetBacktrackToken();
        return node;
    } else if(token.getType() == TokenTypes::Literal){
        node->children.push_back(std::make_unique<LiteralNode>(token.sourceCodeReference,tokenizer.getManager(),token.getValue()));
        resetBacktrackToken();
        return node;
    } else if (token.getType() == TokenTypes::OpenBracket){
        resetBacktrackToken();

        auto addExpr = expectAdditiveExpression();
        if(!addExpr){
            return nullptr;
        }
        Token token2 = backtrackToken;
        if(backtrackToken.getType() == TokenTypes::Invalid){
            if(!tokenizer.hasNext()){
                return node;
            }
            token2 = tokenizer.next();
            if(token.getType() == TokenTypes::Invalid ){
                return nullptr;
            }
        }
        if(token2.getType() != lexer::TokenTypes::CloseBracket){
            printErrorMsg(token2, "error: expected ')'!");
            return nullptr;
        }
        resetBacktrackToken();
        node->children.push_back(std::make_unique<GenericNode>(token.sourceCodeReference, tokenizer.getManager(), Node::Types::OpenBracket));
        node->children.push_back(std::move(addExpr));
        node->children.push_back(std::make_unique<GenericNode>(token2.sourceCodeReference, tokenizer.getManager(),Node::Types::CloseBracket));
        return node;
    }else{
        printErrorMsg(token, "error: an expression is expected");
        return nullptr;
    }
}
} // namespace parser
