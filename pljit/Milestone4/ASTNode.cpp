#include "ASTNode.h"
#include "ASTTreePrintVisitor.h"
#include "../Milestone5/ASTEvaluator.h"
#include <iostream>
#include <utility>
namespace semantic{
using namespace sourceCodeManagement;
//----------------------------------------------------------------------------------------------------------------
ASTIdentifierNode::ASTIdentifierNode(ASTNodeType type, std::string_view value): ASTNode(type), value(value){}
std::string_view ASTIdentifierNode::getValue() const {
    return value;
}

ASTLiteralNode::ASTLiteralNode(double value): ASTNode(LiteralConstant), value(value){}
double ASTLiteralNode::getValue() const{
    return value;
}
//-----------------------------------------------------------------------------------------------------------------
ASTFunctionNode::ASTFunctionNode(): ASTNode(FunctionDefinition){}
void ASTFunctionNode::pop_back_child(){
    children.pop_back();
}
std::vector<std::unique_ptr<ASTNode>> ASTFunctionNode::getChildren(){
    //Only for testing
    return std::move(children);
}

ASTStatementNode::ASTStatementNode(ASTNodeType type, std::unique_ptr<ASTNode> child): ASTNode(type), child(std::move(child)){}
std::unique_ptr<ASTNode> ASTStatementNode::getChild(){
    return std::move(child);
}
ASTOperationExpressionNode::ASTOperationExpressionNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right, SourceCodeReference sourceCodeReference): ASTExpressionNode(type),leftChild(std::move(left)), rightChild(std::move(right)), sourceCodeReference(std::move(sourceCodeReference)){}
ASTAssignmentExpression::ASTAssignmentExpression(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTExpressionNode(AssignmentExpression),leftChild(std::move(left)), rightChild(std::move(right)){}
ASTUnaryExpression::ASTUnaryExpression(ASTNodeType type,std::unique_ptr<ASTNode> child): ASTExpressionNode(type),child(std::move(child)){}
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
void ASTReturnStatementNode::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTAssignmentStatementNode::accept(ASTTreeVisitor& visitor) const {
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
//Evaluation-------------------------------------------------------------------
std::optional<double> ASTFunctionNode::acceptEvaluation(ASTEvaluator& visitor)  {
    for(auto& child: children){
        auto astChild = child->acceptEvaluation(visitor);
        if(!astChild.has_value()) return {};
        if(child->getType() == ASTNode::ReturnStatement){
            return astChild;
        }
    }
    std::cout<<"something has gone wrong!"<<std::endl;
    return {};
}
std::optional<double> ASTIdentifierNode::acceptEvaluation(ASTEvaluator& visitor)  {
        //we are in the statement evaluation phase => return value for variable
        return visitor.table.get(value).value;
}
std::optional<double> ASTLiteralNode::acceptEvaluation(ASTEvaluator&) {
    return value;
}

std::optional<double> ASTReturnStatementNode::acceptEvaluation(ASTEvaluator& visitor)  {
    return child->acceptEvaluation(visitor);
}
std::optional<double> ASTAssignmentStatementNode::acceptEvaluation(ASTEvaluator& visitor)  {
    return child->acceptEvaluation(visitor);
}
std::optional<double> ASTOperationExpressionNode::acceptEvaluation(ASTEvaluator& visitor)  {
    auto leftExpr = leftChild->acceptEvaluation(visitor);
    auto rightExpr = rightChild->acceptEvaluation(visitor);
    if(!leftExpr.has_value() || !rightExpr.has_value()){
        return {};
    } else if(type == ASTNode::PlusOperator){
        return leftExpr.value() + rightExpr.value();
    } else if(type == ASTNode::MinusOperator){
        return leftExpr.value() - rightExpr.value();
    } else if(type== ASTNode::MulOperator){
        return leftExpr.value() * rightExpr.value();
    } else if(type== ASTNode::DivOperator){
        if(rightExpr == 0){
            sourceCodeReference.printContext("error: division by zero!");
            return {}; //abort compilation
        }
        return leftExpr.value() / rightExpr.value();
    } else {
        std::cout<< "something has gone wrong!"<<std::endl;
        return -1;
    }
}
std::optional<double> ASTAssignmentExpression::acceptEvaluation(ASTEvaluator& visitor)  {
    if(leftChild->getType() == ASTNode::Variable ||leftChild->getType() == ASTNode::Parameter ){
        auto astLeft = static_cast<ASTIdentifierNode*>(leftChild.get());
        auto rightLiteral = rightChild->acceptEvaluation(visitor);
        if(!rightLiteral.has_value()) return {};
        visitor.table.get(astLeft->getValue()).value = rightLiteral;
        return 0; //return code that it went smoothly
    }
    return {};
}
std::optional<double> ASTUnaryExpression::acceptEvaluation(ASTEvaluator& visitor)  {
    auto result = child->acceptEvaluation(visitor);
    if(result.has_value() && type == ASTNode::UnaryMinus){
        result.value() *= -1;
    }
    return result;
}
} // namespace semantic