#include "ASTNode.h"
#include "ASTTreePrintVisitor.h"
#include "../Milestone5/ASTEvaluator.h"
namespace semantic{
ASTUnaryNode::ASTUnaryNode(ASTNodeType type,std::unique_ptr<ASTNode> child): ASTNode(type),child(std::move(child)){}
ASTNode* ASTUnaryNode::getChild() const{
    return child.get();
}

ASTBinaryNode::ASTBinaryNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTNode(type),leftChild(std::move(left)),rightChild(std::move(right)){}
ASTNode* ASTBinaryNode::getLeftChild() const{
    return leftChild.get();
}
ASTNode* ASTBinaryNode::getRightChild() const{
    return rightChild.get();
}
MultiASTNode::MultiASTNode(ASTNodeType type): ASTNode(type){}
std::vector<ASTNode*> MultiASTNode::getChildren() const{
    std::vector<ASTNode*> vec;
    for(auto& child: children){
        vec.push_back(child.get());
    }
    return vec;
}
//----------------------------------------------------------------------------------------------------------------
ASTIdentifierNode::ASTIdentifierNode(ASTNodeType type, std::string_view value): ASTNode(type), value(value){}
std::string_view ASTIdentifierNode::getValue() const {
    return value;
}
ASTParamIdentifierNode::ASTParamIdentifierNode(std::string_view value): ASTIdentifierNode(Parameter, value){}

ASTLiteralNode::ASTLiteralNode(double value): ASTNode(LiteralConstant), value(value){}
double ASTLiteralNode::getValue() const{
    return value;
}
//-----------------------------------------------------------------------------------------------------------------
ASTFunctionNode::ASTFunctionNode(): MultiASTNode(FunctionDefinition){}
ASTDeclaratorListNode::ASTDeclaratorListNode(ASTNodeType type): MultiASTNode(type){}
ASTInitDeclaratorListNode::ASTInitDeclaratorListNode(): MultiASTNode(InitDeclaratorList){}
ASTInitDeclaratorNode::ASTInitDeclaratorNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTBinaryNode(InitDeclarator,std::move(left), std::move(right)){}

ASTCompoundStatement::ASTCompoundStatement(): MultiASTNode(CompoundStatement){}
void ASTCompoundStatement::pop_back_child(){ children.pop_back(); }

ASTStatementNode::ASTStatementNode(ASTNodeType type, std::unique_ptr<ASTNode> child): ASTUnaryNode(type, std::move(child)){}
ASTOperationExpressionNode::ASTOperationExpressionNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTBinaryNode(type,std::move(left), std::move(right)){}
ASTAssignmentExpression::ASTAssignmentExpression(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTBinaryNode(AssignmentExpression,std::move(left), std::move(right)){}
ASTUnaryExpression::ASTUnaryExpression(ASTNodeType type,std::unique_ptr<ASTNode> child): ASTUnaryNode(type, std::move(child)){}
//-----------------------------------------------------------------------------------------------------------------
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