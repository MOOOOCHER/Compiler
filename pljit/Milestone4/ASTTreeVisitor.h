#ifndef PLJIT_ASTTREEVISITOR_H
#define PLJIT_ASTTREEVISITOR_H
#include "ASTNode.h"
namespace semantic{
class ASTTreeVisitor{
    public:
    virtual ~ASTTreeVisitor() = default;
    virtual void visit(const semantic::ASTIdentifierNode& node) = 0;
    virtual void visit(const semantic::ASTLiteralNode& node) = 0;
    virtual void visit(const semantic::ASTOperationExpressionNode& node) = 0;
    virtual void visit(const semantic::ASTStatementNode& node) = 0;
    virtual void visit(const semantic::ASTDeclaratorListNode& node) = 0;
    virtual void visit(const semantic::ASTFunctionNode& node) = 0;
    virtual void visit(const semantic::ASTInitDeclaratorNode& node) = 0;
    virtual void visit(const semantic::ASTInitDeclaratorListNode& node) = 0;
    virtual void visit(const semantic::ASTAssignmentExpression& node) = 0;
    virtual void visit(const semantic::ASTCompoundStatement& node) = 0;
    virtual void visit(const semantic::ASTUnaryExpression& node) = 0;
    virtual void visit(const semantic::ASTParamIdentifierNode& node) = 0;
};
} // namespace semantic
#endif //PLJIT_ASTTREEVISITOR_H
