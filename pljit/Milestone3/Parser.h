#ifndef PLJIT_PARSER_H
#define PLJIT_PARSER_H
#include "Node.h"
#include "../Milestone2/Token.h"
namespace parser{
using Token = lexer::Token;
using Tokenizer = lexer::Tokenizer;
using Node = parser::Node;
using TokenTypes = lexer::TokenTypes;

class Parser{
    Tokenizer tokenizer;
    Token backtrackToken = Token(tokenizer.getManager());   //default token

    //------------------------------------------------------------------------------------------
    /*
     * this function prints error msgs for the parser
     */
    static void printErrorMsg(Token& token, std::string_view msg);
    /*
     * this function prints default error msgs for the parser if there is no next token => Reference to the last char of the code
     */
    void printDefaultErrorMsg(std::string_view msg);
    /*
     * this helper function resets the backtrack-token
     */
    void resetBacktrackToken(){
        backtrackToken = Token(tokenizer.getManager());
    }

    //---------------------------------------------------------------------------------------------------------
    /*
     * The following functions parse non-terminal symbols.
     * If there is a parsing error(e.g. a component is not found), a nullptr is returned
     */
    std::unique_ptr<ParameterDeclarationNode> expectParameterDeclaration();
    std::unique_ptr<VariableDeclarationNode> expectVariableDeclaration();
    std::unique_ptr<ConstantDeclarationNode> expectConstantDeclaration();

    std::unique_ptr<DeclaratorListNode> expectDeclaratorList();
    std::unique_ptr<InitDeclaratorListNode> expectInitDeclaratorList();
    std::unique_ptr<InitDeclaratorNode> expectInitDeclarator();

    std::unique_ptr<CompoundStatementNode> expectCompoundStatement();
    std::unique_ptr<StatementListNode> expectStatementList();
    std::unique_ptr<StatementNode> expectStatement();
    std::unique_ptr<AssignmentExpressionNode> expectAssignmentExpression();

    std::unique_ptr<AdditiveExpressionNode> expectAdditiveExpression();
    std::unique_ptr<MultiplicativeExpressionNode> expectMultiplicativeExpression();
    std::unique_ptr<UnaryExpressionNode> expectUnaryExpression();
    std::unique_ptr<PrimaryExpressionNode> expectPrimaryExpression();
    //---------------------------------------------------------------------------------------------------
    /*
     * The following three functions parse terminal symbols.
     * If there is a parsing error(e.g. a component is not found), a nullptr is returned
     */
    std::unique_ptr<IdentifierNode> expectIdentifierNode();

    std::unique_ptr<LiteralNode> expectLiteralNode();

    std::unique_ptr<TerminalNode> expectGenericNode(TokenTypes type);
    //Refactor--------------------------------------------------------------------------------------------------------------------
    /*
     * this function refactors grammar productions of this form, identifier{"," identifier} for declaratorList and InitDeclaratorList.
     * it evaluates the children and returns whether parsing for them was successful.
     *
     * Arguments:
     * func*: the function pointer for the expectFunction, that gets the elements of the list, mentioned above (e.g. identifier for declarator-list)
     * childVec: reference to the childVector
      */
    bool refactorDeclList(auto (Parser::*func)(), std::vector<std::unique_ptr<Node>>& childVec);
    /*
     * this function refactors grammar for parameter/variable/constant declarations and for compound statement
     * it evaluates the children and returns whether parsing for them was successful.
     *
     * Arguments:
     * func*: the function pointer for the expectFunction mentioned above (e.g. identifier for declarator-list)
     * childVec: reference to the childVector
     * keywordType: type of the keyword that starts this expression,e.g. "PARAM", "BEGIN"
     * endingKeyword:type of the keyword that ends this expression,e.g. ";", "END"
      */
    bool refactorDeclaration(auto (Parser::*func)(), Node::Types startingKeyword, TokenTypes endingKeyword, std::vector<std::unique_ptr<Node>>& childVec);
    /*
     * this function refactors grammar productions of this form, (expression) (assignmentOperator) (expression)
     * PARAM:
     * *func1, *func2: the function pointer for the expectFunction mentioned above (e.g. identifier for declarator-list)
     * operator1, operator2: token type of the operator, e.g. "*","+"
     * childVec:  reference to the childVector
      */
    bool refactorExpression(auto (Parser::*func1)(),auto (Parser::*func2)(), TokenTypes operator1, TokenTypes operator2, std::vector<std::unique_ptr<Node>>& childVec);

    public:
    explicit Parser(Tokenizer tokenizer);
    std::unique_ptr<FunctionDefinitionNode> expectFunctionDefinition();
};
} // namespace parser
#endif //PLJIT_PARSER_H
