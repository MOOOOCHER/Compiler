#include "Parser.h"
#include <charconv>
namespace parser{
//Helper
void Parser::printDefaultErrorMsg(std::string_view msg){
    Token token = Token(tokenizer.getManager());
    token.getSourceCodeReference().printContext(msg);
}
void Parser::printErrorMsg(Token& token, std::string_view msg){
        token.getSourceCodeReference().printContext(msg);
}
static std::string returnCorrectGenericForErrorMsg(TokenTypes type){
    switch(type){   //can be easily extended
        case lexer::TokenTypes::Dot: return "'.'";
        case TokenTypes::Semicolon: return "';'";
        case TokenTypes::InitEquals: return "'='";
        case TokenTypes::AssignEquals: return "':='";
        case TokenTypes::END: return "'END'";
        default: return "";
    }
}

//Parser----------------------------------------------------------------------------------------------------
Parser::Parser(Tokenizer tokenizer): tokenizer(tokenizer){}
//Refactor--------------------------------------------------------------------------------------------------
bool Parser::refactorDeclList(auto (Parser::*func)(), std::vector<std::unique_ptr<Node>>& childVec){
    auto element = (this->*func)();
    if(element){
        childVec.push_back(std::move(element));
        //check for more
        while (tokenizer.hasNext()) {
            auto separator = tokenizer.next();
            if(separator.getType() == TokenTypes::Invalid ){
                return false;
            } else if(separator.getType() == TokenTypes::Identifier ||separator.getType()==TokenTypes::Dot){
                printErrorMsg(separator,"error: expected ',' in between!");
                return false;
            } else if(separator.getType() != lexer::TokenTypes::Comma){
                backtrackToken = separator;
                return true;
            }
            element = (this->*func)();
            if(!element) {
                return false;
            }
            childVec.push_back(std::make_unique<CommaNode>(separator.getSourceCodeReference()));
            childVec.push_back(std::move(element));
        }
        return true;
    }
    return false;
}
bool Parser::refactorDeclaration(auto (Parser::*func)(), Node::Types startingKeyword, TokenTypes endingKeyword, std::vector<std::unique_ptr<Node>>& childVec){
    //we only get in here if the check was successful
    if(startingKeyword == Node::PARAM){
        childVec.push_back(std::make_unique<ParameterKeywordNode>(backtrackToken.getSourceCodeReference()));
    }else if(startingKeyword == Node::VAR){
        childVec.push_back(std::make_unique<VariableKeywordNode>(backtrackToken.getSourceCodeReference()));
    }else if(startingKeyword == Node::CONST){
        childVec.push_back(std::make_unique<ConstantKeywordNode>(backtrackToken.getSourceCodeReference()));
    }else {
        childVec.push_back(std::make_unique<BeginNode>(backtrackToken.getSourceCodeReference()));
    }

    resetBacktrackToken();
    auto declList = (this->*func)();
    if(!declList){
        return false;
    }
    //we have already evaluated this token, but it was optional, so we may need to evaluate it again
    if(backtrackToken.getType() == endingKeyword){
        childVec.push_back(std::move(declList));
        if(endingKeyword == lexer::TokenTypes::END){
            childVec.push_back(std::make_unique<EndNode>(backtrackToken.getSourceCodeReference()));
        }else {
            //we would always have a semicolon in this case
            childVec.push_back(std::make_unique<SemicolonNode>(backtrackToken.getSourceCodeReference()));
        }
        resetBacktrackToken();
        return true;
    }
    printErrorMsg(backtrackToken,"error: expected "+returnCorrectGenericForErrorMsg(endingKeyword));
    return false;
}
bool Parser::refactorExpression(auto (Parser::*func1)(),auto (Parser::*func2)(), const TokenTypes operator1, const TokenTypes operator2, std::vector<std::unique_ptr<Node>>& childVec){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: an expression is expected!");
        return false;
    }
    auto firstExpr = (this->*func1)();
    if(!firstExpr){
        return false;
    }
    childVec.push_back(std::move(firstExpr));
    //check for optional
    Token token = backtrackToken;
    if(backtrackToken.getType() == TokenTypes::Invalid){
        if(!tokenizer.hasNext()){
            return true;
        }
        token = tokenizer.next();
        if(token.getType() == TokenTypes::Invalid ){
            return false;
        }
    }
    if (token.getType() != operator1 && token.getType() != operator2){
        //no optional component at this level
        backtrackToken = token;
        return true;
    }
    auto sndExpr = (this->*func2)();
    if(!sndExpr){
        return false;
    }
    auto operatorType = token.getType();
    if(operatorType==TokenTypes::PlusOperator){
        childVec.push_back(std::make_unique<PlusOperatorNode>(token.getSourceCodeReference()));
    } else if (operatorType == TokenTypes::MinusOperator){
        childVec.push_back(std::make_unique<MinusOperatorNode>(token.getSourceCodeReference()));
    } else if (operatorType == TokenTypes::MulOperator){
        childVec.push_back(std::make_unique<MultiplicationOperatorNode>(token.getSourceCodeReference()));
    } else if (operatorType == TokenTypes::DivOperator){
        childVec.push_back(std::make_unique<DivisionOperatorNode>(token.getSourceCodeReference()));
    }
    childVec.push_back(std::move(sndExpr));
    return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------

std::unique_ptr<IdentifierNode> Parser::expectIdentifierNode(){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg( "error: an identifier is expected afterwards!");
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == TokenTypes::Invalid ){
        return nullptr;
    } else if(token.getType() == lexer::TokenTypes::Identifier){
        return std::make_unique<IdentifierNode>(token.getSourceCodeReference(), token.getSourceCodeReference().getText());
    } else {
        printErrorMsg(token, "error: an identifier is expected!");
        return nullptr;
 }
}

std::unique_ptr<LiteralNode> Parser::expectLiteralNode(){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: a literal is expected afterwards!");
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == TokenTypes::Invalid ){
        return nullptr;
    } else if(token.getType() == lexer::TokenTypes::Literal){
        //get number from string
        unsigned value;
        std::string_view name = token.getSourceCodeReference().getText();
        std::from_chars(name.data(),name.data()+name.size(),value);
        return std::make_unique<LiteralNode>(token.getSourceCodeReference(),value);
    } else {
        printErrorMsg(token, "error: a literal is expected!");
        return nullptr;
    }
}

std::unique_ptr<TerminalNode> Parser::expectGenericNode(TokenTypes type){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: expected"+returnCorrectGenericForErrorMsg(type) + "afterwards");
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == TokenTypes::Invalid ){
        return nullptr;
    } else if(type == token.getType()){
        switch(type){   //Only used for these three types, but can be easily extended
            case TokenTypes::Dot: return std::make_unique<DotNode>(token.getSourceCodeReference());
            case TokenTypes::InitEquals: return std::make_unique<InitEqualsNode>(token.getSourceCodeReference());
            case TokenTypes::AssignEquals: return std::make_unique<AssignEqualsNode>(token.getSourceCodeReference());
            default: break;
        }
    }
    printErrorMsg(token,"error: expected"+returnCorrectGenericForErrorMsg(type));
    return nullptr;
}
//----------------------------------------------------------------------------------------------------
std::unique_ptr<FunctionDefinitionNode> Parser::expectFunctionDefinition(){
    if(!tokenizer.hasNext()){
        return nullptr;
    }
    std::vector<std::unique_ptr<Node>> childVec;
    auto token = tokenizer.next();

    auto addParseNode = [this,&childVec, &token](auto func){
        backtrackToken = token;
        auto child = (this->*func)();
        if(!child){
            return false;
        } else if( !tokenizer.hasNext()){
            printDefaultErrorMsg("error: compound statement is expected afterwards!");
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

    if(token.getType() == TokenTypes::Invalid){
        return nullptr;
    } else if (token.getType() == TokenTypes::PARAM){
        if(!addParseNode(&Parser::expectParameterDeclaration))return nullptr;
    }
    if(token.getType() == TokenTypes::VAR){
        if(!addParseNode(&Parser::expectVariableDeclaration))return nullptr;
    }
    if(token.getType() == TokenTypes::CONST){
        if(!addParseNode(&Parser::expectConstantDeclaration))return nullptr;
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
    return std::make_unique<FunctionDefinitionNode>(std::move(childVec));
}
std::unique_ptr<ParameterDeclarationNode> Parser::expectParameterDeclaration(){
    std::vector<std::unique_ptr<Node>> childVec;
    if(!refactorDeclaration(&Parser::expectDeclaratorList, Node::Types::PARAM, TokenTypes::Semicolon, childVec)){
            return nullptr;
    }
    return std::make_unique<ParameterDeclarationNode>(std::move(childVec));
}
std::unique_ptr<VariableDeclarationNode> Parser::expectVariableDeclaration(){
    std::vector<std::unique_ptr<Node>> childVec;
    if(!refactorDeclaration(&Parser::expectDeclaratorList, Node::Types::VAR,TokenTypes::Semicolon, childVec)){
        return nullptr;
    }
    return std::make_unique<VariableDeclarationNode>(std::move(childVec));
}
std::unique_ptr<ConstantDeclarationNode> Parser::expectConstantDeclaration(){
    std::vector<std::unique_ptr<Node>> childVec;
    if(!refactorDeclaration(&Parser::expectInitDeclaratorList, Node::Types::CONST,TokenTypes::Semicolon, childVec)){
        return nullptr;
    }
    return std::make_unique<ConstantDeclarationNode>(std::move(childVec));
}

//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<DeclaratorListNode> Parser::expectDeclaratorList(){
    std::vector<std::unique_ptr<Node>> childVec;
    if(!refactorDeclList(&Parser::expectIdentifierNode, childVec)){
        return nullptr;
    }
    return std::make_unique<DeclaratorListNode>(std::move(childVec));
}
std::unique_ptr<InitDeclaratorListNode> Parser::expectInitDeclaratorList(){
    std::vector<std::unique_ptr<Node>> childVec;
    if(!refactorDeclList(&Parser::expectInitDeclarator,childVec)){
        return nullptr;
    }
    return std::make_unique<InitDeclaratorListNode>(std::move(childVec));
}
std::unique_ptr<InitDeclaratorNode> Parser::expectInitDeclarator(){
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
    return std::make_unique<InitDeclaratorNode>(std::move(childVec));
}
//------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<CompoundStatementNode> Parser::expectCompoundStatement(){
    std::vector<std::unique_ptr<Node>> childVec;
    if(!refactorDeclaration(&Parser::expectStatementList, Node::Types::BEGIN,TokenTypes::END, childVec)){
        return nullptr;
    }
    return std::make_unique<CompoundStatementNode>(std::move(childVec));
}
std::unique_ptr<StatementListNode> Parser::expectStatementList(){
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
                return std::make_unique<StatementListNode>(std::move(childVec));
            } else if (separator.getType() == TokenTypes::CloseBracket){
                printErrorMsg(separator,"error: no corresponding '('");
                return nullptr;
            }
            else if(separator.getType()!=TokenTypes::Semicolon){
                printErrorMsg(separator,"error: expected ';'");
                return nullptr;
            }
            element = expectStatement();
            if(!element) {
                return nullptr;
            }
            childVec.push_back(std::make_unique<SemicolonNode>(separator.getSourceCodeReference()));
            childVec.push_back(std::move(element));
        }
        return std::make_unique<StatementListNode>(std::move(childVec));
    }
    return nullptr;
}
std::unique_ptr<StatementNode> Parser::expectStatement(){
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: expected statement!");
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == TokenTypes::Invalid ){
        //Lexing was not successful
        return nullptr;
    } else if(token.getType() == TokenTypes::RETURN){
        //return additive expression
        auto addExpr = expectAdditiveExpression();
        if(!addExpr){
            return nullptr;
        }
        std::vector<std::unique_ptr<Node>> childVec;
        childVec.push_back(std::make_unique<ReturnNode>(token.getSourceCodeReference()));
        childVec.push_back(std::move(addExpr));
        return std::make_unique<StatementNode>(std::move(childVec));
    } else {
        backtrackToken = token;
        auto assignment = expectAssignmentExpression();
        if(!assignment){
            return nullptr;
        }
        std::vector<std::unique_ptr<Node>> childVec;
        childVec.push_back(std::move(assignment));
        return std::make_unique<StatementNode>(std::move(childVec));
    }
}
//-------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<AssignmentExpressionNode> Parser::expectAssignmentExpression(){
    if(backtrackToken.getType() != TokenTypes::Identifier) {
        printErrorMsg(backtrackToken,"error: expected identifier or 'RETURN' statement!");
        return nullptr;
    }
    std::vector<std::unique_ptr<Node>> childVec;
    childVec.push_back(std::make_unique<IdentifierNode>(backtrackToken.getSourceCodeReference(),backtrackToken.getSourceCodeReference().getText()));
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
    return std::make_unique<AssignmentExpressionNode>(std::move(childVec));
}

std::unique_ptr<AdditiveExpressionNode> Parser::expectAdditiveExpression(){
    std::vector<std::unique_ptr<Node>> childVec;
    if(!refactorExpression(&Parser::expectMultiplicativeExpression,&Parser::expectAdditiveExpression,TokenTypes::PlusOperator,TokenTypes::MinusOperator, childVec)){
        return nullptr;
    }
    return std::make_unique<AdditiveExpressionNode>(std::move(childVec));
}
std::unique_ptr<MultiplicativeExpressionNode> Parser::expectMultiplicativeExpression(){
    std::vector<std::unique_ptr<Node>> childVec;
    if(!refactorExpression(&Parser::expectUnaryExpression,&Parser::expectMultiplicativeExpression, TokenTypes::MulOperator,TokenTypes::DivOperator, childVec)){
        return nullptr;
    }
    return std::make_unique<MultiplicativeExpressionNode>(std::move(childVec));
}

std::unique_ptr<UnaryExpressionNode> Parser::expectUnaryExpression(){
    //check for optional
    if(!tokenizer.hasNext()){
        printDefaultErrorMsg("error: an expression is expected!");
        return nullptr;
    }
    auto token = tokenizer.next();
    if(token.getType() == TokenTypes::Invalid ){
        return nullptr;
    }
    std::vector<std::unique_ptr<Node>> childVec;

    if(token.getType() == lexer::TokenTypes::PlusOperator){
        childVec.push_back(std::make_unique<PlusOperatorNode>(token.getSourceCodeReference()));
        resetBacktrackToken();
    } else if(token.getType() == lexer::TokenTypes::MinusOperator){
        childVec.push_back(std::make_unique<MinusOperatorNode>(token.getSourceCodeReference()));
        resetBacktrackToken();
    }
    else {
        backtrackToken = token;
    }

    auto primaryExpr = expectPrimaryExpression();
    if(!primaryExpr){
        return nullptr;
    }
    childVec.push_back(std::move(primaryExpr));
    return std::make_unique<UnaryExpressionNode>(std::move(childVec));
}
std::unique_ptr<PrimaryExpressionNode> Parser::expectPrimaryExpression(){
    Token token = backtrackToken;
    if(backtrackToken.getType() == TokenTypes::Invalid){
        if(!tokenizer.hasNext()){
            printDefaultErrorMsg("error: an expression is expected!");
            return nullptr;
        }
        token = tokenizer.next();
        if(token.getType() == TokenTypes::Invalid ){
            return nullptr;
        }
    }

    std::vector<std::unique_ptr<Node>> childVec;
    if(token.getType() == TokenTypes::Identifier){
        childVec.push_back(std::make_unique<IdentifierNode>(token.getSourceCodeReference(), token.getSourceCodeReference().getText()));
        resetBacktrackToken();
        return std::make_unique<PrimaryExpressionNode>(std::move(childVec));
    } else if(token.getType() == TokenTypes::Literal){
        unsigned value;
        std::string_view name = token.getSourceCodeReference().getText();
        std::from_chars(name.data(),name.data()+name.size(),value);
        childVec.push_back(std::make_unique<LiteralNode>(token.getSourceCodeReference(),value));
        resetBacktrackToken();
        return std::make_unique<PrimaryExpressionNode>(std::move(childVec));
    } else if (token.getType() == TokenTypes::OpenBracket){
        resetBacktrackToken();

        auto addExpr = expectAdditiveExpression();
        if(!addExpr){
            return nullptr;
        }
        Token token2 = backtrackToken;
        if(backtrackToken.getType() == TokenTypes::Invalid){
            if(!tokenizer.hasNext()){
                return std::make_unique<PrimaryExpressionNode>(std::move(childVec));
            }
            token2 = tokenizer.next();
            if(token.getType() == TokenTypes::Invalid ){
                return nullptr;
            }
        }
        if(token2.getType() != lexer::TokenTypes::CloseBracket){
            printErrorMsg(token2, "error: expected ')'");
            return nullptr;
        }
        resetBacktrackToken();
        childVec.push_back(std::make_unique<OpenBracketNode>(token.getSourceCodeReference()));
        childVec.push_back(std::move(addExpr));
        childVec.push_back(std::make_unique<CloseBracketNode>(token2.getSourceCodeReference()));
        return std::make_unique<PrimaryExpressionNode>(std::move(childVec));
    }else{
        printErrorMsg(token, "error: invalid expression!");
        return nullptr;
    }
}
} // namespace parser
