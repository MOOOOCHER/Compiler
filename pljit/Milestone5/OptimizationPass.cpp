#include "OptimizationPass.h"
namespace semantic{
    void DeadCodeEliminationPass::optimize(ASTNode& node) {
        if(node.getType() == ASTNode::FunctionDefinition){
            auto functionDefinition = static_cast<ASTFunctionNode*>(&node);
            for(auto child: functionDefinition->getChildren()){
                if(child->getType() == ASTNode::CompoundStatement){
                    optimize(*child);
                    return;
                }
            }
        }
        else if(node.getType() == ASTNode::CompoundStatement){
            auto compoundStatement = static_cast<ASTCompoundStatement*>(&node);
            while(true){
                if(compoundStatement->getChildren()[compoundStatement->getChildren().size()-1]->getType() == ASTNode::ReturnStatement){
                    return;
                }
                compoundStatement->pop_back_child();
            }
        }
    }
    //TODO: Fix constantPropagation
    void ConstantPropagationPass::optimize(ASTNode& node) {
        if(node.getType() == ASTNode::FunctionDefinition){
            auto functionDefinition = static_cast<ASTFunctionNode*>(&node);
            for(auto child: functionDefinition->getChildren()){
                if(child->getType() == ASTNode::InitDeclaratorList){
                    optimize(*child);
                } else if(child->getType() == ASTNode::CompoundStatement){
                    optimize(*child);
                    return;
                }
            }
        } else if(node.getType() == ASTNode::InitDeclaratorList){
            //initialize table
            auto constInitDeclList = static_cast<ASTInitDeclaratorListNode*>(&node);
            for(auto child: constInitDeclList->getChildren()){
                auto constInitDecl = static_cast<ASTInitDeclaratorNode*>(child);
                //get Values
                auto name = static_cast<ASTIdentifierNode*>(constInitDecl->getLeftChild());
                auto value = static_cast<ASTLiteralNode*>(constInitDecl->getRightChild());
                variables.insert(std::pair<std::string,double>(name->getValue(),value->getValue()));
            }
        } else if(node.getType() == ASTNode::CompoundStatement){
            auto compoundStatement = static_cast<ASTCompoundStatement*>(&node);
            for(auto child: compoundStatement->getChildren()){
                    optimizeStatement(*child);
            }
            //TODO: if return statement is constant, we could omit the evaluation
        }
    }
    std::optional<double> ConstantPropagationPass::optimizeStatement(ASTNode& node){
        if(node.getType() == ASTNode::AssignStatement){
            auto statement = static_cast<ASTStatementNode*>(&node);
            auto optimized = optimizeStatement(*statement->getChild());
            return optimized;
        } else if(node.getType() == ASTNode::ReturnStatement){
            auto statement = static_cast<ASTStatementNode*>(&node);
            auto optimized = optimizeExpression(*statement->getChild());
            if(optimized.has_value()){
                //change child to constant
                statement->child = std::make_unique<ASTLiteralNode>(optimized.value());
            }
            return optimized;
        } else if(node.getType() == ASTNode::AssignmentExpression){
            auto assignmentExpr = static_cast<ASTAssignmentExpression*>(&node);
            auto optimized = optimizeExpression(*assignmentExpr->getRightChild());
            if(optimized.has_value()){
                auto name = static_cast<ASTIdentifierNode*>(assignmentExpr->getLeftChild());
                //change child to constant
                assignmentExpr->rightChild = std::make_unique<ASTLiteralNode>(optimized.value());
                variables.insert(std::pair<std::string,double>(name->getValue(),optimized.value()));
            }
            return optimized;
        }
        return std::optional<double>();
    }
    std::optional<double> ConstantPropagationPass::optimizeExpression(ASTNode& node){
        if(node.getType() == ASTNode::UnaryPlus || node.getType() == ASTNode::UnaryMinus){
            auto unaryExpr = static_cast<ASTUnaryExpression*>(&node);
            auto optimized = optimizeStatement(*unaryExpr->getChild());
            if(optimized.has_value()){
                //change child to constant
                double value = optimized.value();
                if(node.getType() == ASTNode::UnaryMinus){
                    value *= -1;
                }
                unaryExpr->child = std::make_unique<ASTLiteralNode>(value);
            }
            return optimized;
        } else if(node.getType() == ASTNode::PlusOperator){
            auto expr = static_cast<ASTOperationExpressionNode*>(&node);
            auto optimizedLeft = optimizeStatement(*expr->getLeftChild());
            auto optimizedRight = optimizeStatement(*expr->getRightChild());
            if(optimizedLeft.has_value() && optimizedRight.has_value()){
                //change child to constant
                return optimizedLeft.value() + optimizedRight.value();
            } else if(optimizedRight.has_value()){
                //change child to constant
                double value = optimizedRight.value();
                expr->rightChild = std::make_unique<ASTLiteralNode>(value);
            } else if(optimizedLeft.has_value()){
                //change child to constant
                double value = optimizedLeft.value();
                expr->leftChild = std::make_unique<ASTLiteralNode>(value);
            }
            return std::optional<double>();
        } else if(node.getType() == ASTNode::MinusOperator){
            auto expr = static_cast<ASTOperationExpressionNode*>(&node);
            auto optimizedLeft = optimizeStatement(*expr->getLeftChild());
            auto optimizedRight = optimizeStatement(*expr->getRightChild());
            if(optimizedLeft.has_value() && optimizedRight.has_value()){
                //change child to constant
                return optimizedLeft.value() - optimizedRight.value();
            } else if(optimizedRight.has_value()){
                //change child to constant
                double value = optimizedRight.value();
                expr->rightChild = std::make_unique<ASTLiteralNode>(value);
            } else if(optimizedLeft.has_value()){
                //change child to constant
                double value = optimizedLeft.value();
                expr->leftChild = std::make_unique<ASTLiteralNode>(value);
            }
            return std::optional<double>();
        } else if(node.getType() == ASTNode::MulOperator){
            auto expr = static_cast<ASTOperationExpressionNode*>(&node);
            auto optimizedLeft = optimizeStatement(*expr->getLeftChild());
            auto optimizedRight = optimizeStatement(*expr->getRightChild());
            if(optimizedLeft.has_value() && optimizedRight.has_value()){
                //change child to constant
                return optimizedLeft.value() * optimizedRight.value();
            } else if(optimizedRight.has_value()){
                //change child to constant
                double value = optimizedRight.value();
                expr->rightChild = std::make_unique<ASTLiteralNode>(value);
            } else if(optimizedLeft.has_value()){
                //change child to constant
                double value = optimizedLeft.value();
                expr->leftChild = std::make_unique<ASTLiteralNode>(value);
            }
            return std::optional<double>();
        } else if(node.getType() == ASTNode::DivOperator ){
            auto expr = static_cast<ASTOperationExpressionNode*>(&node);
            auto optimizedLeft = optimizeStatement(*expr->getLeftChild());
            auto optimizedRight = optimizeStatement(*expr->getRightChild());
            if(optimizedLeft.has_value() && optimizedRight.has_value()){
                //change child to constant
                return optimizedLeft.value() / optimizedRight.value();
            } else if(optimizedRight.has_value()){
                //change child to constant
                double value = optimizedRight.value();
                expr->rightChild = std::make_unique<ASTLiteralNode>(value);
            } else if(optimizedLeft.has_value()){
                //change child to constant
                double value = optimizedLeft.value();
                expr->leftChild = std::make_unique<ASTLiteralNode>(value);
            }
            return std::optional<double>();
        } else if(node.getType() == ASTNode::Constant){
            auto constantExpr = static_cast<ASTIdentifierNode*>(&node);
            return variables.find(constantExpr->getValue())->second;
        } else if(node.getType() == ASTNode::LiteralConstant){
            auto literalExpr = static_cast<ASTLiteralNode*>(&node);
            return literalExpr->getValue();
        }
        return std::optional<double>();
    }
} // namespace semantic
