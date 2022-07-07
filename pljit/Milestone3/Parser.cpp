#include "Parser.h"
#include <charconv>
namespace parser{
//Helper
void Parser::printErrorMsg(size_t location, std::string_view msg){
    tokens[location].sourceCodeReference.printContext(msg,tokens[location].getText().size());
}
//Parser--------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::parse() {
    if(tokens.empty()) return nullptr;
    else {
        return expectFunctionDefinition();
    }
}
//Refactor--------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::refactorList(size_t& currentPos,auto (Parser::*func)(size_t&), Node::Types nodeType, lexer::TokenTypes tokenType,const std::string& separatorType){
    auto identifier = (this->*func)(currentPos);
    if(identifier){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,sourceCodeManager);
        node->children.push_back(std::move(identifier));
        //check for more identifier

        while (currentPos+1< tokens.size()) {
            if(tokens[currentPos+1].getType() == tokenType){
                auto separator = expectGenericNode(separatorType, ++currentPos, false);
                return nullptr;
            } else if(tokens[currentPos+1].getType() == lexer::TokenTypes::Separator){
                auto separator = expectGenericNode(separatorType, ++currentPos, true);
                if (!separator) {
                    --currentPos;
                    return node;
                }
                identifier = (this->*func)(++currentPos);
                if(!identifier) {
                    return nullptr;
                }
                node->children.push_back(std::move(separator));
                node->children.push_back(std::move(identifier));
            } else {
                break;
            }
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::refactorAssignmentInit(size_t& currentPos,auto (Parser::*func)(size_t&), Node::Types nodeType, const std::string& assignmentType){
    auto identifier = expectIdentifierNode(currentPos);
    if(!identifier) {
        return nullptr;
    }
    auto equals = expectGenericNode(assignmentType, ++currentPos, false);
    if(!equals){
        return nullptr;
    }
    auto expr = (this->*func)(++currentPos);
    if (!expr) {
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType, sourceCodeManager);
    node->children.push_back(std::move(identifier));
    node->children.push_back(std::move(equals));
    node->children.push_back(std::move(expr));
    return node;
}
std::unique_ptr<NonTerminalNode> Parser::refactorDeclaration(size_t& currentPos,auto (Parser::*func)(size_t&), Node::Types nodeType, const std::string& keywordType,const std::string& endingKeyword){
    auto keyword = expectGenericNode(keywordType,currentPos, false);
    if(!keyword){
        return nullptr;
    }
    auto declList = (this->*func)(++currentPos);
    if(!declList){
        return nullptr;
    }
    auto ending = expectGenericNode(endingKeyword, ++currentPos, false);
    if(!ending){
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,sourceCodeManager);
    node->children.push_back(std::move(keyword));
    node->children.push_back(std::move(declList));
    node->children.push_back(std::move(ending));
    return node;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
//TODO: Fehlermeldung für optional ausschalten
std::unique_ptr<IdentifierNode> Parser::expectIdentifierNode(size_t& currentPos){
    if(tokens.empty()){
        return nullptr;
    } else if(currentPos>=tokens.size()){
        printErrorMsg(tokens.size()-1,"error: an identifier is expected!");
        return nullptr;
    }
 if(tokens[currentPos].getType() == lexer::TokenTypes::Identifier){
     return std::make_unique<IdentifierNode>(tokens[currentPos].getText(),sourceCodeManager);
 } else {
     printErrorMsg(currentPos,"error: an identifier is expected!");
     return nullptr;
 }
}

std::unique_ptr<LiteralNode> Parser::expectLiteralNode(size_t& currentPos){
    if(tokens.empty()){
        return nullptr;
    } else if(currentPos>=tokens.size()){
        printErrorMsg(tokens.size()-1,"error: a literal is expected!");
        return nullptr;
    }
    if(currentPos<tokens.size()&&tokens[currentPos].getType() == lexer::TokenTypes::Literal){
        //get number from string
        unsigned long value;
        std::from_chars(tokens[currentPos].getText().data(),tokens[currentPos].getText().data()+tokens[currentPos].getText().size(),value);
        return std::make_unique<LiteralNode>(value,sourceCodeManager);
    } else {
        printErrorMsg(currentPos,"error: a literal is expected!");
        return nullptr;
    }
}

std::unique_ptr<GenericNode> Parser::expectGenericNode(const std::string& c, size_t& currentPos, bool optional){
    if(tokens.empty()){
        return nullptr;
    } else if(currentPos>=tokens.size()){
        if(!optional){
            printErrorMsg(tokens.size()-1,"error: expected '"+c+"'");
        }
        return nullptr;
    }
    if(currentPos<tokens.size()){
        lexer::TokenTypes type = tokens[currentPos].getType();
        if((type == lexer::TokenTypes::Operator || type == lexer::TokenTypes::Separator || type == lexer::TokenTypes::Keyword) && tokens[currentPos].getText() == c){
                return std::make_unique<GenericNode>(tokens[currentPos].getText(),sourceCodeManager);
        }
    }
    if(!optional) {
        printErrorMsg(currentPos,"error: expected '"+c+"'");
    }
    return nullptr;
}
//----------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectFunctionDefinition(){
    size_t currentPosition = 0;
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::FunctionDefinition,sourceCodeManager);
    auto paramDecl = expectParameterDeclaration(currentPosition);
    if(paramDecl){
        node->children.push_back(std::move(paramDecl));
        ++currentPosition;
    }
    auto varDecl = expectVariableDeclaration(currentPosition);
    if(varDecl){
        node->children.push_back(std::move(varDecl));
        ++currentPosition;
    }
    auto constDecl = expectConstantDeclaration(currentPosition);
    if(constDecl){
        node->children.push_back(std::move(constDecl));
        ++currentPosition;
    }
    auto compStatement = expectCompoundStatement(currentPosition);
    if(!compStatement){
        return nullptr;
    }
    auto dot = expectGenericNode(".",++currentPosition, false);
    if(!dot){
        return nullptr;
    }
    node->children.push_back(std::move(compStatement));
    node->children.push_back(std::move(dot));
    return node;
}
std::unique_ptr<NonTerminalNode> Parser::expectParameterDeclaration(size_t& currentPos){
    return refactorDeclaration(currentPos,&Parser::expectDeclaratorList,Node::Types::ParameterDeclaration, "PARAM",";");
}
std::unique_ptr<NonTerminalNode> Parser::expectVariableDeclaration(size_t& currentPos){
    return refactorDeclaration(currentPos,&Parser::expectDeclaratorList,Node::Types::VariableDeclaration, "VAR",";");
}
std::unique_ptr<NonTerminalNode> Parser::expectConstantDeclaration(size_t& currentPos){
    return refactorDeclaration(currentPos,&Parser::expectInitDeclaratorList,Node::Types::ConstantDeclaration, "CONST",";");
}
//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectDeclaratorList(size_t& currentPos){
    return refactorList(currentPos, &Parser::expectIdentifierNode, Node::Types::DeclaratorList, lexer::TokenTypes::Identifier, ",");
}
std::unique_ptr<NonTerminalNode> Parser::expectInitDeclaratorList(size_t& currentPos){
    return refactorList(currentPos, &Parser::expectInitDeclarator, Node::Types::InitDeclaratorList, lexer::TokenTypes::Identifier, ",");
}
std::unique_ptr<NonTerminalNode> Parser::expectInitDeclarator(size_t& currentPos){
    return refactorAssignmentInit(currentPos,&Parser::expectLiteralNode,Node::Types::InitDeclarator,"=");
}
//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectCompoundStatement(size_t& currentPos){
    return refactorDeclaration(currentPos,&Parser::expectStatementList,Node::Types::CompoundStatement, "BEGIN","END");
}
std::unique_ptr<NonTerminalNode> Parser::expectStatementList(size_t& currentPos){
    return refactorList(currentPos, &Parser::expectStatement, Node::Types::StatementList, lexer::TokenTypes::Identifier, ";");
}
std::unique_ptr<NonTerminalNode> Parser::expectStatement(size_t& currentPos){
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::Statement,sourceCodeManager);
    auto returnStatement = expectGenericNode("RETURN", currentPos, false);
    if(returnStatement){
        //return additive expression
        auto addExpr = expectAdditiveExpression(++currentPos);
        if(addExpr){
            node->children.push_back(std::move(returnStatement));
            node->children.push_back(std::move(addExpr));
            return node;
        } else{
            return nullptr;
        }
    } else{
        auto assignment = expectAssignmentExpression(currentPos);
        if(assignment){
            node->children.push_back(std::move(assignment));
            return node;
        } else{
            return nullptr;
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectAssignmentExpression(size_t& currentPos){
    return refactorAssignmentInit(currentPos,&Parser::expectAdditiveExpression,Node::Types::AssignmentExpression,":=");
}
std::unique_ptr<NonTerminalNode> Parser::refactorExpression(size_t& currentPos,auto (Parser::*func1)(size_t&),auto (Parser::*func2)(size_t&), Node::Types nodeType, const std::string& operator1, const std::string& operator2){
    if(currentPos>= tokens.size()){
        return nullptr;
    }
    auto firstExpr = (this->*func1)(currentPos);
    if(!firstExpr){
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,sourceCodeManager);
    node->children.push_back(std::move(firstExpr));
    //check for optional
    auto firstOperator = expectGenericNode(operator1,++currentPos, true);
    if(firstOperator){
        node->children.push_back(std::move(firstOperator));
    } else {
        auto sndOperator = expectGenericNode(operator2,currentPos, true);
        if(sndOperator) {
            node->children.push_back(std::move(sndOperator));
        } else {    //no optional elements
            --currentPos;
            return node;
        }
    }
    auto sndExpr = (this->*func2)(++currentPos);
    if(!sndExpr){
        return nullptr;
    }
    node->children.push_back(std::move(sndExpr));
    return node;
}
std::unique_ptr<NonTerminalNode> Parser::expectAdditiveExpression(size_t& currentPos){
    auto result = refactorExpression(currentPos,&Parser::expectMultiplicativeExpression,&Parser::expectAdditiveExpression,Node::Types::AdditiveExpression,"+","-");
    if(!result){
        tokens[tokens.size()-1].sourceCodeReference.printContext("error: an additive expression is expected!",tokens[tokens.size()-1].getText().size());
    }
    return result;
}
std::unique_ptr<NonTerminalNode> Parser::expectMultiplicativeExpression(size_t& currentPos){
    auto result =  refactorExpression(currentPos,&Parser::expectUnaryExpression,&Parser::expectMultiplicativeExpression,Node::Types::MultiplicativeExpression,"*","/");
    if(!result){
        tokens[tokens.size()-1].sourceCodeReference.printContext("error: a multiplicative expression is expected!",tokens[tokens.size()-1].getText().size());
    }
    return result;
}
std::unique_ptr<NonTerminalNode> Parser::expectUnaryExpression(size_t& currentPos){
    //check for optional
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::UnaryExpression,sourceCodeManager);
    auto plusOperator = expectGenericNode("+",currentPos, true);
    if(plusOperator){
        node->children.push_back(std::move(plusOperator));
        ++currentPos;
    } else {
        auto minusOperator = expectGenericNode("-",currentPos, true);
        if(minusOperator){
            node->children.push_back(std::move(minusOperator));
            ++currentPos;
        }
    }
    auto primaryExpr = expectPrimaryExpression(currentPos);
    if(primaryExpr){
        node->children.push_back(std::move(primaryExpr));
        return node;
    } else {
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectPrimaryExpression(size_t& currentPos){
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::PrimaryExpression,sourceCodeManager);
    auto bracketOpen = expectGenericNode("(",currentPos, true);
    if(bracketOpen){
        node->children.push_back(std::move(bracketOpen));
        auto addExpr = expectAdditiveExpression(++currentPos);
        if(!addExpr){
            return nullptr;
        }
        auto bracketClose = expectGenericNode(")",++currentPos,false);
        if(!bracketClose){
            return nullptr;
        }
        node->children.push_back(std::move(addExpr));
        node->children.push_back(std::move(bracketClose));
        return node;
    }
    auto identifier = expectIdentifierNode(currentPos);
    if(identifier) {
        node->children.push_back(std::move(identifier));
        return node;
    }

    auto literal = expectLiteralNode(currentPos);
    if(literal){
        node->children.push_back(std::move(literal));
        return node;
    }
    //TODO:Korrekte Fehlermeldung
    tokens[currentPos].sourceCodeReference.printContext("error: a primary expression is expected", tokens[currentPos].getText().size());
    return nullptr;
}
} // namespace parser
