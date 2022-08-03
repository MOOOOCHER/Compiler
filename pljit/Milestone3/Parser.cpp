#include "Parser.h"
namespace parser{
//Helper
void Parser::printDefaultErrorMsg(std::string_view msg){
    Token token = Token(tokenizer.getManager());
    token.sourceCodeReference.printContext(msg);
}
void Parser::printErrorMsg(Token& token, std::string_view msg){
        token.sourceCodeReference.printContext(msg);
}
static std::string returnCorrectGenericForErrorMsg(TokenTypes type){
    switch(type){
        case lexer::TokenTypes::Dot: return "'.'";
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
        std::vector<std::unique_ptr<Node>> childVec;
        childVec.push_back(std::move(element));
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
                return std::make_unique<NonTerminalNode>(nodeType, std::move(childVec));
            }
            element = (this->*func)();
            if(!element) {
                return nullptr;
            }
            childVec.push_back(std::make_unique<GenericNode>(separator.sourceCodeReference, Node::Types::Comma));
            childVec.push_back(std::move(element));
        }
        return std::make_unique<NonTerminalNode>(nodeType, std::move(childVec));
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::refactorDeclaration(auto (Parser::*func)(), Node::Types startingKeyword, TokenTypes endingKeyword, Node::Types nodeType){
    //we only get in here if the check was successful
    std::vector<std::unique_ptr<Node>> childVec;
    childVec.push_back(std::make_unique<GenericNode>(backtrackToken.sourceCodeReference,startingKeyword));
    resetBacktrackToken();
    auto declList = (this->*func)();
    if(!declList){
        return nullptr;
    }
    //we have already evaluated this token, but it was optional, so we may need to evaluate it again
    if(backtrackToken.getType() == endingKeyword){
        childVec.push_back(std::move(declList));
        childVec.push_back(std::make_unique<GenericNode>(backtrackToken.sourceCodeReference, getNodeTypeFromTokenType(endingKeyword)));
        resetBacktrackToken();
        return std::make_unique<NonTerminalNode>(nodeType, std::move(childVec));
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
    std::vector<std::unique_ptr<Node>> childVec;
    childVec.push_back(std::move(firstExpr));
    //check for optional
    Token token = backtrackToken;
    if(backtrackToken.getType() == TokenTypes::Invalid){
        if(!tokenizer.hasNext()){
            return std::make_unique<NonTerminalNode>(nodeType, std::move(childVec));
        }
        token = tokenizer.next();
        if(token.getType() == TokenTypes::Invalid ){
            return nullptr;
        }
    }
    if (token.getType() != operator1 && token.getType() != operator2){
        //no optional component at this level
        backtrackToken = token;
        return std::make_unique<NonTerminalNode>(nodeType, std::move(childVec));
    }
    auto sndExpr = (this->*func2)();
    if(!sndExpr){
        return nullptr;
    }
    childVec.push_back(std::make_unique<GenericNode>(token.sourceCodeReference, getNodeTypeFromTokenType(token.getType())));
    childVec.push_back(std::move(sndExpr));
    return std::make_unique<NonTerminalNode>(nodeType, std::move(childVec));
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
        return std::make_unique<IdentifierNode>(token.sourceCodeReference, token.sourceCodeReference.getText());
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
        return std::make_unique<LiteralNode>(token.sourceCodeReference,token.getValue());
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
        return std::make_unique<GenericNode>(token.sourceCodeReference,getNodeTypeFromTokenType(type));
    }
    printErrorMsg(token,"error: expected"+returnCorrectGenericForErrorMsg(type));
    return nullptr;
}
//----------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectFunctionDefinition(){
    if(!tokenizer.hasNext()){
        return nullptr;
    }
    std::vector<std::unique_ptr<Node>> childVec;
    auto token = tokenizer.next();

    auto addParseNode = [this,&childVec, &token](auto func){
        backtrackToken = token;
        auto child = (this->*func)();
        if(!child ||!tokenizer.hasNext()){
            return false;
        }
        childVec.push_back(std::move(child));
        //get next token, so we can directly compare the token type with the next section (PARAM,VAR,CONST,BEGIN)
        token = tokenizer.next();
        if(token.getType() == TokenTypes::Invalid ){
            return false;
        }
        return true;
    };

    if(token.getType() == TokenTypes::Invalid
        || (token.getType() == TokenTypes::PARAM && !addParseNode(&Parser::expectParameterDeclaration))
        || (token.getType() == TokenTypes::VAR && !addParseNode(&Parser::expectVariableDeclaration))
        || (token.getType() == TokenTypes::CONST && !addParseNode(&Parser::expectConstantDeclaration))){
        return nullptr;
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
    childVec.push_back(std::move(compStatement));
    childVec.push_back(std::move(dot));
    if(tokenizer.hasNext()){
        auto invalid = tokenizer.next();
        printErrorMsg(invalid,"error: Invalid token after function definition!");
        return nullptr;
    }
    return std::make_unique<NonTerminalNode>(Node::Types::FunctionDefinition, std::move(childVec));
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
    std::vector<std::unique_ptr<Node>> childVec;
    childVec.push_back(std::move(identifier));
    childVec.push_back(std::move(equals));
    childVec.push_back(std::move(expr));
    return std::make_unique<NonTerminalNode>(Node::Types::InitDeclarator, std::move(childVec));
}
//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectCompoundStatement(){
    return refactorDeclaration(&Parser::expectStatementList, Node::Types::BEGIN,TokenTypes::END, Node::Types::CompoundStatement);
}
std::unique_ptr<NonTerminalNode> Parser::expectStatementList(){
    auto element = expectStatement();
    if(element){
        std::vector<std::unique_ptr<Node>> childVec;
        childVec.push_back(std::move(element));
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
                return std::make_unique<NonTerminalNode>(Node::Types::StatementList, std::move(childVec));
            } else if(separator.getType()!=TokenTypes::Semicolon){
                printErrorMsg(separator,"error: expected ';'");
                return nullptr;
            }
            element = expectStatement();
            if(!element) {
                return nullptr;
            }
            childVec.push_back(std::make_unique<GenericNode>(separator.sourceCodeReference,Node::Types::Semicolon));
            childVec.push_back(std::move(element));
        }
        return std::make_unique<NonTerminalNode>(Node::Types::StatementList, std::move(childVec));
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
            std::vector<std::unique_ptr<Node>> childVec;
            childVec.push_back(std::make_unique<GenericNode>(token.sourceCodeReference, Node::Types::RETURN));
            childVec.push_back(std::move(addExpr));
            return std::make_unique<NonTerminalNode>(Node::Types::Statement, std::move(childVec));
        } else{
            return nullptr;
        }
    } else {
        backtrackToken = token;
        auto assignment = expectAssignmentExpression();
        if(assignment){
            std::vector<std::unique_ptr<Node>> childVec;
            childVec.push_back(std::move(assignment));
            return std::make_unique<NonTerminalNode>(Node::Types::Statement, std::move(childVec));
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
    std::vector<std::unique_ptr<Node>> childVec;
    childVec.push_back(std::make_unique<IdentifierNode>(backtrackToken.sourceCodeReference,backtrackToken.sourceCodeReference.getText()));
    resetBacktrackToken();

    auto equals = expectGenericNode(lexer::TokenTypes::AssignEquals);
    if(!equals){
        return nullptr;
    }
    auto expr = expectAdditiveExpression();
    if (!expr) {
        return nullptr;
    }
    childVec.push_back(std::move(equals));
    childVec.push_back(std::move(expr));
    return std::make_unique<NonTerminalNode>(Node::Types::AssignmentExpression, std::move(childVec));
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
    std::vector<std::unique_ptr<Node>> childVec;

    if(token.getType() == lexer::TokenTypes::PlusOperator || token.getType() == lexer::TokenTypes::MinusOperator){
        childVec.push_back(std::make_unique<GenericNode>(token.sourceCodeReference, getNodeTypeFromTokenType(token.getType())));
        resetBacktrackToken();
    } else {
        backtrackToken = token;
    }

    auto primaryExpr = expectPrimaryExpression();
    if(primaryExpr){
        childVec.push_back(std::move(primaryExpr));
        return std::make_unique<NonTerminalNode>(Node::Types::UnaryExpression, std::move(childVec));
    } else {
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectPrimaryExpression(){
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

    std::vector<std::unique_ptr<Node>> childVec;
    if(token.getType() == TokenTypes::Identifier){
        childVec.push_back(std::make_unique<IdentifierNode>(token.sourceCodeReference, token.sourceCodeReference.getText()));
        resetBacktrackToken();
        return std::make_unique<NonTerminalNode>(Node::Types::PrimaryExpression, std::move(childVec));
    } else if(token.getType() == TokenTypes::Literal){
        childVec.push_back(std::make_unique<LiteralNode>(token.sourceCodeReference,token.getValue()));
        resetBacktrackToken();
        return std::make_unique<NonTerminalNode>(Node::Types::PrimaryExpression, std::move(childVec));
    } else if (token.getType() == TokenTypes::OpenBracket){
        resetBacktrackToken();

        auto addExpr = expectAdditiveExpression();
        if(!addExpr){
            return nullptr;
        }
        Token token2 = backtrackToken;
        if(backtrackToken.getType() == TokenTypes::Invalid){
            if(!tokenizer.hasNext()){
                std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::PrimaryExpression, std::move(childVec));
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
        childVec.push_back(std::make_unique<GenericNode>(token.sourceCodeReference, Node::Types::OpenBracket));
        childVec.push_back(std::move(addExpr));
        childVec.push_back(std::make_unique<GenericNode>(token2.sourceCodeReference, Node::Types::CloseBracket));
        return std::make_unique<NonTerminalNode>(Node::Types::PrimaryExpression, std::move(childVec));
    }else{
        printErrorMsg(token, "error: an expression is expected");
        return nullptr;
    }
}
} // namespace parser
