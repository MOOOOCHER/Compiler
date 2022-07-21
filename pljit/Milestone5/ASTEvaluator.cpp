#include "ASTEvaluator.h"
#include <iostream>
#include "../Milestone1/SourceCodeManager.h"
namespace semantic{
long ASTEvaluator::evaluate(const semantic::ASTFunctionNode& node){
    for(auto child: node.getChildren()){
        if(child->getType() == ASTNode::CompoundStatement){
            return child->acceptEvaluation(*this);
        }
        child->acceptEvaluation(*this);
    }
    std::cout<< "something has gone wrong!"<<std::endl;
    return -1;
}
void ASTEvaluator::evaluate(const semantic::ASTDeclaratorListNode& node){
    for(auto child: node.getChildren()){
        child->acceptEvaluation(*this);
    }
}
void ASTEvaluator::evaluate(const semantic::ASTInitDeclaratorListNode& node){
    for(auto child: node.getChildren()){
        child->acceptEvaluation(*this);
    }
}
void ASTEvaluator::evaluate(const semantic::ASTInitDeclaratorNode& node){
  //we are in the initialization phase, Init Declarator only applies to constants
    node.getLeftChild()->acceptEvaluation(*this);
    auto astLeft = static_cast<ASTIdentifierNode*>(node.getLeftChild());
    auto variableName = astLeft->getValue();

    long rightLiteral = node.getLeftChild()->acceptEvaluation(*this);
    variables.find(astLeft->getValue())->second = std::make_optional<long>(rightLiteral);
}
long ASTEvaluator::evaluate(const semantic::ASTIdentifierNode& node){
    if(!variables.contains(node.getValue())){
        //we are in the initialization phase
        variables.insert(std::pair<std::string,std::optional<long>>(node.getValue(),std::optional<long>()));
        return 0;
    } else {
        //we are in the statement evaluation phase => return value for variable
        return variables.find(node.getValue())->second.value();
    }
}
long ASTEvaluator::evaluate(const semantic::ASTLiteralNode& node){
    return node.getValue();
}
long ASTEvaluator::evaluate(const semantic::ASTOperationExpressionNode& node){
    auto leftExpr = node.getLeftChild()->acceptEvaluation(*this);
    auto rightExpr = node.getRightChild()->acceptEvaluation(*this);
    if(node.getType() == ASTNode::PlusOperator){
        return leftExpr + rightExpr;
    } else if(node.getType() == ASTNode::MinusOperator){
        return leftExpr - rightExpr;
    } else if(node.getType() == ASTNode::MulOperator){
        return leftExpr * rightExpr;
    } else if(node.getType() == ASTNode::DivOperator){
        if(rightExpr == 0){
            sourceCodeManagement::SourceCodeManager defaultManager = sourceCodeManagement::SourceCodeManager();
            sourceCodeManagement::SourceCodeReference a = sourceCodeManagement::SourceCodeReference (defaultManager);
            a.printContext("error: division by zero!");
            return -1; //todo: abort computation
        }
        return leftExpr / rightExpr;
    } else {
        std::cout<< "something has gone wrong!"<<std::endl;
        return -1;
    }
}
long ASTEvaluator::evaluate(const semantic::ASTStatementNode& node){
    return node.getChild()->acceptEvaluation(*this);
}
long ASTEvaluator::evaluate(const semantic::ASTAssignmentExpression& node){
    if(node.getLeftChild()->getType() == ASTNode::Variable ||node.getLeftChild()->getType() == ASTNode::Parameter ){
        auto astLeft = static_cast<ASTIdentifierNode*>(node.getLeftChild());
        auto variableName = astLeft->getValue();

        long rightLiteral = node.getLeftChild()->acceptEvaluation(*this);
        variables.find(astLeft->getValue())->second = std::make_optional<long>(rightLiteral);
        return 0;
    }
    return -1;
}
long ASTEvaluator::evaluate(const semantic::ASTCompoundStatement& node){
    for(auto child: node.getChildren()){
        if(child->getType() == ASTNode::ReturnStatement){
            return child->acceptEvaluation(*this);
        }
        child->acceptEvaluation(*this);
    }
    std::cout<< "something has gone wrong!"<<std::endl;
    return -1;
}
long ASTEvaluator::evaluate(const semantic::ASTUnaryExpression& node){
    long result = node.getChild()->acceptEvaluation(*this);
    if(node.getType() == ASTNode::UnaryMinus){
        result *= -1;
    }
    return result;
}
} // namespace semantic