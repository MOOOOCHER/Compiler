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
    public:
    explicit Parser(SourceCodeManager& sourceCodeManager): sourceCodeManager(sourceCodeManager){};

    std::unique_ptr<NonTerminalNode> parse(std::vector<Token>& tokens);

    //---------------------------------------------------------------------------------------------------
    /*
     * The following three functions parse terminal symbols.
     * If there is a parsing error(e.g. a component is not found), a nullptr is returned
     */
    std::unique_ptr<IdentifierNode> expectIdentifierNode(size_t& currentPos, std::vector<Token>& tokens);

    std::unique_ptr<LiteralNode> expectLiteralNode(size_t& currentPos, std::vector<Token>& tokens);

    std::unique_ptr<GenericNode> expectGenericNode(const std::string& c, size_t& currentPos, std::vector<Token>& tokens);
    //---------------------------------------------------------------------------------------------------------
    /*
     * The following functions parse non-terminal symbols.
     * If there is a parsing error(e.g. a component is not found), a nullptr is returned
     */
    std::unique_ptr<NonTerminalNode> expectFunctionDefinition(std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectParameterDeclaration(size_t& currentPos, std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectVariableDeclaration(size_t& currentPos, std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectConstantDeclaration(size_t& currentPos, std::vector<Token>& tokens);

    std::unique_ptr<NonTerminalNode> expectDeclaratorList(size_t& currentPos, std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectInitDeclaratorList(size_t& currentPos, std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectInitDeclarator(size_t& currentPos, std::vector<Token>& tokens);

    std::unique_ptr<NonTerminalNode> expectCompoundStatement(size_t& currentPos, std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectStatementList(size_t& currentPos, std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectStatement(size_t& currentPos, std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectAssignmentExpression(size_t& currentPos, std::vector<Token>& tokens);

    std::unique_ptr<NonTerminalNode> expectAdditiveExpression(size_t& currentPos, std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectMultiplicativeExpression(size_t& currentPos, std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectUnaryExpression(size_t& currentPos, std::vector<Token>& tokens);
    std::unique_ptr<NonTerminalNode> expectPrimaryExpression(size_t& currentPos, std::vector<Token>& tokens);
    private:
    /*
     * this function refactors grammar productions of this form, identifier{"," identifier}
      */
    std::unique_ptr<NonTerminalNode> refactorList(size_t& currentPos, std::vector<Token>& tokens,auto (Parser::*func)(size_t&, std::vector<Token>&), Node::Types NodeType, lexer::TokenTypes tokenType,std::string separatorType);
};
} // namespace parser
#endif //PLJIT_PARSER_H
