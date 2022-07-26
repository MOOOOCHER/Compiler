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

        std::map<std::string, std::optional<double>> variables;
        /*
         * these functions are needed for setting up variable storing
         */
        std::optional<double> evaluate( semantic::ASTFunctionNode& node);
        std::optional<double> evaluate(const semantic::ASTDeclaratorListNode& node);
        std::optional<double> evaluate(const semantic::ASTInitDeclaratorNode& node);
        std::optional<double> evaluate(const semantic::ASTInitDeclaratorListNode& node);
        /*
         * these functions are needed for the actual computation
         */
        std::optional<double> evaluate( semantic::ASTIdentifierNode& node);
        std::optional<double> evaluate( semantic::ASTLiteralNode& node);
        std::optional<double> evaluate( semantic::ASTOperationExpressionNode& node);
        std::optional<double> evaluate( semantic::ASTStatementNode& node);
        std::optional<double> evaluate( semantic::ASTAssignmentExpression& node) ;
        std::optional<double> evaluate( semantic::ASTCompoundStatement& node) ;
        std::optional<double> evaluate( semantic::ASTUnaryExpression& node);

        public:
        std::optional<double> evaluateFunction(semantic::ASTNode& node);
        std::optional<double> evaluate( semantic::ASTParamIdentifierNode& node);
    };
} // namespace semantic

#endif //PLJIT_ASTEVALUATOR_H
