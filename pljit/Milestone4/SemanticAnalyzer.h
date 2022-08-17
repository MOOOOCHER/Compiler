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
    bool refactorDeclaration(auto& astNode,ASTNode::ASTNodeType astChildType, parser::NonTerminalNode& parseNode);

    std::unique_ptr<ASTParamDeclaratorListNode> analyzeParameterDeclaration(parser::ParameterDeclarationNode& parseNode);
    std::unique_ptr<ASTVarDeclaratorListNode> analyzeVariableDeclaration(parser::VariableDeclarationNode& parseNode);
    std::unique_ptr<ASTInitDeclaratorListNode> analyzeConstantDeclaration(parser::ConstantDeclarationNode& parseNode);
    std::unique_ptr<ASTCompoundStatement> analyzeCompoundStatement(parser::CompoundStatementNode& parseNode);
    std::unique_ptr<ASTStatementNode> analyzeStatement(parser::StatementNode& parseNode);
    std::unique_ptr<ASTNode> analyzeExpression(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTNode> analyzeInitDeclarator(parser::InitDeclaratorNode& parseNode);
    /*
     * this function is used for identifier which are being initialized (in declarator list)
     */
    std::unique_ptr<ASTNode> analyzeInitIdentifier(ASTNode::ASTNodeType type, parser::IdentifierNode& parseNode);
    /*
     * this function is used for identifier which are being used in expressions
     */
    std::unique_ptr<ASTIdentifierNode> analyzeIdentifier(parser::IdentifierNode& parseNode);
    /*
     * this function is used for identifier which are being used in expressions
     */
    std::unique_ptr<ASTIdentifierNode> analyzeAssignIdentifier(parser::IdentifierNode& parseNode);
    public:
    std::unique_ptr<ASTFunctionNode> analyzeFunction(parser::FunctionDefinitionNode& parseNode);
    SemanticAnalyzer() = default;
};
} // namespace semantic
#endif //PLJIT_SEMANTICANALYZER_H
