#include "ASTEvaluator.h"
#include <utility>
namespace semantic{
std::optional<double> ASTEvaluator::evaluateFunction(std::vector<double> arg,ASTTree& node){
    auto printErrorParameter = [](){
        sourceCodeManagement::SourceCodeManager defaultManager = sourceCodeManagement::SourceCodeManager();
        SourceCodeReference a = SourceCodeReference (defaultManager);
        a.printContext("error: parameter size doesn't match with argument size!");
        return std::optional<double>();
    };
    //init variables table
    size_t count = 0;
    for(auto& entry: node.table.table){
        if(entry.second.identifierType == ASTNode::Parameter){
            if(count<arg.size()){
                variables.insert(std::make_pair(entry.first,arg[arg.size()-1-count]));//the parameters are stored in the reverse order of their declaration
                ++count;
            } else {
                return printErrorParameter();
            }
        } else if (entry.second.identifierType == ASTNode::Variable){
            variables.insert(std::make_pair(entry.first,std::optional<double>()));//the parameters are stored in the reverse order of their declaration
        }
    }

    if(count != arg.size()){
        return printErrorParameter();
    }

    return node.root->acceptEvaluation(*this);
}
} // namespace semantic