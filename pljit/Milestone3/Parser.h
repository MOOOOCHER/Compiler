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
     * PARAM:
     * optional: is this an optional check, if so we don't need to print error messages
     */
    std::unique_ptr<IdentifierNode> expectIdentifierNode();

    std::unique_ptr<LiteralNode> expectLiteralNode();

    std::unique_ptr<GenericNode> expectGenericNode(const std::string& c);
    /*
     * this function prints error msgs for the parser
     */
    static void printErrorMsg(Token& token, std::string_view msg);
    /*
     * this function prints default error msgs for the parser if there is no next token
     */
    void printDefaultErrorMsg(std::string_view msg);
    /*
     * this function refactors grammar productions of this form, identifier{"," identifier}
     * PARAM
     * currentPos: current position within the token stream (which token we currently evaluate)
     * tokens: token stream
     * func*: the function pointer for the expectFunction mentioned above (e.g. identifier for declarator-list)
     * NodeType: the NodeType for the returned NonTerminalNode
     * tokenType: the type for the token in the stream, which we need to check for invalid inputs (e.g. identifier identifier for declarator list)
     * separatorType: type of the separator between the elements of the list, e.g. "," or ";"
      */
    //std::unique_ptr<NonTerminalNode> refactorList(auto (Parser::*func)(), Node::Types nodeType, lexer::TokenTypes tokenType,const std::string& separatorType);
    /*
     * this function refactors grammar productions of this form, identifier assignmentOperator (literal | additive-expression)
     * PARAM
     * currentPos: current position within the token stream (which token we currently evaluate)
     * tokens: token stream
     * func*: the function pointer for the expectFunction mentioned above (e.g. identifier for declarator-list)
     * NodeType: the NodeType for the returned NonTerminalNode
     * separatorType: type of the assignment between the elements of the list, e.g. "=" or ":="
      */
    //std::unique_ptr<NonTerminalNode> refactorAssignmentInit(auto (Parser::*func)(), Node::Types nodeType, const std::string& assignmentType);
    /*
     * this function refactors grammar for parameter/variable/constant declarations
     * PARAM
     * currentPos: current position within the token stream (which token we currently evaluate)
     * tokens: token stream
     * func*: the function pointer for the expectFunction mentioned above (e.g. identifier for declarator-list)
     * NodeType: the NodeType for the returned NonTerminalNode
     * keywordType: type of the keyword that starts this expression,e.g. "PARAM", "BEGIN"
     * endingKeyword:type of the keyword that ends this expression,e.g. ";", "END"
      */
    //std::unique_ptr<NonTerminalNode> refactorDeclaration(auto (Parser::*func)(), Node::Types nodeType, const std::string& keywordType,const std::string& endingKeyword, bool optional);
    std::unique_ptr<NonTerminalNode> refactorExpression(auto (Parser::*func1)(),auto (Parser::*func2)(), Node::Types nodeType, const std::string& operator1, const std::string& operator2);
    /*
     * this helper function resets the backtrack token
     */
    void resetBacktrackToken(){
        backtrackToken = Token(tokenizer.getManager());
    }
};
} // namespace parser
#endif //PLJIT_PARSER_H
