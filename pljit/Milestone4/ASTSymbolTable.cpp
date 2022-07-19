#include "ASTSymbolTable.h"

namespace semantic {
    //helper function for hashing
    //One bucket for every Letter
    ASTSymbolTable::ASTSymbolTable(){
        for(size_t i=0;i<27;i++){
            buckets.push_back(nullptr);
        }
    }
    ASTSymbolTable::~ASTSymbolTable(){
        for(auto& bucket: buckets){
            std::unique_ptr<ASTSymbolWrappedEntry> entry = std::move(bucket);
            while(entry!=nullptr){
                entry = std::move(entry->next);
            }
        }
    }

    size_t ASTSymbolTable::computeHash(std::string_view node){
        size_t result = 0;
        size_t factor = 1;
        for(auto a: node){
            result += a* factor;
            factor *= 5;
        }
        return result% buckets.size();
    }

    void ASTSymbolTable::insert(ASTSymbolEntry entry){
        size_t index = computeHash(entry.name);
        ASTSymbolWrappedEntry* indexBucket = buckets[index].get();
        while(indexBucket != nullptr){
            indexBucket = indexBucket->next.get();
        }
        buckets[index] = std::make_unique<ASTSymbolWrappedEntry>(entry, std::move(buckets[index]));
    }
    void ASTSymbolTable::insert(ASTNode::ASTNodeType identifierType, SourceCodeReference sourceCodeReference){
        insert(ASTSymbolEntry(identifierType,sourceCodeReference));
    }

    bool ASTSymbolTable::contains(std::string_view identifier){
        size_t index = computeHash(identifier);
        ASTSymbolWrappedEntry* indexBucket = buckets[index].get();
        while(indexBucket != nullptr){
            if(indexBucket->entry.name== identifier){
                return true;
            }
            indexBucket = indexBucket->next.get();
        }
        return false;
    }
    ASTSymbolTable::ASTSymbolEntry* ASTSymbolTable::get(std::string_view identifier){
        size_t index = computeHash(identifier);
        ASTSymbolWrappedEntry* indexBucket = buckets[index].get();
        while(indexBucket != nullptr){
            if(indexBucket->entry.name== identifier){
                return &indexBucket->entry;
            }
            indexBucket = indexBucket->next.get();
        }
        return nullptr;
    }
} // namespace semantic