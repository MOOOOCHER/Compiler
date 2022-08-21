#ifndef PLJIT_ASTEVALUATOR_H
#define PLJIT_ASTEVALUATOR_H
#include "../Milestone4/ASTSymbolTable.h"
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
        std::unordered_map<std::string_view, std::optional<double>> variables;
        /*
         * this function initializes the parameters with the arguments;
         */
        public:
        std::optional<double> evaluateFunction(std::vector<double> arg,ASTTree& node);
    };
} // namespace semantic

#endif //PLJIT_ASTEVALUATOR_H
