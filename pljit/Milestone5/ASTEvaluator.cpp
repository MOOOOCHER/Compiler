#include "ASTEvaluator.h"
#include <utility>
namespace semantic{
std::optional<double> ASTEvaluator::evaluateFunction(std::vector<double> arg,ASTTree& node){
    if(!node.table.initArguments(std::move(arg))){
        return {};
    }
    return node.root->acceptEvaluation(*this);
}
} // namespace semantic