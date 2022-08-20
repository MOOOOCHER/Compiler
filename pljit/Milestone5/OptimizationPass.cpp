#include "OptimizationPass.h"
namespace semantic{
    void DeadCodeEliminationPass::optimize(ASTTree& node) {
        if(node.root->getType() == ASTNode::FunctionDefinition){
            auto& functionDefinition = static_cast<ASTFunctionNode&>(*node.root);
            while(true){
                //start removing statements from the end until return statement is reached
                if(functionDefinition.children[functionDefinition.children.size()-1]->getType() == ASTNode::ReturnStatement){
                    return;
                }
                functionDefinition.pop_back_child();
            }
        }
    }
    //ConstantPropagation---------------------------------------------------------------------------------------------------------
    void ConstantPropagationPass::optimize(ASTTree& node) {
        for(auto& entry: node.table.getTable()){
            //get constant values and add them into the map
            if(entry.second.getIdentifierType() == ASTNode::Constant){
                auto name = entry.first;
                auto value = entry.second.getValue();
                variables.insert(std::pair<std::string_view,double>(name,value.value()));
            }

        }
        if(node.root->getType() == ASTNode::FunctionDefinition){
            auto& functionDefinition = static_cast<ASTFunctionNode&>(*node.root);
            for(auto& child: functionDefinition.children){
                optimizeStatement(*child);
            }
        }
    }
    void ConstantPropagationPass::optimizeStatement(ASTNode& node){
        if(node.getType() == ASTNode::ReturnStatement){
            auto& statement = static_cast<ASTReturnStatementNode&>(node);
            auto optimized = optimizeExpression(*statement.child);
            if(optimized.has_value()){
                //change child to constant
                statement.child = std::make_unique<ASTLiteralNode>(optimized.value());
            }
        } else {
            auto& statement = static_cast<ASTAssignmentStatementNode&>(node);
            optimizeExpression(*statement.child);
        }
    }
    std::optional<double> ConstantPropagationPass::optimizeExpression(ASTNode& node){
        if(node.getType() == ASTNode::Constant){
            auto& constantExpr = static_cast<ASTIdentifierNode&>(node);
            return variables.find(constantExpr.getValue())->second;
        } else if(node.getType() == ASTNode::LiteralConstant){
            auto& literalExpr = static_cast<ASTLiteralNode&>(node);
            return literalExpr.getValue();
        }else if(node.getType() == ASTNode::Variable){
            auto& constantExpr = static_cast<ASTIdentifierNode&>(node);
            auto variableName = constantExpr.getValue();
            if(variables.contains(variableName)){
                return variables[variableName];
            }
        } else if(node.getType() == ASTNode::AssignmentExpression){
            auto& assignmentExpr = static_cast<ASTAssignmentExpression&>(node);
            auto optimized = optimizeExpression(*assignmentExpr.rightChild);
            auto name = static_cast<ASTIdentifierNode*>(assignmentExpr.leftChild.get());
            if(optimized.has_value()){
                //change child to constant
                assignmentExpr.rightChild = std::make_unique<ASTLiteralNode>(optimized.value());
                variables[name->getValue()]  = optimized.value();
            } else {
                //remove if variable isn't constexpr anymore
                auto it = variables.find(name->getValue());
                if(it != std::end(variables)){
                    variables.erase(it);
                }
            }
        }
        else if(node.getType() == ASTNode::UnaryPlus || node.getType() == ASTNode::UnaryMinus){
            auto& unaryExpr = static_cast<ASTUnaryExpression&>(node);
            auto optimized = optimizeExpression(*unaryExpr.child);
            if(optimized.has_value()){
                //change child to constant
                if(node.getType() == ASTNode::UnaryMinus){
                    optimized = std::optional<double>(optimized.value()*(-1));
                }
                unaryExpr.child = std::make_unique<ASTLiteralNode>(optimized.value());
            }
            return optimized;
        } else if(node.getType() == ASTNode::PlusOperator || node.getType() == ASTNode::MinusOperator
                   || node.getType() == ASTNode::MulOperator || node.getType() == ASTNode::DivOperator){
            //binary operation
            auto& expr = static_cast<ASTOperationExpressionNode&>(node);
            auto optimizedLeft = optimizeExpression(*expr.leftChild);
            auto optimizedRight = optimizeExpression(*expr.rightChild);
            if(optimizedLeft.has_value() && optimizedRight.has_value()){
                //change child to constant
                if(node.getType() == ASTNode::PlusOperator){
                    return optimizedLeft.value() + optimizedRight.value();
                } else if (node.getType() == ASTNode::MinusOperator){
                    return optimizedLeft.value() - optimizedRight.value();
                } else if (node.getType() == ASTNode::MulOperator){
                    return optimizedLeft.value() * optimizedRight.value();
                } else if (node.getType() == ASTNode::DivOperator && optimizedRight.value() != 0){
                        //check for divide by 0
                        return optimizedLeft.value() / optimizedRight.value();
                }
            } else if(optimizedRight.has_value()){
              //if only right child is constant
                double value = optimizedRight.value();
                expr.rightChild = std::make_unique<ASTLiteralNode>(value);
            } else if(optimizedLeft.has_value()){
                //if only left child is constant
                double value = optimizedLeft.value();
                expr.leftChild = std::make_unique<ASTLiteralNode>(value);
            }
        }
        return {};
    }

    void AssociationPass::optimize(ASTTree& node) {
        if(node.root->getType() == ASTNode::FunctionDefinition){
            auto& functionDefinition = static_cast<ASTFunctionNode&>(*node.root);
            for(auto& child: functionDefinition.children){
                optimizeStatement(*child);
            }
        }
    }
    void AssociationPass::optimizeStatement(ASTNode& node){
        if(node.getType() == ASTNode::ReturnStatement){
            auto& statement = static_cast<ASTReturnStatementNode&>(node);
            optimizeExpression(statement.child);
        } else {
            auto& statement = static_cast<ASTAssignmentStatementNode&>(node);
            optimizeExpression(statement.child);
        }
    }
   void AssociationPass::optimizeExpression(std::unique_ptr<ASTNode>& node){
        if(node->getType() == ASTNode::AssignmentExpression){
            auto& assignmentExpr = static_cast<ASTAssignmentExpression&>(*node);
            optimizeExpression(assignmentExpr.rightChild);
        } else if(node->getType() == ASTNode::PlusOperator || node->getType() == ASTNode::MinusOperator){
            //binary operation
            auto* expr = static_cast<ASTOperationExpressionNode*>(node.get());
            std::unique_ptr<ASTOperationExpressionNode> currentBuild;

            if(expr->rightChild->getType() == ASTNode::PlusOperator || expr->rightChild->getType()  == ASTNode::MinusOperator){
                auto currentTraverse = expr;
                auto& rightChild = static_cast<ASTOperationExpressionNode&>(*currentTraverse->rightChild);
                optimizeExpression(currentTraverse->leftChild);
                optimizeExpression(rightChild.leftChild);
                //first initialize
                currentBuild = std::make_unique<ASTOperationExpressionNode>(currentTraverse->getType(),std::move(currentTraverse->leftChild),std::move(rightChild.leftChild));
                currentTraverse = static_cast<ASTOperationExpressionNode*>(currentTraverse->rightChild.get());
                while(currentTraverse->rightChild->getType() == ASTNode::PlusOperator || currentTraverse->rightChild->getType()  == ASTNode::MinusOperator){
                    auto& newRightChild = static_cast<ASTOperationExpressionNode&>(*currentTraverse->rightChild);
                    optimizeExpression(newRightChild.leftChild);
                    currentBuild = std::make_unique<ASTOperationExpressionNode>(currentTraverse->getType(),std::move(currentBuild),std::move(newRightChild.leftChild));

                    currentTraverse = static_cast<ASTOperationExpressionNode*>(currentTraverse->rightChild.get());

                }
                /*if(newRightChild.rightChild->getType() != ASTNode::PlusOperator && newRightChild.rightChild->getType()  != ASTNode::MinusOperator){
                    //add the furthest right child to the tree
                    optimizeExpression(newRightChild.leftChild);
                    currentBuild = std::make_unique<ASTOperationExpressionNode>(currentTraverse->getType(),std::move(currentBuild),std::move(currentTraverse->rightChild));
                    break;
                }*/
                node = std::move(currentBuild);
            }
            return;
        } else if (node->getType() == ASTNode::MulOperator || node->getType() == ASTNode::DivOperator){
            auto* expr = static_cast<ASTOperationExpressionNode*>(node.get());
            std::unique_ptr<ASTOperationExpressionNode> currentBuild;
            auto currentTraverse = expr;
            if(currentTraverse->rightChild->getType() == ASTNode::MulOperator || currentTraverse->rightChild->getType()  == ASTNode::DivOperator){
                auto& rightChild = static_cast<ASTOperationExpressionNode&>(*currentTraverse->rightChild);
                optimizeExpression(currentTraverse->leftChild);
                optimizeExpression(rightChild.leftChild);
                //first initialize
                currentBuild = std::make_unique<ASTOperationExpressionNode>(currentTraverse->getType(),std::move(currentTraverse->leftChild),std::move(rightChild.leftChild));
                currentTraverse = static_cast<ASTOperationExpressionNode*>(currentTraverse->rightChild.get());
                while(currentTraverse->rightChild->getType() == ASTNode::MulOperator || currentTraverse->rightChild->getType()  == ASTNode::DivOperator){
                    auto& newRightChild = static_cast<ASTOperationExpressionNode&>(*currentTraverse->rightChild);
                    optimizeExpression(newRightChild.leftChild);
                    currentBuild = std::make_unique<ASTOperationExpressionNode>(currentTraverse->getType(),std::move(currentBuild),std::move(newRightChild.leftChild));
                    currentTraverse = static_cast<ASTOperationExpressionNode*>(currentTraverse->rightChild.get());
                }
                node = std::move(currentBuild);
            }
            return;
        }else if(node->getType() == ASTNode::UnaryPlus || node->getType() == ASTNode::UnaryMinus){
            auto& unaryExpr = static_cast<ASTUnaryExpression&>(*node);
            optimizeExpression(unaryExpr.child);
            return;
        } else {
            return;
        }
    }
} // namespace semantic
