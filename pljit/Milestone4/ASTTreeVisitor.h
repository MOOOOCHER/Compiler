#ifndef PLJIT_ASTTREEVISITOR_H
#define PLJIT_ASTTREEVISITOR_H
#include "pljit/ASTDataStructures/ASTNode.h"
namespace semantic{
class ASTTreeVisitor{
    public:
    virtual ~ASTTreeVisitor() = default;
    virtual void visit(const semantic::ASTIdentifierNode& node) = 0;
    virtual void visit(const semantic::ASTLiteralNode& node) = 0;
    virtual void visit(const semantic::ASTOperationExpressionNode& node) = 0;
    virtual void visit(const semantic::ASTReturnStatementNode& node) = 0;
    virtual void visit(const semantic::ASTAssignmentStatementNode& node) = 0;
    virtual void visit(const semantic::ASTFunctionNode& node) = 0;
    virtual void visit(const semantic::ASTAssignmentExpression& node) = 0;
    virtual void visit(const semantic::ASTUnaryExpression& node) = 0;
    virtual void visit(const semantic::ASTBracketExpression& node) = 0;
};
} // namespace semantic
#endif //PLJIT_ASTTREEVISITOR_H
