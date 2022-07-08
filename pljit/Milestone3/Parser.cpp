#include "Parser.h"
#include <charconv>
#include <iostream>
namespace parser{
//TODO umöndern, sodass wir next() und hasNext() haben
//Helper
void Parser::printErrorMsg(Token token, std::string_view msg){
        token.sourceCodeReference.printContext(msg,token.getText().size());
}
//Refactor--------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::refactorList(auto (Parser::*func)(), Node::Types nodeType, lexer::TokenTypes tokenType,const std::string& separatorType){
    auto element = (this->*func)();
    if(element){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,tokenizer.getManager());
        node->children.push_back(std::move(element));
        //check for more

        while (tokenizer.hasNext()) {
                auto separator = expectGenericNode(separatorType, true);
                if (!separator) {
                    //--currentPos;
                    return node;
                }
                element = (this->*func)();
                if(!element) {
                    return nullptr;
                }
                node->children.push_back(std::move(separator));
                node->children.push_back(std::move(element));
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::refactorAssignmentInit(auto (Parser::*func)(), Node::Types nodeType, const std::string& assignmentType){
    auto identifier = expectIdentifierNode(false);
    if(!identifier) {
        return nullptr;
    }
    auto equals = expectGenericNode(assignmentType, false);
    if(!equals){
        return nullptr;
    }
    auto expr = (this->*func)();
    if (!expr) {
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType, tokenizer.getManager());
    node->children.push_back(std::move(identifier));
    node->children.push_back(std::move(equals));
    node->children.push_back(std::move(expr));
    return node;
}
std::unique_ptr<NonTerminalNode> Parser::refactorDeclaration(auto (Parser::*func)(), Node::Types nodeType, const std::string& keywordType,const std::string& endingKeyword, bool optional){
    auto keyword = expectGenericNode(keywordType, optional);
    if(!keyword){
        return nullptr;
    }
    auto declList = (this->*func)();
    if(!declList){
        return nullptr;
    }
    auto ending = expectGenericNode(endingKeyword, optional);
    if(!ending){
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,tokenizer.getManager());
    node->children.push_back(std::move(keyword));
    node->children.push_back(std::move(declList));
    node->children.push_back(std::move(ending));
    return node;
}
std::unique_ptr<NonTerminalNode> Parser::refactorExpression(auto (Parser::*func1)(),auto (Parser::*func2)(), Node::Types nodeType, const std::string& operator1, const std::string& operator2){
    if(!tokenizer.hasNext()){
        return nullptr;
    }
    auto firstExpr = (this->*func1)();
    if(!firstExpr){
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,tokenizer.getManager());
    node->children.push_back(std::move(firstExpr));
    //check for optional
    auto firstOperator = expectGenericNode(operator1, true);
    if(firstOperator){
        node->children.push_back(std::move(firstOperator));
    } else {
        auto sndOperator = expectGenericNode(operator2, true);
        if(sndOperator) {
            node->children.push_back(std::move(sndOperator));
        } else {    //no optional elements
           // --currentPos;
            return node;
        }
    }
    auto sndExpr = (this->*func2)();
    if(!sndExpr){
        return nullptr;
    }
    node->children.push_back(std::move(sndExpr));
    return node;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IdentifierNode> Parser::expectIdentifierNode(){
    return expectIdentifierNode(false);
}
std::unique_ptr<IdentifierNode> Parser::expectIdentifierNode(bool optional){
    if(!tokenizer.hasNext()){
        if(!optional) {
            printErrorMsg(Token(tokenizer.getManager()), "error: an identifier is expected!");
        }
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == lexer::TokenTypes::Identifier){
        return std::make_unique<IdentifierNode>(token.getText(),tokenizer.getManager());
    } else {
        if(!optional) {
            printErrorMsg(token, "error: an identifier is expected!");
     }
     return nullptr;
 }
}
std::unique_ptr<LiteralNode> Parser::expectLiteralNode(){
    return expectLiteralNode(false);
}
std::unique_ptr<LiteralNode> Parser::expectLiteralNode( bool optional){
    if(!tokenizer.hasNext()){
        if(!optional){
            printErrorMsg(Token(tokenizer.getManager()),"error: a literal is expected!");
        }
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == lexer::TokenTypes::Literal){
        //get number from string
        unsigned long value;
        std::from_chars(token.getText().data(),token.getText().data()+token.getText().size(),value);
        return std::make_unique<LiteralNode>(value,tokenizer.getManager());
    } else {
        if(!optional) {
            printErrorMsg(token, "error: a literal is expected!");
        }
        return nullptr;
    }
}

std::unique_ptr<GenericNode> Parser::expectGenericNode(const std::string& c, bool optional){
    if(!tokenizer.hasNext()){
        if(!optional){
            printErrorMsg(Token(tokenizer.getManager()),"error: expected '"+c+"'");
        }
        return nullptr;
    }
    auto token = tokenizer.next();
    lexer::TokenTypes type = token.getType();
    if((type == lexer::TokenTypes::Operator || type == lexer::TokenTypes::Separator || type == lexer::TokenTypes::Keyword) && token.getText() == c){
        return std::make_unique<GenericNode>(token.getText(),tokenizer.getManager());
    }
    if(!optional) {
        printErrorMsg(token,"error: expected '"+c+"'");
    }
    return nullptr;
}
//----------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectFunctionDefinition(){
    if(tokenizer.getManager().source.empty()){
        std::cout << "Please insert code!" << std::endl;
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::FunctionDefinition,tokenizer.getManager());
    auto paramDecl = expectParameterDeclaration(true);
    if(paramDecl){
        node->children.push_back(std::move(paramDecl));
    }
    auto varDecl = expectVariableDeclaration(true);
    if(varDecl){
        node->children.push_back(std::move(varDecl));
    }
    auto constDecl = expectConstantDeclaration(true);
    if(constDecl){
        node->children.push_back(std::move(constDecl));
    }
    auto compStatement = expectCompoundStatement();
    if(!compStatement){
        return nullptr;
    }
    auto dot = expectGenericNode(".", false);
    if(!dot){
        return nullptr;
    }
    node->children.push_back(std::move(compStatement));
    node->children.push_back(std::move(dot));
    return node;
}
std::unique_ptr<NonTerminalNode> Parser::expectParameterDeclaration(){
    return expectParameterDeclaration(false);
}
std::unique_ptr<NonTerminalNode> Parser::expectVariableDeclaration(){
    return expectVariableDeclaration(false);
}
std::unique_ptr<NonTerminalNode> Parser::expectConstantDeclaration(){
    return expectConstantDeclaration(false);
}
std::unique_ptr<NonTerminalNode> Parser::expectParameterDeclaration(bool optional){
    return refactorDeclaration(&Parser::expectDeclaratorList,Node::Types::ParameterDeclaration, "PARAM",";", optional);
}
std::unique_ptr<NonTerminalNode> Parser::expectVariableDeclaration(bool optional){
    return refactorDeclaration(&Parser::expectDeclaratorList,Node::Types::VariableDeclaration, "VAR",";", optional);
}
std::unique_ptr<NonTerminalNode> Parser::expectConstantDeclaration(bool optional){
    return refactorDeclaration(&Parser::expectInitDeclaratorList,Node::Types::ConstantDeclaration, "CONST",";", optional);
}
//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectDeclaratorList(){
    return refactorList(&Parser::expectIdentifierNode, Node::Types::DeclaratorList, lexer::TokenTypes::Identifier, ",");
}
std::unique_ptr<NonTerminalNode> Parser::expectInitDeclaratorList(){
    return refactorList(&Parser::expectInitDeclarator, Node::Types::InitDeclaratorList, lexer::TokenTypes::Identifier, ",");
}
std::unique_ptr<NonTerminalNode> Parser::expectInitDeclarator(){
    return refactorAssignmentInit(&Parser::expectLiteralNode,Node::Types::InitDeclarator,"=");
}
//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectCompoundStatement(){
    return refactorDeclaration(&Parser::expectStatementList,Node::Types::CompoundStatement, "BEGIN","END", false);
}
std::unique_ptr<NonTerminalNode> Parser::expectStatementList(){
    auto statement = expectStatement();
    if(statement){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::StatementList,tokenizer.getManager());
        node->children.push_back(std::move(statement));
        //check for more identifier
        while (tokenizer.hasNext()) {
            auto nextToken = tokenizer.next();
            if(nextToken.getType() == lexer::TokenTypes::Identifier || (nextToken.getType() == lexer::TokenTypes::Keyword && nextToken.getText() == "RETURN")){
                //no separator
                printErrorMsg(nextToken,"error: expected ';'");
                return nullptr;
            } else if(nextToken.getType() == lexer::TokenTypes::Separator){
                auto separator = expectGenericNode(";", false);
                if (!separator) {   //wrong separator was used
                   return nullptr;
                }
                statement = expectStatement();
                if(!statement) {
                    return nullptr;
                }
                node->children.push_back(std::move(separator));
                node->children.push_back(std::move(statement));
            } else {
                break;
            }
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::expectStatement(){
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::Statement,tokenizer.getManager());
    auto returnStatement = expectGenericNode("RETURN", true);
    if(returnStatement){
        //return additive expression
        auto addExpr = expectAdditiveExpression();
        if(addExpr){
            node->children.push_back(std::move(returnStatement));
            node->children.push_back(std::move(addExpr));
            return node;
        } else{
            return nullptr;
        }
    } else{
        auto assignment = expectAssignmentExpression();
        if(assignment){
            node->children.push_back(std::move(assignment));
            return node;
        } else{
            return nullptr;
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectAssignmentExpression(){
    return refactorAssignmentInit(&Parser::expectAdditiveExpression,Node::Types::AssignmentExpression,":=");
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
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::UnaryExpression,tokenizer.getManager());
    auto plusOperator = expectGenericNode("+", true);
    if(plusOperator){
        node->children.push_back(std::move(plusOperator));
    } else {
        auto minusOperator = expectGenericNode("-", true);
        if(minusOperator){
            node->children.push_back(std::move(minusOperator));
        }
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
    auto bracketOpen = expectGenericNode("(", true);
    if(bracketOpen){
        node->children.push_back(std::move(bracketOpen));
        auto addExpr = expectAdditiveExpression();
        if(!addExpr){
            return nullptr;
        }
        auto bracketClose = expectGenericNode(")",false);
        if(!bracketClose){
            return nullptr;
        }
        node->children.push_back(std::move(addExpr));
        node->children.push_back(std::move(bracketClose));
        return node;
    }
    auto identifier = expectIdentifierNode(true);
    if(identifier) {
        node->children.push_back(std::move(identifier));
        return node;
    }

    auto literal = expectLiteralNode(true);
    if(literal){
        node->children.push_back(std::move(literal));
        return node;
    }
    printErrorMsg(Token(tokenizer.getManager()), "error: a primary expression is expected");
    return nullptr;
}
} // namespace parser
