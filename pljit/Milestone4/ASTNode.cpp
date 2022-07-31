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
std::optional<double> ASTFunctionNode::acceptEvaluation(ASTEvaluator& visitor)  {
    return visitor.evaluate(*this);
}
std::optional<double> ASTIdentifierNode::acceptEvaluation(ASTEvaluator& visitor)  {
    return visitor.evaluate(*this);
}
std::optional<double> ASTLiteralNode::acceptEvaluation(ASTEvaluator& visitor)  {
    return visitor.evaluate(*this);
}
std::optional<double> ASTDeclaratorListNode::acceptEvaluation(ASTEvaluator& visitor)  {
    visitor.evaluate(*this);
    return 0;   //return code 0;
}
std::optional<double> ASTInitDeclaratorListNode::acceptEvaluation(ASTEvaluator& visitor)  {
    visitor.evaluate(*this);
    return 0;   //return code 0;
}
std::optional<double> ASTInitDeclaratorNode::acceptEvaluation(ASTEvaluator& visitor)  {
    visitor.evaluate(*this);
    return 0;   //return code 0;
}
std::optional<double> ASTCompoundStatement::acceptEvaluation(ASTEvaluator& visitor)  {
    return visitor.evaluate(*this);
}
std::optional<double> ASTStatementNode::acceptEvaluation(ASTEvaluator& visitor)  {
    return visitor.evaluate(*this);
}
std::optional<double> ASTOperationExpressionNode::acceptEvaluation(ASTEvaluator& visitor)  {
    return visitor.evaluate(*this);
}
std::optional<double> ASTAssignmentExpression::acceptEvaluation(ASTEvaluator& visitor)  {
    return visitor.evaluate(*this);   //return code 0;
}
std::optional<double> ASTUnaryExpression::acceptEvaluation(ASTEvaluator& visitor)  {
    return visitor.evaluate(*this);
}

void ASTParamIdentifierNode::accept(ASTTreeVisitor& visitor) const{
    visitor.visit(*this);
}
std::optional<double> ASTParamIdentifierNode::acceptEvaluation(ASTEvaluator& visitor) {
    return visitor.evaluate(*this);
}
} // namespace semantic