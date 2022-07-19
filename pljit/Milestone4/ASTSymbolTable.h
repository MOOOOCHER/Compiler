#ifndef PLJIT_ASTSYMBOLTABLE_H
#define PLJIT_ASTSYMBOLTABLE_H
#include "../Milestone1/SourceCodeManager.h"
#include "ASTNode.h"
#include <memory>
#include <utility>
#include <vector>
namespace semantic {
using SourceCodeReference = sourceCodeManagement::SourceCodeReference;
class SemanticAnalyzer;
class ASTSymbolTable {
    friend class SemanticAnalyzer;
    class ASTSymbolEntry{
        friend class SemanticAnalyzer;
        friend class ASTSymbolTable;
        ASTNode::ASTNodeType identifierType;    //only using parameter,constant or variable
        std::string name;
        SourceCodeReference sourceCodeReference;

        ASTSymbolEntry(ASTNode::ASTNodeType identifierType, SourceCodeReference sourceCodeReference): identifierType(identifierType),sourceCodeReference(sourceCodeReference){
            name = sourceCodeReference.getText();
        }
    };
    class ASTSymbolWrappedEntry{
        friend class SemanticAnalyzer;
        friend class ASTSymbolTable;
        ASTSymbolEntry entry;
        std::unique_ptr<ASTSymbolWrappedEntry> next;

        public:
        ASTSymbolWrappedEntry(ASTSymbolEntry entry, std::unique_ptr<ASTSymbolWrappedEntry> next): entry(std::move(entry)), next(std::move(next)){}
    };
    std::vector<std::unique_ptr<ASTSymbolWrappedEntry>> buckets;
    ASTSymbolTable();
    ~ASTSymbolTable();
    /*
     * this function computes the Hashfunction  TODO: change hashfunction, so that we can rehash
     */
    size_t computeHash(std::string_view node);
    /*
     * inserts a node into the hashtable
     */
    void insert(ASTSymbolEntry node);
    /*
     * inserts a node into the hashtable
     */
    void insert(ASTNode::ASTNodeType identifierType, SourceCodeReference sourceCodeReference);
    /*
     * checks whether the hastable contains a node
     */
    bool contains(std::string_view identifier);
    /*
     * gets an entry with the identifier name, returns emptry string if there is none
     */
    ASTSymbolEntry* get(std::string_view identifier);
};
} // namespace semantic

#endif //PLJIT_ASTSYMBOLTABLE_H
