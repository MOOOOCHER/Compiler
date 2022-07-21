#include "ASTNode.h"
#include "ASTTreePrintVisitor.h"
#include "../Milestone5/ASTEvaluator.h"
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
//Evaluation
long ASTFunctionNode::acceptEvaluation(ASTEvaluator& visitor) const {
    return visitor.evaluate(*this);
}
long ASTIdentifierNode::acceptEvaluation(ASTEvaluator& visitor) const {
    return visitor.evaluate(*this);
}
long ASTLiteralNode::acceptEvaluation(ASTEvaluator& visitor) const {
    return visitor.evaluate(*this);
}
long ASTDeclaratorListNode::acceptEvaluation(ASTEvaluator& visitor) const {
    visitor.evaluate(*this);
    return 0;   //return code 0;
}
long ASTInitDeclaratorListNode::acceptEvaluation(ASTEvaluator& visitor) const {
    visitor.evaluate(*this);
    return 0;   //return code 0;
}
long ASTInitDeclaratorNode::acceptEvaluation(ASTEvaluator& visitor) const {
    visitor.evaluate(*this);
    return 0;   //return code 0;
}
long ASTCompoundStatement::acceptEvaluation(ASTEvaluator& visitor) const {
    return visitor.evaluate(*this);
}
long ASTStatementNode::acceptEvaluation(ASTEvaluator& visitor) const {
    return visitor.evaluate(*this);
}
long ASTOperationExpressionNode::acceptEvaluation(ASTEvaluator& visitor) const {
    return visitor.evaluate(*this);
}
long ASTAssignmentExpression::acceptEvaluation(ASTEvaluator& visitor) const {
    visitor.evaluate(*this);
    return 0;   //return code 0;
}
long ASTUnaryExpression::acceptEvaluation(ASTEvaluator& visitor) const {
    return visitor.evaluate(*this);
}
} // namespace semantic