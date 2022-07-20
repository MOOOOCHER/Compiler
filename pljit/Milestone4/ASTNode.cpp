#include "ASTNode.h"
#include "ASTTreePrintVisitor.h"
namespace semantic{
void ASTFunctionNode::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTIdentifierNode::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTLiteralNode::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTDeclaratorListNode::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTInitDeclaratorListNode::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTInitDeclaratorNode::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTCompoundStatement::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTStatementNode::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTOperationExpressionNode::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTAssignmentExpression::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTUnaryExpression::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
} // namespace semantic