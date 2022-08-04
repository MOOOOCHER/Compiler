#include "ASTNode.h"
#include "ASTTreePrintVisitor.h"
#include "../Milestone5/ASTEvaluator.h"
#include "../Milestone1/SourceCodeManager.h"
#include <iostream>
namespace semantic{
using namespace sourceCodeManagement;
ASTUnaryNode::ASTUnaryNode(ASTNodeType type,std::unique_ptr<ASTNode> child): ASTNode(type),child(std::move(child)){}
ASTBinaryNode::ASTBinaryNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTNode(type),leftChild(std::move(left)),rightChild(std::move(right)){}
MultiASTNode::MultiASTNode(ASTNodeType type): ASTNode(type){}
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
ASTFunctionNode::ASTFunctionNode(): MultiASTNode(FunctionDefinition){}
ASTParamDeclaratorListNode::ASTParamDeclaratorListNode(): MultiASTNode(ParamDeclaratorList){}
ASTVarDeclaratorListNode::ASTVarDeclaratorListNode(): MultiASTNode(VarDeclaratorList){}
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
void ASTParamDeclaratorListNode::accept(ASTTreeVisitor& visitor) const {
    visitor.visit(*this);
}
void ASTVarDeclaratorListNode::accept(ASTTreeVisitor& visitor) const {
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
//Evaluation-------------------------------------------------------------------
std::optional<double> ASTFunctionNode::acceptEvaluation(ASTEvaluator& visitor)  {
    auto printErrorParameter = []() -> std::optional<double>{
        SourceCodeManager defaultManager = SourceCodeManager();
        SourceCodeReference a = SourceCodeReference (defaultManager);
        a.printContext("error: parameter size doesn't match with argument size!");
        return {};
    };

    bool hasParam = false;
    for(auto& child: children){
        if(child->getType() == ASTNode::CompoundStatement){
            return child->acceptEvaluation(visitor);
        }else if(child->getType() == ParamDeclaratorList){
            hasParam = true;
            if(!child->acceptEvaluation(visitor).has_value()){
                return printErrorParameter();
            }
        } else {
            if(!hasParam && !visitor.arguments.empty()){
                return printErrorParameter();
            }
            child->acceptEvaluation(visitor);
        }

    }
    std::cout<< "something has gone wrong!"<<std::endl;
    return {};
}
std::optional<double> ASTIdentifierNode::acceptEvaluation(ASTEvaluator& visitor)  {
    if(!visitor.variables.contains(value)){
        //we are in the initialization phase
        visitor.variables.insert(std::pair<std::string_view,std::optional<double>>(value,std::optional<double>()));
        return 0;
    } else {
        //we are in the statement evaluation phase => return value for variable
        return visitor.variables.find(value)->second.value();
    }
}
std::optional<double> ASTLiteralNode::acceptEvaluation(ASTEvaluator&)  {
    return value;
}
std::optional<double> ASTParamDeclaratorListNode::acceptEvaluation(ASTEvaluator& visitor)  {
    if(children.size() != visitor.arguments.size()){
        //if parameters size don't match
        return {};
    } else {
        for(size_t i=0;i<children.size();i++){
            auto identifierChild = static_cast<ASTIdentifierNode*>(children[i].get());
            visitor.variables.insert(std::make_pair(identifierChild->getValue(),visitor.arguments[i]));
        }
    }
    return 0;
}
std::optional<double> ASTVarDeclaratorListNode::acceptEvaluation(ASTEvaluator& visitor)  {
    for(auto& child: children){
        child->acceptEvaluation(visitor);
    }
    return {};
}
std::optional<double> ASTInitDeclaratorListNode::acceptEvaluation(ASTEvaluator&)  {
    //do nothing because constants are optimized out
    return {};
}
std::optional<double> ASTInitDeclaratorNode::acceptEvaluation(ASTEvaluator& visitor)  {
    //we are in the initialization phase, Init Declarator only applies to constants
    leftChild->acceptEvaluation(visitor);
    auto astLeft = static_cast<ASTIdentifierNode*>(leftChild.get());
    auto variableName = astLeft->getValue();

    auto rightLiteral = leftChild->acceptEvaluation(visitor);
    visitor.variables.find(variableName)->second = rightLiteral;
    return {};
}
std::optional<double> ASTCompoundStatement::acceptEvaluation(ASTEvaluator& visitor)  {
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
std::optional<double> ASTStatementNode::acceptEvaluation(ASTEvaluator& visitor)  {
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
            SourceCodeManager defaultManager = SourceCodeManager();
            SourceCodeReference a = SourceCodeReference (defaultManager);
            a.printContext("error: division by zero!");
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
        auto variableName = astLeft->getValue();

        auto rightLiteral = rightChild->acceptEvaluation(visitor);
        if(!rightLiteral.has_value()) return {};
        visitor.variables.find(variableName)->second = rightLiteral;
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