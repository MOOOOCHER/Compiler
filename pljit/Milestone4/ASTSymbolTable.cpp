#include "ASTSymbolTable.h"

namespace semantic {

    void ASTSymbolTable::insert(const ASTSymbolEntry& entry){
        table.insert(std::make_pair(entry.sourceCodeReference.getText(),entry));
    }
    void ASTSymbolTable::insert(ASTNode::ASTNodeType identifierType, const SourceCodeReference& sourceCodeReference, std::optional<double> value){
            insert(ASTSymbolEntry(identifierType,sourceCodeReference,value));
    }

    bool ASTSymbolTable::contains(std::string_view identifier){
        return table.contains(identifier);
    }
    ASTSymbolTable::ASTSymbolEntry& ASTSymbolTable::get(std::string_view identifier){
        return table.find(identifier)->second;
    }
    void ASTSymbolTable::printVariableWasDeclaredHereErrorMessage(std::string_view identifier){
        if(table.contains(identifier)){
            auto entry = get(identifier);
            entry.sourceCodeReference.printContext("note: '"+std::string(identifier)+ "' was declared here.");
        }
    }
    bool ASTSymbolTable::initArguments(std::vector<double> arg){
        auto printErrorParameter = [](){
            sourceCodeManagement::SourceCodeManager defaultManager = sourceCodeManagement::SourceCodeManager();
            SourceCodeReference a = SourceCodeReference (defaultManager);
            a.printContext("error: parameter size doesn't match with argument size!");
            return false;
        };
        size_t count = 0;
        for(auto& entry: table){
            if(entry.second.identifierType == ASTNode::Parameter){
                if(count<arg.size()){
                    entry.second.value = arg[arg.size()-1-count];   //the parameters are stored in the reverse order of their declaration
                    ++count;
                } else {
                    return printErrorParameter();
                }
            }
        }

        if(count != arg.size()){
            return printErrorParameter();
        }
        return true;
    }
} // namespace semantic