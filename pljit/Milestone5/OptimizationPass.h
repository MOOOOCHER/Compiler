#ifndef PLJIT_OPTIMIZATIONPASS_H
#define PLJIT_OPTIMIZATIONPASS_H
#include "../Milestone4/ASTNode.h"
#include "ASTEvaluator.h"
namespace semantic{
    class AbstractOptimizationPass {
        protected:
        virtual void optimize(ASTNode& node) = 0;
    };
    class DeadCodeEliminationPass: public AbstractOptimizationPass {
        friend class ASTEvaluator;
        void optimize(ASTNode& node) override;
    };
    class ConstantPropagationPass: public AbstractOptimizationPass {
        friend class ASTEvaluator;
        std::map<std::string, double> variables;
        void optimize(ASTNode& node) override;
        /*
         * helper function for constant propagation, returns whether the expression/statement is a constant
         */
        std::optional<double> optimizeStatement(ASTNode& node);
        std::optional<double> optimizeExpression(ASTNode& node);
    };
} // namespace semantic
#endif //PLJIT_OPTIMIZATIONPASS_H
