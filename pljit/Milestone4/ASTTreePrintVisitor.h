#ifndef PLJIT_ASTTREEPRINTVISITOR_H
#define PLJIT_ASTTREEPRINTVISITOR_H
#include "ASTTreeVisitor.h"
namespace semantic{
class ASTTreePrintVisitor: public ASTTreeVisitor {
    size_t index = 0;
    public:
     void visit(const semantic::ASTIdentifierNode& node)override;
     void visit(const semantic::ASTLiteralNode& node)override;
     void visit(const semantic::ASTOperationExpressionNode& node)override;
     void visit(const semantic::ASTReturnStatementNode& node)override;
     void visit(const semantic::ASTAssignmentStatementNode& node)override;
     void visit(const semantic::ASTParamDeclaratorListNode& node)override;
     void visit(const semantic::ASTVarDeclaratorListNode& node)override;
     void visit(const semantic::ASTFunctionNode& node) override;
     void visit(const semantic::ASTInitDeclaratorNode& node)override;
     void visit(const semantic::ASTInitDeclaratorListNode& node) override;
     void visit(const semantic::ASTAssignmentExpression& node) override;
     void visit(const semantic::ASTCompoundStatement& node) override;
     void visit(const semantic::ASTUnaryExpression& node) override;
     void printTree(const ASTNode& node);
};
} // namespace semantic
#endif //PLJIT_ASTTREEPRINTVISITOR_H
