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
     void visit(const semantic::ASTFunctionNode& node) override;
     void visit(const semantic::ASTAssignmentExpression& node) override;
     void visit(const semantic::ASTUnaryExpression& node) override;
     void visit(const semantic::ASTBracketExpression& node) override;

     void printValueAST(const auto& node);
     void printUnaryAST(const auto& node);
     void printBinaryAST(const auto& node);

     void printTree(const ASTNode& node);
};
} // namespace semantic
#endif //PLJIT_ASTTREEPRINTVISITOR_H
