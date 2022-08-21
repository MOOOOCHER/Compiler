#ifndef PLJIT_SEMANTICANALYZER_H
#define PLJIT_SEMANTICANALYZER_H
#include "../Milestone3/Node.h"
#include "ASTSymbolTable.h"
#include "pljit/ASTDataStructures/ASTNode.h"
#include "pljit/ASTDataStructures/ASTTree.h"
#include <memory>
namespace semantic{
class SemanticAnalyzer {
    ASTSymbolTable table;

    bool refactorDeclaration(ASTNode::ASTNodeType astChildType,parser::NonTerminalNode& parseNode);
    bool analyzeParameterDeclaration(parser::ParameterDeclarationNode& parseNode);
    bool analyzeVariableDeclaration(parser::VariableDeclarationNode& parseNode);
    bool analyzeConstantDeclaration(parser::ConstantDeclarationNode& parseNode);
    bool analyzeInitDeclarator(parser::InitDeclaratorNode& parseNode);
    /*
     * this function is used for identifier which are being initialized (in declarator list)
     */
    bool analyzeInitIdentifier(ASTNode::ASTNodeType type, parser::IdentifierNode& parseNode, auto value);

    std::unique_ptr<ASTStatementNode> analyzeStatement(parser::StatementNode& parseNode);
    std::unique_ptr<ASTNode> analyzeExpression(parser::NonTerminalNode& parseNode);
    /*
     * this function is used for identifier which are being used in expressions
     */
    std::unique_ptr<ASTIdentifierNode> analyzeIdentifier(parser::IdentifierNode& parseNode);
    /*
     * this function is used for identifier which are being used in assign statements on the left hand side
     */
    std::unique_ptr<ASTIdentifierNode> analyzeAssignIdentifier(parser::IdentifierNode& parseNode);
    std::unique_ptr<ASTFunctionNode> analyzeFunction(parser::FunctionDefinitionNode& parseNode);
    public:
    std::unique_ptr<ASTTree> analyzeSemantic(parser::FunctionDefinitionNode& parseNode);
};
} // namespace semantic
#endif //PLJIT_SEMANTICANALYZER_H
