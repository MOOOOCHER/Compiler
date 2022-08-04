#include "ASTEvaluator.h"
#include <utility>
namespace semantic{
std::optional<double> ASTEvaluator::evaluateFunction(std::vector<long> arg,semantic::ASTNode& node){
    arguments = std::move(arg);
    return node.acceptEvaluation(*this);
}
} // namespace semantic