#ifndef PLJIT_ASTEVALUATOR_H
#define PLJIT_ASTEVALUATOR_H
#include "../Milestone4/ASTNode.h"
#include <map>
#include <optional>
namespace semantic{
    class ASTEvaluator {
        friend class ASTIdentifierNode;
        friend class ASTCompoundStatement;
        friend class ASTLiteralNode;
        friend class ASTUnaryExpression;
        friend class ASTAssignmentExpression;
        friend class ASTInitDeclaratorListNode;
        friend class ASTInitDeclaratorNode;
        friend class ASTFunctionNode;
        friend class ASTDeclaratorListNode;
        friend class ASTStatementNode;
        friend class ASTOperationExpressionNode;

        std::map<std::string, std::optional<long>> variables;
        /*
         * these functions are needed for setting up variable storing
         */
        void evaluate(const semantic::ASTDeclaratorListNode& node);
        long evaluate(const semantic::ASTFunctionNode& node);
        void evaluate(const semantic::ASTInitDeclaratorNode& node);
        void evaluate(const semantic::ASTInitDeclaratorListNode& node);
        /*
         * these functions are needed for the actual computation
         */
        long evaluate(const semantic::ASTIdentifierNode& node);
        long evaluate(const semantic::ASTLiteralNode& node);
        long evaluate(const semantic::ASTOperationExpressionNode& node);
        long evaluate(const semantic::ASTStatementNode& node);
        long evaluate(const semantic::ASTAssignmentExpression& node) ;
        long evaluate(const semantic::ASTCompoundStatement& node) ;
        long evaluate(const semantic::ASTUnaryExpression& node);
    };
} // namespace semantic

#endif //PLJIT_ASTEVALUATOR_H
