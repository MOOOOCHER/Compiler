#ifndef PLJIT_PARSER_H
#define PLJIT_PARSER_H
#include <optional>
#include "Node.h"
#include "../Milestone2/Token.h"
namespace parser{
using Token = lexer::Token;
using Node = parser::Node;
class Parser{
    SourceCodeManager sourceCodeManager;
    std::vector<Token>& tokens;
    public:
    explicit Parser(SourceCodeManager& sourceCodeManager,std::vector<Token>& tokens): sourceCodeManager(sourceCodeManager), tokens(tokens){};

    std::unique_ptr<NonTerminalNode> parse();

    //---------------------------------------------------------------------------------------------------
    /*
     * The following three functions parse terminal symbols.
     * If there is a parsing error(e.g. a component is not found), a nullptr is returned
     * PARAM:
     * optional: is this an optional check, if so we don't need to print error messages TODO
     */
    std::unique_ptr<IdentifierNode> expectIdentifierNode(size_t& currentPos, bool optional);
    std::unique_ptr<IdentifierNode> expectIdentifierNode(size_t& currentPos);

    std::unique_ptr<LiteralNode> expectLiteralNode(size_t& currentPos, bool optional);
    std::unique_ptr<LiteralNode> expectLiteralNode(size_t& currentPos);

    std::unique_ptr<GenericNode> expectGenericNode(const std::string& c, size_t& currentPos, bool optional);
    //---------------------------------------------------------------------------------------------------------
    /*
     * The following functions parse non-terminal symbols.
     * If there is a parsing error(e.g. a component is not found), a nullptr is returned
     */
    std::unique_ptr<NonTerminalNode> expectFunctionDefinition(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectParameterDeclaration(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectVariableDeclaration(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectConstantDeclaration(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectParameterDeclaration(size_t& currentPos, bool optional);
    std::unique_ptr<NonTerminalNode> expectVariableDeclaration(size_t& currentPos, bool optional);
    std::unique_ptr<NonTerminalNode> expectConstantDeclaration(size_t& currentPos, bool optional);

    std::unique_ptr<NonTerminalNode> expectDeclaratorList(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectInitDeclaratorList(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectInitDeclarator(size_t& currentPos);

    std::unique_ptr<NonTerminalNode> expectCompoundStatement(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectStatementList(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectStatement(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectAssignmentExpression(size_t& currentPos);

    std::unique_ptr<NonTerminalNode> expectAdditiveExpression(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectMultiplicativeExpression(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectUnaryExpression(size_t& currentPos);
    std::unique_ptr<NonTerminalNode> expectPrimaryExpression(size_t& currentPos);
    private:
    /*
     * this function prints error msgs for the parser
     */
    void printErrorMsg(size_t location, std::string_view msg);
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
    std::unique_ptr<NonTerminalNode> refactorList(size_t& currentPos,auto (Parser::*func)(size_t&), Node::Types nodeType, lexer::TokenTypes tokenType,const std::string& separatorType);
    /*
     * this function refactors grammar productions of this form, identifier assignmentOperator (literal | additive-expression)
     * PARAM
     * currentPos: current position within the token stream (which token we currently evaluate)
     * tokens: token stream
     * func*: the function pointer for the expectFunction mentioned above (e.g. identifier for declarator-list)
     * NodeType: the NodeType for the returned NonTerminalNode
     * separatorType: type of the assignment between the elements of the list, e.g. "=" or ":="
      */
    std::unique_ptr<NonTerminalNode> refactorAssignmentInit(size_t& currentPos,auto (Parser::*func)(size_t&), Node::Types nodeType, const std::string& assignmentType);
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
    std::unique_ptr<NonTerminalNode> refactorDeclaration(size_t& currentPos,auto (Parser::*func)(size_t&), Node::Types nodeType, const std::string& keywordType,const std::string& endingKeyword, bool optional);
    std::unique_ptr<NonTerminalNode> refactorExpression(size_t& currentPos,auto (Parser::*func1)(size_t&),auto (Parser::*func2)(size_t&), Node::Types nodeType, const std::string& operator1, const std::string& operator2);

};
} // namespace parser
#endif //PLJIT_PARSER_H
