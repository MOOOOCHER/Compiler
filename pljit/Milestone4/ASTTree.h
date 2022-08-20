#ifndef PLJIT_ASTTREE_H
#define PLJIT_ASTTREE_H
#include "ASTNode.h"
#include "ASTSymbolTable.h"
#include <memory>
namespace semantic{
//wrapper class
class ASTEvaluator;
class ConstantPropagationPass;
class DeadCodeEliminationPass;
class ASTTree {
    friend class ASTEvaluator;
    friend class ConstantPropagationPass;
    friend class DeadCodeEliminationPass;
    public:
    ASTTree(std::unique_ptr<ASTFunctionNode> root, ASTSymbolTable table);
    std::unique_ptr<ASTFunctionNode> root;
    private:
    ASTSymbolTable table;
};
} // namespace semantic


#endif //PLJIT_ASTTREE_H
