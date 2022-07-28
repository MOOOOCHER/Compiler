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
                //start removing statements from the end until return statement is reached
                if(compoundStatement->getChildren()[compoundStatement->getChildren().size()-1]->getType() == ASTNode::ReturnStatement){
                    return;
                }
                compoundStatement->pop_back_child();
            }
        }
    }
    //ConstantPropagation---------------------------------------------------------------------------------------------------------
    void ConstantPropagationPass::optimize(ASTNode& node) {
        if(node.getType() == ASTNode::FunctionDefinition){
            auto functionDefinition = static_cast<ASTFunctionNode*>(&node);
            for(auto child: functionDefinition->getChildren()){
                switch(child->getType()){
                    case ASTNode::InitDeclaratorList: {
                        optimize(*child);
                        break;
                    }
                    case ASTNode::CompoundStatement:{
                        optimize(*child);
                        return;
                    }
                    default: break;
                }
            }
        } else if(node.getType() == ASTNode::InitDeclaratorList){
            //initialize table with constant values
            auto constInitDeclList = static_cast<ASTInitDeclaratorListNode*>(&node);
            for(auto child: constInitDeclList->getChildren()){
                auto constInitDecl = static_cast<ASTInitDeclaratorNode*>(child);
                //get Values
                auto name = static_cast<ASTIdentifierNode*>(constInitDecl->getLeftChild());
                auto value = static_cast<ASTLiteralNode*>(constInitDecl->getRightChild());
                variables.insert(std::pair<std::string,double>(name->getValue(),value->getValue()));
            }
        } else if(node.getType() == ASTNode::CompoundStatement){
            //optimize each statement
            auto compoundStatement = static_cast<ASTCompoundStatement*>(&node);
            for(auto child: compoundStatement->getChildren()){
                    optimizeStatement(*child);
            }
        }
    }
    void ConstantPropagationPass::optimizeStatement(ASTNode& node){
        auto statement = static_cast<ASTStatementNode*>(&node);
        auto optimized = optimizeExpression(*statement->getChild());
        if(node.getType() == ASTNode::ReturnStatement){
            if(optimized.has_value()){
                //change child to constant
                statement->child = std::make_unique<ASTLiteralNode>(optimized.value());
            }
        }
    }
    std::optional<double> ConstantPropagationPass::optimizeExpression(ASTNode& node){
        if(node.getType() == ASTNode::Constant){
            auto constantExpr = static_cast<ASTIdentifierNode*>(&node);
            return variables.find(constantExpr->getValue())->second;
        } else if(node.getType() == ASTNode::LiteralConstant){
            auto literalExpr = static_cast<ASTLiteralNode*>(&node);
            return literalExpr->getValue();
        }else if(node.getType() == ASTNode::Variable){
            auto constantExpr = static_cast<ASTIdentifierNode*>(&node);
            if(variables.contains(constantExpr->getValue())){
                return variables.find(constantExpr->getValue())->second;
            }
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
        else if(node.getType() == ASTNode::UnaryPlus || node.getType() == ASTNode::UnaryMinus){
            auto unaryExpr = static_cast<ASTUnaryExpression*>(&node);
            auto optimized = optimizeExpression(*unaryExpr->getChild());
            if(optimized.has_value()){
                //change child to constant
                if(node.getType() == ASTNode::UnaryMinus){
                    optimized = std::optional<double>(optimized.value()*-1);
                }
                unaryExpr->child = std::make_unique<ASTLiteralNode>(optimized.value());
            }
            return optimized;
        } else if(node.getType() == ASTNode::PlusOperator || node.getType() == ASTNode::MinusOperator
                   || node.getType() == ASTNode::MulOperator || node.getType() == ASTNode::DivOperator){
            //binary operation
            auto expr = static_cast<ASTOperationExpressionNode*>(&node);
            auto optimizedLeft = optimizeExpression(*expr->getLeftChild());
            auto optimizedRight = optimizeExpression(*expr->getRightChild());
            if(optimizedLeft.has_value() && optimizedRight.has_value()){
                //change child to constant
                if(node.getType() == ASTNode::PlusOperator){
                    return optimizedLeft.value() + optimizedRight.value();
                } else if (node.getType() == ASTNode::MinusOperator){
                    return optimizedLeft.value() - optimizedRight.value();
                } else if (node.getType() == ASTNode::MulOperator){
                    return optimizedLeft.value() * optimizedRight.value();
                } else if (node.getType() == ASTNode::DivOperator){
                    if(optimizedRight.value() != 0){
                        //check for divide by 0
                        return optimizedLeft.value() / optimizedRight.value();
                    }
                }
            } else if(optimizedRight.has_value()){
              //if only right child is constant
                double value = optimizedRight.value();
                expr->rightChild = std::make_unique<ASTLiteralNode>(value);
            } else if(optimizedLeft.has_value()){
                //if only left child is constant
                double value = optimizedLeft.value();
                expr->leftChild = std::make_unique<ASTLiteralNode>(value);
            }
        }
        return {};
    }
} // namespace semantic
