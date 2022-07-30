#ifndef PLJIT_PARSER_H
#define PLJIT_PARSER_H
#include <optional>
#include "Node.h"
#include "../Milestone2/Token.h"
namespace parser{
using Token = lexer::Token;
using Tokenizer = lexer::Tokenizer;
using Node = parser::Node;
using TokenTypes =lexer::TokenTypes;
class Parser{
    Tokenizer tokenizer;
    Token backtrackToken = Token(tokenizer.getManager());
    public:
    explicit Parser(Tokenizer tokenizer): tokenizer(tokenizer){};

    //---------------------------------------------------------------------------------------------------------
    /*
     * The following functions parse non-terminal symbols.
     * If there is a parsing error(e.g. a component is not found), a nullptr is returned
     */
    std::unique_ptr<NonTerminalNode> expectFunctionDefinition();
    private:
    std::unique_ptr<NonTerminalNode> expectParameterDeclaration();
    std::unique_ptr<NonTerminalNode> expectVariableDeclaration();
    std::unique_ptr<NonTerminalNode> expectConstantDeclaration();

    std::unique_ptr<NonTerminalNode> expectDeclaratorList();
    std::unique_ptr<NonTerminalNode> expectInitDeclaratorList();
    std::unique_ptr<NonTerminalNode> expectInitDeclarator();

    std::unique_ptr<NonTerminalNode> expectCompoundStatement();
    std::unique_ptr<NonTerminalNode> expectStatementList();
    std::unique_ptr<NonTerminalNode> expectStatement();
    std::unique_ptr<NonTerminalNode> expectAssignmentExpression();

    std::unique_ptr<NonTerminalNode> expectAdditiveExpression();
    std::unique_ptr<NonTerminalNode> expectMultiplicativeExpression();
    std::unique_ptr<NonTerminalNode> expectUnaryExpression();
    std::unique_ptr<NonTerminalNode> expectPrimaryExpression();
    //---------------------------------------------------------------------------------------------------
    /*
     * The following three functions parse terminal symbols.
     * If there is a parsing error(e.g. a component is not found), a nullptr is returned
     */
    std::unique_ptr<IdentifierNode> expectIdentifierNode();

    std::unique_ptr<LiteralNode> expectLiteralNode();

    std::unique_ptr<GenericNode> expectGenericNode(TokenTypes type);
    /*
     * this function prints error msgs for the parser
     */
    static void printErrorMsg(Token& token, std::string_view msg);
    /*
     * this function prints default error msgs for the parser if there is no next token
     */
    void printDefaultErrorMsg(std::string_view msg);
    /*
     * this function refactors grammar productions of this form, identifier{"," identifier} for declaratorList and InitDeclaratorList
     * PARAM:
     * func*: the function pointer for the expectFunction, that gets the elements of the list, mentioned above (e.g. identifier for declarator-list)
     * NodeType: the NodeType for the returned NonTerminalNode
      */
    std::unique_ptr<NonTerminalNode> refactorDeclList(auto (Parser::*func)(), Node::Types nodeType);

    /*
     * this function refactors grammar for parameter/variable/constant declarations
     * PARAM:
     * func*: the function pointer for the expectFunction mentioned above (e.g. identifier for declarator-list)
     * NodeType: the NodeType for the returned NonTerminalNode
     * keywordType: type of the keyword that starts this expression,e.g. "PARAM", "BEGIN"
     * endingKeyword:type of the keyword that ends this expression,e.g. ";", "END"
      */
    std::unique_ptr<NonTerminalNode> refactorDeclaration(auto (Parser::*func)(), Node::Types startingKeyword, TokenTypes endingKeyword, Node::Types nodeType);
    /*
     * this function refactors grammar productions of this form, identifier assignmentOperator (literal | additive-expression)
     * PARAM:
     * *func1, *func2: the function pointer for the expectFunction mentioned above (e.g. identifier for declarator-list)
     * NodeType: the NodeType for the returned NonTerminalNode
     * operator1, operator2: token type of the operator, e.g. "*","+"
      */
    std::unique_ptr<NonTerminalNode> refactorExpression(auto (Parser::*func1)(),auto (Parser::*func2)(), Node::Types nodeType, TokenTypes operator1, TokenTypes operator2);
    /*
     * this helper function resets the backtrack token
     */
    void resetBacktrackToken(){
        backtrackToken = Token(tokenizer.getManager());
    }
};
} // namespace parser
#endif //PLJIT_PARSER_H
