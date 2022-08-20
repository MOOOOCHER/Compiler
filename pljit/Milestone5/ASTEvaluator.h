#ifndef PLJIT_ASTEVALUATOR_H
#define PLJIT_ASTEVALUATOR_H
#include "../Milestone4/ASTTree.h"
#include "../Milestone4/ASTSymbolTable.h"
#include <unordered_map>
#include <optional>
namespace semantic{
    class ASTEvaluator {
        friend class ASTIdentifierNode;
        friend class ASTLiteralNode;
        friend class ASTUnaryExpression;
        friend class ASTAssignmentExpression;
        friend class ASTFunctionNode;
        friend class ASTStatementNode;
        friend class ASTOperationExpressionNode;
        ASTSymbolTable& table;
        /*
         * this function initializes the parameters with the arguments;
         */
        public:
        explicit ASTEvaluator(ASTSymbolTable& table): table(table){}
        std::optional<double> evaluateFunction(std::vector<double> arg,ASTTree& node);
    };
} // namespace semantic

#endif //PLJIT_ASTEVALUATOR_H
