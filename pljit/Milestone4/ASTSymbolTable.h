#ifndef PLJIT_ASTSYMBOLTABLE_H
#define PLJIT_ASTSYMBOLTABLE_H
#include "../Milestone1/SourceCodeManager.h"
#include "pljit/ASTDataStructures/ASTNode.h"
#include <unordered_map>
#include <utility>
namespace semantic {
using SourceCodeReference = sourceCodeManagement::SourceCodeReference;
class SemanticAnalyzer;
class ConstantPropagationPass;
class ASTEvaluator;
class ASTSymbolTable {
    friend class SemanticAnalyzer;
    friend class ConstantPropagationPass;
    friend class ASTEvaluator;
    class ASTSymbolEntry{
        friend class SemanticAnalyzer;
        friend class ASTSymbolTable;
        friend class ConstantPropagationPass;
        friend class ASTEvaluator;
        ASTNode::ASTNodeType identifierType;    //only using parameter,constant or variable
        SourceCodeReference sourceCodeReference;
        std::optional<double> value;    //used for storing constant value that are needed for later

        ASTSymbolEntry(ASTNode::ASTNodeType identifierType, SourceCodeReference  sourceCodeReference, std::optional<double> value);
    };
    ASTSymbolTable() = default;
    std::unordered_map<std::string_view, ASTSymbolEntry> table;
    /*
     * inserts an identifier into the hashtable with a symbol entry
     */
    void insert(const ASTSymbolEntry& node);
    public:
    /*
     * inserts an identifier into the hashtable
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
};
} // namespace semantic

#endif //PLJIT_ASTSYMBOLTABLE_H
