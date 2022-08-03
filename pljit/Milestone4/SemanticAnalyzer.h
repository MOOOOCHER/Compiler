#ifndef PLJIT_SEMANTICANALYZER_H
#define PLJIT_SEMANTICANALYZER_H
#include "ASTNode.h"
#include "ASTSymbolTable.h"
#include "../Milestone3/Node.h"
#include <memory>
namespace semantic{
class SemanticAnalyzer {
    ASTSymbolTable table;
    /*
     * this function returns the ASTNode for a given non-terminal child parse node.
     */
    std::unique_ptr<ASTNode> getChild( auto func, auto child);
    std::unique_ptr<ASTDeclaratorListNode> analyzeParameterDeclaration(const std::vector<long>& arg,parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTDeclaratorListNode> analyzeVariableDeclaration(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTInitDeclaratorListNode> analyzeConstantDeclaration(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTCompoundStatement> analyzeCompoundStatement(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTStatementNode> analyzeStatement(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTNode> analyzeExpression(parser::NonTerminalNode& parseNode);
    /*
     * this function is used for identifier which are being initialized (in declarator list)
     */
    std::unique_ptr<ASTNode> analyzeInitIdentifier(ASTNode::ASTNodeType type, parser::IdentifierNode& parseNode);
    /*
     * this function is used for identifier which are being used in expressions
     */
    std::unique_ptr<ASTIdentifierNode> analyzeIdentifier(parser::IdentifierNode& parseNode);
    std::unique_ptr<ASTNode> analyzeInitDeclarator(parser::NonTerminalNode& parseNode);
    public:
    std::unique_ptr<ASTFunctionNode> analyzeFunction(const std::vector<long>& arg, parser::NonTerminalNode& parseNode);
    SemanticAnalyzer() = default;
};
} // namespace semantic
#endif //PLJIT_SEMANTICANALYZER_H
