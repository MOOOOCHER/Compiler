#include "Parser.h"
#include <charconv>
namespace parser{
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
        node->vec.push_back(std::move(identifier));
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
                node->vec.push_back(std::move(separator));
                node->vec.push_back(std::move(identifier));
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
    node->vec.push_back(std::move(identifier));
    node->vec.push_back(std::move(equals));
    node->vec.push_back(std::move(expr));
    return node;
}
std::unique_ptr<NonTerminalNode> Parser::refactorDeclaration(size_t& currentPos,auto (Parser::*func)(size_t&), Node::Types nodeType, const std::string& keywordType){
    auto paramKeyword = expectGenericNode(keywordType,currentPos, false);
    if(!paramKeyword){
        return nullptr;
    }
    auto declList = (this->*func)(++currentPos);
    if(!declList){
        return nullptr;
    }
    auto semikolon = expectGenericNode(";", ++currentPos, false);
    if(!semikolon){
        return nullptr;
    }
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,sourceCodeManager);
    node->vec.push_back(std::move(paramKeyword));
    node->vec.push_back(std::move(declList));
    node->vec.push_back(std::move(semikolon));
    return node;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IdentifierNode> Parser::expectIdentifierNode(size_t& currentPos){
    if(tokens.empty()){
        return nullptr;
    } else if(currentPos>=tokens.size()){
        tokens[tokens.size()-1].sourceCodeReference.printContext("error: an identifier is expected!",tokens[tokens.size()-1].getText().size());
        return nullptr;
    }
 if(tokens[currentPos].getType() == lexer::TokenTypes::Identifier){
     return std::make_unique<IdentifierNode>(tokens[currentPos].getText(),sourceCodeManager);
 } else {
     tokens[currentPos].sourceCodeReference.printContext("error: an identifier is expected!",tokens[currentPos].getText().size());
     return nullptr;
 }
}

std::unique_ptr<LiteralNode> Parser::expectLiteralNode(size_t& currentPos){
    if(tokens.empty()){
        return nullptr;
    } else if(currentPos>=tokens.size()){
        tokens[tokens.size()-1].sourceCodeReference.printContext("error: a literal is expected!",tokens[tokens.size()-1].getText().size());
        return nullptr;
    }
    if(currentPos<tokens.size()&&tokens[currentPos].getType() == lexer::TokenTypes::Literal){
        //get number from string
        unsigned long value;
        std::from_chars(tokens[currentPos].getText().data(),tokens[currentPos].getText().data()+tokens[currentPos].getText().size(),value);
        return std::make_unique<LiteralNode>(value,sourceCodeManager);
    } else {
        tokens[currentPos].sourceCodeReference.printContext("error: a literal is expected!",tokens[currentPos].getText().size());
        return nullptr;
    }
}

std::unique_ptr<GenericNode> Parser::expectGenericNode(const std::string& c, size_t& currentPos, bool optional){
    if(tokens.empty()){
        return nullptr;
    } else if(currentPos>=tokens.size()){
        if(!optional){
            tokens[tokens.size()-1].sourceCodeReference.printContext("error: expected '"+c+"'",tokens[tokens.size()-1].getText().size());
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
        tokens[currentPos].sourceCodeReference.printContext("error: expected '" + c + "'", tokens[currentPos].getText().size());
    }
    return nullptr;
}
//----------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectFunctionDefinition(){
    size_t currentPosition = 0;
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::FunctionDefinition,sourceCodeManager);
    auto paramDecl = expectParameterDeclaration(currentPosition);
    if(paramDecl){
        node->vec.push_back(std::move(paramDecl));
        ++currentPosition;
    }
    auto varDecl = expectVariableDeclaration(currentPosition);
    if(varDecl){
        node->vec.push_back(std::move(varDecl));
        ++currentPosition;
    }
    auto constDecl = expectConstantDeclaration(currentPosition);
    if(constDecl){
        node->vec.push_back(std::move(constDecl));
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
    node->vec.push_back(std::move(compStatement));
    node->vec.push_back(std::move(dot));
    return node;
}
std::unique_ptr<NonTerminalNode> Parser::expectParameterDeclaration(size_t& currentPos){
    return refactorDeclaration(currentPos,&Parser::expectDeclaratorList,Node::Types::ParameterDeclaration, "PARAM");
}
std::unique_ptr<NonTerminalNode> Parser::expectVariableDeclaration(size_t& currentPos){
    return refactorDeclaration(currentPos,&Parser::expectDeclaratorList,Node::Types::VariableDeclaration, "VAR");
}
std::unique_ptr<NonTerminalNode> Parser::expectConstantDeclaration(size_t& currentPos){
    return refactorDeclaration(currentPos,&Parser::expectInitDeclaratorList,Node::Types::ConstantDeclaration, "CONST");
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
    auto begin = expectGenericNode("BEGIN",currentPos, false);
    auto statementList = expectStatementList(++currentPos);
    auto end = expectGenericNode("END", ++currentPos, false);
    if(begin && statementList && end){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::CompoundStatement,sourceCodeManager);
        node->vec.push_back(std::move(begin));
        node->vec.push_back(std::move(statementList));
        node->vec.push_back(std::move(end));
        return node;
    } else {
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectStatementList(size_t& currentPos){
    return refactorList(currentPos, &Parser::expectStatement, Node::Types::Statement, lexer::TokenTypes::Identifier, ";");
}
std::unique_ptr<NonTerminalNode> Parser::expectStatement(size_t& currentPos){
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::Statement,sourceCodeManager);
    auto returnStatement = expectGenericNode("RETURN", currentPos, false);
    if(returnStatement){
        //return additive expression
        auto addExpr = expectAdditiveExpression(++currentPos);
        if(addExpr){
            node->vec.push_back(std::move(returnStatement));
            node->vec.push_back(std::move(addExpr));
            return node;
        } else{
            return nullptr;
        }
    } else{
        auto assignment = expectAssignmentExpression(currentPos);
        if(assignment){
            node->vec.push_back(std::move(assignment));
            return node;
        } else{
            return nullptr;
        }
    }
}

std::unique_ptr<NonTerminalNode> Parser::expectAssignmentExpression(size_t& currentPos){
    return refactorAssignmentInit(currentPos,&Parser::expectAdditiveExpression,Node::Types::AssignmentExpression,":=");
}

std::unique_ptr<NonTerminalNode> Parser::expectAdditiveExpression(size_t& currentPos){
    auto mulExpr = expectMultiplicativeExpression(currentPos);
    if(mulExpr){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::AdditiveExpression,sourceCodeManager);
        node->vec.push_back(std::move(mulExpr));
        //check for optional
        auto plusOperator = expectGenericNode("+",++currentPos, true);
        auto minusOperator = expectGenericNode("-",++currentPos, true);
        if(plusOperator){
            node->vec.push_back(std::move(plusOperator));
        } else if (minusOperator){
            node->vec.push_back(std::move(minusOperator));
        } else {    //no further options
            return node;
        }
        auto addExpr = expectAdditiveExpression(++currentPos);
        if(addExpr){
            node->vec.push_back(std::move(addExpr));
            return node;
        }else{
            return nullptr;
        }
    }else{
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectMultiplicativeExpression(size_t& currentPos){
    auto unaryExpr = expectUnaryExpression(currentPos);
    if(unaryExpr){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::MultiplicativeExpression,sourceCodeManager);
        node->vec.push_back(std::move(unaryExpr));
        //check for optional
        auto mulOperator = expectGenericNode("*",++currentPos, true);
        auto divOperator = expectGenericNode("/",++currentPos, true);
        if(mulOperator){
            node->vec.push_back(std::move(mulOperator));
        } else if (divOperator){
            node->vec.push_back(std::move(divOperator));
        } else {    //no further options
            return node;
        }
        auto mulExpr = expectMultiplicativeExpression(++currentPos);
        if(mulExpr){
            node->vec.push_back(std::move(mulExpr));
            return node;
        }else{
            return nullptr;
        }
    }else{
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectUnaryExpression(size_t& currentPos){
    //check for optional
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::UnaryExpression,sourceCodeManager);
    auto plusOperator = expectGenericNode("+",currentPos, true);
    auto minusOperator = expectGenericNode("-",currentPos, true);
    if(plusOperator){
        node->vec.push_back(std::move(plusOperator));
    } else if (minusOperator){
        node->vec.push_back(std::move(minusOperator));
    }
    auto primaryExpr = expectPrimaryExpression(currentPos);
    if(primaryExpr){
        node->vec.push_back(std::move(primaryExpr));
        return node;
    } else {
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectPrimaryExpression(size_t& currentPos){
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::UnaryExpression,sourceCodeManager);
    auto bracketOpen = expectGenericNode("(",currentPos, true);
    if(bracketOpen){
        auto addExpr = expectAdditiveExpression(currentPos);
        if(!addExpr){
            return nullptr;
        }
        auto bracketClose = expectGenericNode(")",currentPos,false);
        if(!bracketClose){
            return nullptr;
        }
        node->vec.push_back(std::move(addExpr));
        node->vec.push_back(std::move(bracketClose));
        return node;
    } else{
        auto identifier = expectIdentifierNode(currentPos);
        auto literal = expectLiteralNode(currentPos);
        if(identifier){
            node->vec.push_back(std::move(identifier));
        } else if (literal){
            node->vec.push_back(std::move(literal));
        }
        else {
            return nullptr;
        }
    }
    return node;
}
} // namespace parser
