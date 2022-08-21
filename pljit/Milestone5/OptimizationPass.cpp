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
        for(auto& [name,entry]: node.table.table){
            //get constant values and add them into the map
            if(entry.identifierType== ASTNode::Constant){
                variables.insert(std::pair<std::string_view,double>(name,entry.value.value()));
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
        auto nodeType = node.getType();
        if(nodeType == ASTNode::Constant){
            auto& constantExpr = static_cast<ASTIdentifierNode&>(node);
            return variables.find(constantExpr.getValue())->second;
        } else if(nodeType == ASTNode::LiteralConstant){
            auto& literalExpr = static_cast<ASTLiteralNode&>(node);
            return literalExpr.getValue();
        }else if(nodeType == ASTNode::Variable){
            auto& constantExpr = static_cast<ASTIdentifierNode&>(node);
            auto variableName = constantExpr.getValue();
            if(variables.contains(variableName)){
                return variables[variableName];
            }
        } else if(nodeType == ASTNode::AssignmentExpression){
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
        } else if(nodeType == ASTNode::BracketExpression){
            auto& bracketExpr = static_cast<ASTBracketExpression&>(node);
            return optimizeExpression(*bracketExpr.child);
        } else if(nodeType == ASTNode::UnaryPlus || node.getType() == ASTNode::UnaryMinus){
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
        } else if(nodeType == ASTNode::PlusOperator || nodeType == ASTNode::MinusOperator
                   || nodeType == ASTNode::MulOperator || nodeType == ASTNode::DivOperator){
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
       /*
        * this lamdba function converts to the left-to-right association , given the type of operation
        * we have to group addition and subtraction / multiplication and division together
        */
       auto optimizeAssociation = [&node, this](ASTNode::ASTNodeType op1, ASTNode::ASTNodeType op2){
           auto* expr = static_cast<ASTOperationExpressionNode*>(node.get());
           std::unique_ptr<ASTOperationExpressionNode> currentBuild;
           bool changed = expr->rightChild->getType() == op1 || expr->rightChild->getType()  == op2;
           auto currentTraverse = expr;
           // if we have e.g. addition and subtraction, we have to convert the tree until we have a division or multiplication operation
           while(currentTraverse->rightChild->getType() == op1 || currentTraverse->rightChild->getType()  == op2){
               auto& rightChild = static_cast<ASTOperationExpressionNode&>(*currentTraverse->rightChild);
               if(currentTraverse == expr){
                   optimizeExpression(currentTraverse->leftChild);
                   optimizeExpression(rightChild.leftChild);
                   //first initialize
                   currentBuild = std::make_unique<ASTOperationExpressionNode>(currentTraverse->getType(),std::move(currentTraverse->leftChild),std::move(rightChild.leftChild));
               } else {
                   optimizeExpression(rightChild.leftChild);
                   currentBuild = std::make_unique<ASTOperationExpressionNode>(currentTraverse->getType(),std::move(currentBuild),std::move(rightChild.leftChild));
               }
               //if we break before the next iteration we need to add the furthest right child of the original tree to the new tree
               if(rightChild.rightChild->getType() != op1 && rightChild.rightChild->getType()  != op2){
                    //add the furthest right child to the tree
                    optimizeExpression(rightChild.rightChild);
                    currentBuild = std::make_unique<ASTOperationExpressionNode>(rightChild.getType(),std::move(currentBuild),std::move(rightChild.rightChild));
                    break;
                }
               currentTraverse = static_cast<ASTOperationExpressionNode*>(currentTraverse->rightChild.get());

           }
           if(changed){
               //if we changed the tree structure => update
               node = std::move(currentBuild);
           }
           return;
       };
        auto nodeType = node->getType();
        if(nodeType == ASTNode::AssignmentExpression){
            auto& assignmentExpr = static_cast<ASTAssignmentExpression&>(*node);
            optimizeExpression(assignmentExpr.rightChild);
        } else if(nodeType == ASTNode::PlusOperator || nodeType == ASTNode::MinusOperator){
            optimizeAssociation(ASTNode::PlusOperator,ASTNode::MinusOperator);
        } else if (nodeType == ASTNode::MulOperator || nodeType == ASTNode::DivOperator){
            optimizeAssociation(ASTNode::MulOperator,ASTNode::DivOperator);
        }else if(nodeType == ASTNode::UnaryPlus || nodeType == ASTNode::UnaryMinus){
            auto& unaryExpr = static_cast<ASTUnaryExpression&>(*node);
            optimizeExpression(unaryExpr.child);
        }  else if(nodeType == ASTNode::BracketExpression){
            auto& bracketExpr = static_cast<ASTBracketExpression&>(*node);
            optimizeExpression(bracketExpr.child);
        }
    }
} // namespace semantic
