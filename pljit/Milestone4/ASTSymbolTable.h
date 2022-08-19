#ifndef PLJIT_ASTSYMBOLTABLE_H
#define PLJIT_ASTSYMBOLTABLE_H
#include "../Milestone1/SourceCodeManager.h"
#include "ASTNode.h"
#include <utility>
#include <unordered_map>
namespace semantic {
using SourceCodeReference = sourceCodeManagement::SourceCodeReference;
class SemanticAnalyzer;
class ASTSymbolTable {
    friend class SemanticAnalyzer;
    class ASTSymbolEntry{
        friend class SemanticAnalyzer;
        friend class ASTSymbolTable;
        friend class ASTAssignmentExpression;
        friend class ASTIdentifierNode;
        ASTNode::ASTNodeType identifierType;    //only using parameter,constant or variable
        SourceCodeReference sourceCodeReference;
        std::optional<double> value;

        ASTSymbolEntry(ASTNode::ASTNodeType identifierType, SourceCodeReference  sourceCodeReference, std::optional<double> value): identifierType(identifierType),sourceCodeReference(std::move(sourceCodeReference)), value(value){}
        public:
        std::optional<double> getValue()const{ return value;}
        ASTNode::ASTNodeType getIdentifierType() const{return identifierType;}
    };
    ASTSymbolTable() = default;
    std::unordered_map<std::string_view, ASTSymbolEntry> table;
    /*
     * inserts a node into the hashtable
     */
    void insert(const ASTSymbolEntry& node);
    public:
    /*
     * inserts a node into the hashtable
     */
    void insert(ASTNode::ASTNodeType type,  const SourceCodeReference& sourceCodeReference, std::optional<double> value = std::optional<double>());
    /*
     * checks whether the hashtable contains a node
     */
    bool contains(std::string_view identifier);
    /*
     * gets an entry with the identifier name, returns empty string if there is none
     */
    ASTSymbolEntry& get(std::string_view identifier);
    /*
     * prints an error message for the declared identifier
     */
    void printVariableWasDeclaredHereErrorMessage(std::string_view identifier);
    /*
     * initializes parameter identifiers in the table
     */
    bool initArguments(std::vector<double> arg);
    auto& getTable() const{
        return table;
    }
};
} // namespace semantic

#endif //PLJIT_ASTSYMBOLTABLE_H
