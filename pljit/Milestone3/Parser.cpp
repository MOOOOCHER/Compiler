#include "Parser.h"
#include <charconv>
namespace parser{
//Parser--------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::parse(std::vector<Token>& tokens) {
    if(tokens.empty()) return nullptr;
    else {
        return expectFunctionDefinition(tokens);
    }
}
//TODO: Für Fall dass currentPos größer ist als tokens.size() => wie soll die Fehlermeldung gehandhabt werden?
//--------------------------------------------------------------------------------------------------
std::unique_ptr<IdentifierNode> Parser::expectIdentifierNode(size_t& currentPos, std::vector<Token>& tokens){
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

std::unique_ptr<LiteralNode> Parser::expectLiteralNode(size_t& currentPos, std::vector<Token>& tokens){
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

std::unique_ptr<GenericNode> Parser::expectGenericNode(const std::string& c, size_t& currentPos, std::vector<Token>& tokens){
    if(tokens.empty()){
        return nullptr;
    } else if(currentPos>=tokens.size()){
        tokens[tokens.size()-1].sourceCodeReference.printContext("error: expected '"+c+"'",tokens[tokens.size()-1].getText().size());
        return nullptr;
    }
    if(currentPos<tokens.size()){
        lexer::TokenTypes type = tokens[currentPos].getType();
        if((type == lexer::TokenTypes::Operator || type == lexer::TokenTypes::Separator || type == lexer::TokenTypes::Keyword) && tokens[currentPos].getText() == c){
                return std::make_unique<GenericNode>(tokens[currentPos].getText(),sourceCodeManager);
        }
    }
    tokens[currentPos].sourceCodeReference.printContext("error: expected '"+c+"'",tokens[currentPos].getText().size());
    return nullptr;
}
//----------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectFunctionDefinition(std::vector<Token>& tokens){
    size_t currentPosition = 0;
    auto paramDecl = expectParameterDeclaration(currentPosition,tokens);
    auto varDecl = expectVariableDeclaration(currentPosition,tokens);
    auto constDecl = expectConstantDeclaration(currentPosition,tokens);

    auto compStatement = expectCompoundStatement(currentPosition,tokens);
    auto dot = expectGenericNode(".",++currentPosition,tokens);
    if(compStatement && dot){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::FunctionDefinition,sourceCodeManager);
        node->vec.push_back(std::move(paramDecl));
        node->vec.push_back(std::move(varDecl));
        node->vec.push_back(std::move(constDecl));
        node->vec.push_back(std::move(compStatement));
        node->vec.push_back(std::move(dot));
        return node;
    } else {
        return nullptr;
    }

}
std::unique_ptr<NonTerminalNode> Parser::expectParameterDeclaration(size_t& currentPos, std::vector<Token>& tokens){
    auto paramKeyword = expectGenericNode("PARAM",currentPos,tokens);
    auto declList = expectDeclaratorList(++currentPos,tokens);
    auto semikolon = expectGenericNode(";", ++currentPos, tokens);
    if(paramKeyword && declList && semikolon){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::ParameterDeclaration,sourceCodeManager);
        node->vec.push_back(std::move(paramKeyword));
        node->vec.push_back(std::move(declList));
        node->vec.push_back(std::move(semikolon));
        return node;
    } else {
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectVariableDeclaration(size_t& currentPos, std::vector<Token>& tokens){

    auto varKeyword = expectGenericNode("VAR",currentPos,tokens);
    auto declList = expectDeclaratorList(++currentPos,tokens);
    auto semikolon = expectGenericNode(";", ++currentPos, tokens);
    if(varKeyword && declList && semikolon){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::VariableDeclaration,sourceCodeManager);
        node->vec.push_back(std::move(varKeyword));
        node->vec.push_back(std::move(declList));
        node->vec.push_back(std::move(semikolon));
        return node;
    } else {
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectConstantDeclaration(size_t& currentPos, std::vector<Token>& tokens){
    auto varKeyword = expectGenericNode("VAR",currentPos,tokens);
    auto declList = expectDeclaratorList(++currentPos,tokens);
    auto semikolon = expectGenericNode(";", ++currentPos, tokens);
    if(varKeyword && declList && semikolon){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::ConstantDeclaration,sourceCodeManager);
        node->vec.push_back(std::move(varKeyword));
        node->vec.push_back(std::move(declList));
        node->vec.push_back(std::move(semikolon));
        return node;
    } else {
        return nullptr;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::refactorList(size_t& currentPos, std::vector<Token>& tokens,auto (Parser::*func)(size_t&, std::vector<Token>&), Node::Types nodeType, lexer::TokenTypes tokenType,std::string separatorType){
    auto identifier = (*func)(currentPos,tokens);
    if(identifier){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(nodeType,sourceCodeManager);
        node->vec.push_back(std::move(identifier));
        //check for more identifier
        while (currentPos+1< tokens.size()&& (tokens[currentPos+1].getType() == lexer::TokenTypes::Separator || tokens[currentPos+1].getType() == tokenType)) {
            auto separator = expectGenericNode(separatorType, ++currentPos, tokens);
            if (!separator) {
                return nullptr;
            }
            identifier = (*func)(++currentPos, tokens);
            if(!identifier) {
                return nullptr;
            }
            node->vec.push_back(std::move(separator));
            node->vec.push_back(std::move(identifier));
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::expectDeclaratorList(size_t& currentPos, std::vector<Token>& tokens){
    auto identifier = expectIdentifierNode(currentPos,tokens);
    if(identifier){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::DeclaratorList,sourceCodeManager);
        node->vec.push_back(std::move(identifier));
        //check for more identifier
        while (currentPos+1< tokens.size()&& (tokens[currentPos+1].getType() == lexer::TokenTypes::Separator || tokens[currentPos+1].getType() == lexer::TokenTypes::Identifier)) {
            auto comma = expectGenericNode(",", ++currentPos, tokens);
            if (!comma) {
                return nullptr;
            }
            identifier = expectIdentifierNode(++currentPos, tokens);
            if(!identifier) {
                return nullptr;
            }
            node->vec.push_back(std::move(comma));
            node->vec.push_back(std::move(identifier));
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<NonTerminalNode> Parser::expectInitDeclaratorList(size_t& currentPos, std::vector<Token>& tokens){
    auto identifier = expectInitDeclarator(currentPos,tokens);
    if(identifier){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::InitDeclaratorList,sourceCodeManager);
        node->vec.push_back(std::move(identifier));
        //check for more identifier
        while (currentPos+1< tokens.size() && (tokens[currentPos+1].getType() == lexer::TokenTypes::Separator || tokens[currentPos+1].getType() == lexer::TokenTypes::Identifier)) {
            auto comma = expectGenericNode(",", ++currentPos, tokens);
            if (!comma) {
                return nullptr;
            }
            identifier = expectInitDeclarator(++currentPos, tokens);
            if(!identifier) {
                return nullptr;
            }
            node->vec.push_back(std::move(comma));
            node->vec.push_back(std::move(identifier));
        }
        return node;
    } else {
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectInitDeclarator(size_t& currentPos, std::vector<Token>& tokens){
    auto identifier = expectIdentifierNode(currentPos,tokens);
    if(identifier) {
        auto equals = expectGenericNode("=", ++currentPos, tokens);
        auto literal = expectLiteralNode(++currentPos, tokens);
        if (equals && literal) {
            std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::InitDeclarator, sourceCodeManager);
            node->vec.push_back(std::move(identifier));
            node->vec.push_back(std::move(equals));
            node->vec.push_back(std::move(literal));
            return node;
        }
    }
    return nullptr;
}
//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<NonTerminalNode> Parser::expectCompoundStatement(size_t& currentPos, std::vector<Token>& tokens){
    auto begin = expectGenericNode("BEGIN",currentPos,tokens);
    auto statementList = expectStatementList(++currentPos,tokens);
    auto end = expectGenericNode("END", ++currentPos, tokens);
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
std::unique_ptr<NonTerminalNode> Parser::expectStatementList(size_t& currentPos, std::vector<Token>& tokens){
    auto statement = expectStatement(currentPos,tokens);
    if(statement){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::Statement,sourceCodeManager);
        node->vec.push_back(std::move(statement));
        //check for more identifier
        for(auto comma = expectGenericNode(",",++currentPos,tokens); comma != nullptr;){
            node->vec.push_back(std::move(comma));
            statement = expectStatement(++currentPos,tokens);
            if(statement){
                node->vec.push_back(std::move(statement));
            }else {
                return nullptr;
            }
        }
        return node;
    } else {
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectStatement(size_t& currentPos, std::vector<Token>& tokens){
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::Statement,sourceCodeManager);
    auto returnStatement = expectGenericNode("RETURN", currentPos, tokens);
    if(returnStatement){
        //return additive expression
        auto addExpr = expectAdditiveExpression(++currentPos,tokens);
        if(addExpr){
            node->vec.push_back(std::move(returnStatement));
            node->vec.push_back(std::move(addExpr));
            return node;
        } else{
            return nullptr;
        }
    } else{
        auto assignment = expectAdditiveExpression(currentPos, tokens);
        if(assignment){
            node->vec.push_back(std::move(assignment));
            return node;
        } else{
            return nullptr;
        }
    }
}

std::unique_ptr<NonTerminalNode> Parser::expectAssignmentExpression(size_t& currentPos, std::vector<Token>& tokens){
    auto identifier = expectIdentifierNode(currentPos,tokens);
    auto equalsAssign = expectGenericNode(":=",++currentPos,tokens);
    auto additiveExpr = expectAdditiveExpression( ++currentPos, tokens);
    if(identifier && equalsAssign && additiveExpr){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::AssignmentExpression,sourceCodeManager);
        node->vec.push_back(std::move(identifier));
        node->vec.push_back(std::move(equalsAssign));
        node->vec.push_back(std::move(additiveExpr));
        return node;
    } else {
        return nullptr;
    }
}

std::unique_ptr<NonTerminalNode> Parser::expectAdditiveExpression(size_t& currentPos, std::vector<Token>& tokens){
    auto mulExpr = expectMultiplicativeExpression(currentPos,tokens);
    if(mulExpr){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::AdditiveExpression,sourceCodeManager);
        node->vec.push_back(std::move(mulExpr));
        //check for optional
        auto plusOperator = expectGenericNode("+",++currentPos,tokens);
        auto minusOperator = expectGenericNode("-",++currentPos,tokens);
        if(plusOperator){
            node->vec.push_back(std::move(plusOperator));
        } else if (minusOperator){
            node->vec.push_back(std::move(minusOperator));
        } else {    //no further options
            return node;
        }
        auto addExpr = expectAdditiveExpression(++currentPos,tokens);
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
std::unique_ptr<NonTerminalNode> Parser::expectMultiplicativeExpression(size_t& currentPos, std::vector<Token>& tokens){
    auto unaryExpr = expectUnaryExpression(currentPos,tokens);
    if(unaryExpr){
        std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::MultiplicativeExpression,sourceCodeManager);
        node->vec.push_back(std::move(unaryExpr));
        //check for optional
        auto mulOperator = expectGenericNode("*",++currentPos,tokens);
        auto divOperator = expectGenericNode("/",++currentPos,tokens);
        if(mulOperator){
            node->vec.push_back(std::move(mulOperator));
        } else if (divOperator){
            node->vec.push_back(std::move(divOperator));
        } else {    //no further options
            return node;
        }
        auto mulExpr = expectMultiplicativeExpression(++currentPos,tokens);
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
std::unique_ptr<NonTerminalNode> Parser::expectUnaryExpression(size_t& currentPos, std::vector<Token>& tokens){
    //check for optional
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::UnaryExpression,sourceCodeManager);
    auto plusOperator = expectGenericNode("+",currentPos,tokens);
    auto minusOperator = expectGenericNode("-",currentPos,tokens);
    if(plusOperator){
        node->vec.push_back(std::move(plusOperator));
    } else if (minusOperator){
        node->vec.push_back(std::move(minusOperator));
    }
    auto primaryExpr = expectPrimaryExpression(currentPos,tokens);
    if(primaryExpr){
        node->vec.push_back(std::move(primaryExpr));
        return node;
    } else {
        return nullptr;
    }
}
std::unique_ptr<NonTerminalNode> Parser::expectPrimaryExpression(size_t& currentPos, std::vector<Token>& tokens){
    std::unique_ptr<NonTerminalNode> node = std::make_unique<NonTerminalNode>(Node::Types::UnaryExpression,sourceCodeManager);
    auto bracketOpen = expectGenericNode("(",currentPos,tokens);
    if(bracketOpen){
        auto addExpr = expectAdditiveExpression(currentPos,tokens);
        auto bracketClose = expectGenericNode(")",currentPos,tokens);
    } else{
        auto identifier = expectIdentifierNode(currentPos,tokens);
        auto literal = expectLiteralNode(currentPos,tokens);
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
