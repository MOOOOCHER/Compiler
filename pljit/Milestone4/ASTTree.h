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
    ASTTree(std::unique_ptr<ASTFunctionNode> root, ASTSymbolTable table): root(std::move(root)), table(table){}
    std::unique_ptr<ASTFunctionNode> root;
    ASTSymbolTable& getTable(){ return table;}
    private:
    ASTSymbolTable table;
};
}


#endif //PLJIT_ASTTREE_H
