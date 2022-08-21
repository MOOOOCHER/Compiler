#include "ASTTree.h"
#include <utility>
namespace semantic{
ASTTree::ASTTree(std::unique_ptr<ASTFunctionNode> root, ASTSymbolTable table): root(std::move(root)), table(std::move(table)){}
} // namespace semantic