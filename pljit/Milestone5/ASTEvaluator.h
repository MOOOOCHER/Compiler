#ifndef PLJIT_ASTEVALUATOR_H
#define PLJIT_ASTEVALUATOR_H
#include "pljit/ASTDataStructures/ASTTree.h"
#include <optional>
#include <unordered_map>
namespace semantic{
    class ASTEvaluator {
        friend class ASTIdentifierNode;
        friend class ASTLiteralNode;
        friend class ASTUnaryExpression;
        friend class ASTAssignmentExpression;
        friend class ASTFunctionNode;
        friend class ASTStatementNode;
        friend class ASTOperationExpressionNode;
        /*
         * each evaluation builds its own table: faster in a multi-thread environment, but needs more memory space
         */
        std::unordered_map<std::string_view, std::optional<double>> variables;
        /*
         * this function initializes the parameters with the arguments;
         */
        bool initArguments(const std::vector<double>& arg,ASTTree& node);
        public:
        std::optional<double> evaluateFunction(const std::vector<double>& arg,ASTTree& node);
    };
} // namespace semantic

#endif //PLJIT_ASTEVALUATOR_H
