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

        public:
        void optimize(ASTNode& node) override;
    };
    class ConstantPropagationPass: public AbstractOptimizationPass {
        friend class ASTEvaluator;
        std::unordered_map<std::string, double> variables;
        /*
         * helper function for constant propagation, returns whether the expression/statement is a constant
         */
        void optimizeStatement(ASTNode& node);
        std::optional<double> optimizeExpression(ASTNode& node);

        public:
        void optimize(ASTNode& node) override;
    };
} // namespace semantic
#endif //PLJIT_OPTIMIZATIONPASS_H
