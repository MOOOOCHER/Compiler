#include "ASTEvaluator.h"
#include <iostream>
#include <utility>
#include "../Milestone1/SourceCodeManager.h"
namespace semantic{
std::optional<double> ASTEvaluator::evaluateFunction(std::vector<long> arg,semantic::ASTNode& node){
    if(!initArguments(std::move(arg),node)){
        return {};
    }
    return node.acceptEvaluation(*this);
}
bool ASTEvaluator::initArguments(std::vector<long> arg, semantic::ASTNode& node) {
    //initialize Parameter
    auto functionDefinition = static_cast<ASTFunctionNode*>(&node);
    for(auto child: functionDefinition->getChildren()){
        if(child->getType() == ASTNode::DeclaratorList){
            auto declList = static_cast<ASTDeclaratorListNode*>(child);
            if(declList->getChildren()[0]->getType() == ASTNode::Parameter){
                if(declList->getChildren().size() != arg.size()){
                    //if parameters size don't match
                    sourceCodeManagement::SourceCodeManager defaultManager = sourceCodeManagement::SourceCodeManager();
                    sourceCodeManagement::SourceCodeReference a = sourceCodeManagement::SourceCodeReference (defaultManager);
                    a.printContext("error: parameter size doesn't match with argument size!");
                    return false;
                }
                for(size_t i=0;i<arg.size();i++){
                    auto parameterNode = static_cast<ASTParamIdentifierNode*>(declList->getChildren()[i]);
                    parameterNode->paramValue = arg[i];
                }
            } else {
                if(!arg.empty()){
                    //if parameters size don't match
                    sourceCodeManagement::SourceCodeManager defaultManager = sourceCodeManagement::SourceCodeManager();
                    sourceCodeManagement::SourceCodeReference a = sourceCodeManagement::SourceCodeReference (defaultManager);
                    a.printContext("error: parameter size doesn't match with argument size!");
                    return false;
                }
            }
            break;
        }
    }
    return true;
}
//-------------------------------------------------------------------------------------------------
std::optional<double> ASTEvaluator::evaluate( semantic::ASTFunctionNode& node){
    for(auto child: node.getChildren()){
        if(child->getType() == ASTNode::CompoundStatement){
            return child->acceptEvaluation(*this);
        }
        child->acceptEvaluation(*this);
    }
    std::cout<< "something has gone wrong!"<<std::endl;
    return {};
}
std::optional<double> ASTEvaluator::evaluate(const semantic::ASTDeclaratorListNode& node){
    for(auto child: node.getChildren()){
        child->acceptEvaluation(*this);
    }
    return {};
}
std::optional<double> ASTEvaluator::evaluate(const semantic::ASTInitDeclaratorListNode&){
   //do nothing because constants are optimized out
    return {};
}
std::optional<double> ASTEvaluator::evaluate(const semantic::ASTInitDeclaratorNode& node){
  //we are in the initialization phase, Init Declarator only applies to constants
    node.getLeftChild()->acceptEvaluation(*this);
    auto astLeft = static_cast<ASTIdentifierNode*>(node.getLeftChild());
    auto variableName = astLeft->getValue();

    auto rightLiteral = node.getLeftChild()->acceptEvaluation(*this);
    variables.find(variableName)->second = rightLiteral;
    return {};
}
std::optional<double> ASTEvaluator::evaluate( semantic::ASTIdentifierNode& node){
    if(!variables.contains(node.getValue())){
        //we are in the initialization phase
        variables.insert(std::pair<std::string_view,std::optional<double>>(node.getValue(),std::optional<double>()));
        return 0;
    } else {
        //we are in the statement evaluation phase => return value for variable
        return variables.find(node.getValue())->second.value();
    }
}
std::optional<double> ASTEvaluator::evaluate( semantic::ASTLiteralNode& node){
    return node.getValue();
}
std::optional<double> ASTEvaluator::evaluate( semantic::ASTOperationExpressionNode& node){
    auto leftExpr = node.getLeftChild()->acceptEvaluation(*this);
    auto rightExpr = node.getRightChild()->acceptEvaluation(*this);
    if(!leftExpr.has_value() || !rightExpr.has_value()){
        return {};
    } else if(node.getType() == ASTNode::PlusOperator){
        return leftExpr.value() + rightExpr.value();
    } else if(node.getType() == ASTNode::MinusOperator){
        return leftExpr.value() - rightExpr.value();
    } else if(node.getType() == ASTNode::MulOperator){
        return leftExpr.value() * rightExpr.value();
    } else if(node.getType() == ASTNode::DivOperator){
        if(rightExpr == 0){
            sourceCodeManagement::SourceCodeManager defaultManager = sourceCodeManagement::SourceCodeManager();
            sourceCodeManagement::SourceCodeReference a = sourceCodeManagement::SourceCodeReference (defaultManager);
            a.printContext("error: division by zero!");
            return {}; //abort compilation
        }
        return leftExpr.value() / rightExpr.value();
    } else {
        std::cout<< "something has gone wrong!"<<std::endl;
        return -1;
    }
}
std::optional<double> ASTEvaluator::evaluate( semantic::ASTStatementNode& node){
    return node.getChild()->acceptEvaluation(*this);
}
std::optional<double> ASTEvaluator::evaluate( semantic::ASTAssignmentExpression& node){
    if(node.getLeftChild()->getType() == ASTNode::Variable ||node.getLeftChild()->getType() == ASTNode::Parameter ){
        auto astLeft = static_cast<ASTIdentifierNode*>(node.getLeftChild());
        auto variableName = astLeft->getValue();

        auto rightLiteral = node.getRightChild()->acceptEvaluation(*this);
        if(!rightLiteral.has_value()) return {};
        variables.find(variableName)->second = rightLiteral;
        return 0; //return code that it went smoothly
    }
    return {};
}
std::optional<double> ASTEvaluator::evaluate( semantic::ASTCompoundStatement& node){
    for(auto child: node.getChildren()){
        auto astChild = child->acceptEvaluation(*this);
        if(!astChild.has_value()) return {};
        if(child->getType() == ASTNode::ReturnStatement){
            return astChild;
        }
    }
    std::cout<<"something has gone wrong!"<<std::endl;
    return {};
}
std::optional<double> ASTEvaluator::evaluate( semantic::ASTUnaryExpression& node){
    auto result = node.getChild()->acceptEvaluation(*this);
    if(result.has_value() && node.getType() == ASTNode::UnaryMinus){
        result.value() *= -1;
    }
    return result;
}
std::optional<double> ASTEvaluator::evaluate( semantic::ASTParamIdentifierNode& node){
    if(!variables.contains(node.getValue())){
        //we are in the initialization phase
        variables.insert(std::pair<std::string_view,std::optional<double>>(node.getValue(),node.paramValue));
        return 0;
    } else {
        //we are in the statement evaluation phase => return value for variable
        return variables.find(node.getValue())->second.value();
    }
}
} // namespace semantic