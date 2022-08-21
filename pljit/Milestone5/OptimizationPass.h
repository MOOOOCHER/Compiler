#ifndef PLJIT_OPTIMIZATIONPASS_H
#define PLJIT_OPTIMIZATIONPASS_H
#include "../Milestone4/ASTSymbolTable.h"
#include "ASTEvaluator.h"
#include "pljit/ASTDataStructures/ASTNode.h"
namespace semantic{
    class AbstractOptimizationPass {
        protected:
        virtual void optimize(ASTTree& node) = 0;
    };
    class DeadCodeEliminationPass: public AbstractOptimizationPass {
        friend class ASTEvaluator;

        public:
        void optimize(ASTTree& node) override;
    };
    class ConstantPropagationPass: public AbstractOptimizationPass {
        friend class ASTEvaluator;
        std::unordered_map<std::string_view, double> variables;
        /*
         * helper function for constant propagation, returns whether the expression/statement is a constant
         */
        void optimizeStatement(ASTNode& node);
        std::optional<double> optimizeExpression(ASTNode& node);

        public:
        void optimize(ASTTree& node) override;
    };
    /*
     * class for turning from right-to-left association to left-to-right association
     */
    class AssociationPass: public AbstractOptimizationPass {
        friend class ASTEvaluator;
        void optimizeStatement(ASTNode& node);
        void optimizeExpression(std::unique_ptr<ASTNode> &node);
        public:
        void optimize(ASTTree& node) override;
    };
    } // namespace semantic
#endif //PLJIT_OPTIMIZATIONPASS_H
