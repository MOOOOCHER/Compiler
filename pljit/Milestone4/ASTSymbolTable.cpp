#include "ASTSymbolTable.h"

namespace semantic {

    void ASTSymbolTable::insert(const ASTSymbolEntry& entry){
        table.insert(std::make_pair(entry.sourceCodeReference.getText(),entry));
    }
    void ASTSymbolTable::insert(ASTNode::ASTNodeType identifierType, const SourceCodeReference& sourceCodeReference){
        insert(ASTSymbolEntry(identifierType,sourceCodeReference));
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
} // namespace semantic