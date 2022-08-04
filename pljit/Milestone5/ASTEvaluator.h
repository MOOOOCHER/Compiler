#ifndef PLJIT_ASTEVALUATOR_H
#define PLJIT_ASTEVALUATOR_H
#include "../Milestone4/ASTNode.h"
#include <unordered_map>
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
        friend class ASTParamDeclaratorListNode;
        friend class ASTVarDeclaratorListNode;
        friend class ASTStatementNode;
        friend class ASTOperationExpressionNode;
        std::unordered_map<std::string_view, std::optional<double>> variables;
        std::vector<long> arguments;
        /*
         * this function initializes the parameters with the arguments;
         */
        public:
        std::optional<double> evaluateFunction(std::vector<long> arg,semantic::ASTNode& node);
    };
} // namespace semantic

#endif //PLJIT_ASTEVALUATOR_H
