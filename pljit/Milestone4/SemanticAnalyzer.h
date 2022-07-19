#ifndef PLJIT_SEMANTICANALYZER_H
#define PLJIT_SEMANTICANALYZER_H
#include "ASTNode.h"
#include "ASTSymbolTable.h"
#include "../Milestone3/Node.h"
namespace semantic{
class SemanticAnalyzer {
    ASTSymbolTable table;
    std::unique_ptr<ASTDeclaratorListNode> analyzeParameterDeclaration(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTDeclaratorListNode> analyzeVariableDeclaration(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTInitDeclaratorListNode> analyzeConstantDeclaration(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTCompoundStatement> analyzeCompoundStatement(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTStatementNode> analyzeStatement(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTNode> analyzeExpression(parser::NonTerminalNode& parseNode);
    std::unique_ptr<ASTIdentifierNode> analyzeInitIdentifier(ASTNode::ASTNodeType type, parser::IdentifierNode& parseNode);
    std::unique_ptr<ASTIdentifierNode> analyzeIdentifier(parser::IdentifierNode& parseNode);
    std::unique_ptr<ASTNode> analyzeInitDeclarator(parser::NonTerminalNode& parseNode);
    public:
    std::unique_ptr<ASTFunctionNode> analyzeFunction(parser::NonTerminalNode& parseNode);
    SemanticAnalyzer(): table(ASTSymbolTable()){}
};
}//semantic
#endif //PLJIT_SEMANTICANALYZER_H
