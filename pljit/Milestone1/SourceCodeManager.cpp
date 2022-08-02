#include "SourceCodeManager.h"
#include <iostream>
namespace sourceCodeManagement{
    SourceCodeManager::SourceCodeManager(std::string_view source): source(source) {}
    //SourceCodeReference-----------------------------------------------------------------------------------------------------------------------------------------
    SourceCodeReference::SourceCodeReference(SourceCodeManager& manager): manager(manager){
        if(!manager.source.empty()){
            positionInCode = manager.source.size()-1;
        }
    }

    SourceCodeReference::SourceCodeReference(const SourceCodeReference& other) = default;
    SourceCodeReference& SourceCodeReference::operator=(const SourceCodeReference& other){
        if(&other!= this){
            positionInCode = other.positionInCode;
            manager = other.manager;
            lengthOfString = other.lengthOfString;
        }
        return *this;
    }

    SourceCodeReference::SourceCodeReference(SourceCodeReference&& other)noexcept: positionInCode(other.positionInCode),manager(other.manager), lengthOfString(other.lengthOfString){
        other.positionInCode = 0;
        other.lengthOfString = 0;
    }
    SourceCodeReference& SourceCodeReference::operator=(SourceCodeReference&& other) noexcept {
        if(&other!= this){
            positionInCode = other.positionInCode;
            manager = other.manager;
            lengthOfString = other.lengthOfString;
        }
        other.positionInCode = 0;
        other.lengthOfString = 0;
        return *this;
    }
    /*
     * this function returns the corresponding line and position within the line for a given pointer to the source code
     */
    std::pair<size_t ,size_t> SourceCodeReference::resolveLocation() const{
        size_t line = 1;
        size_t linePos = 1;
        for(const char& c: manager.source){
            if(&c == getText().data()){
                return {line, linePos};
            }
            if(c=='\n'){
                ++line;
                linePos = 1;
            } else {
                ++linePos;
            }
        }
        return {};
    }
    void SourceCodeReference::printContext(std::string_view errorMsg) const {
        if(manager.source.empty()) {
            //in case of messages without a location
            std::cout << 0<<":"<<0<<": " << errorMsg<<std::endl;
            return;
        }
        std::pair<size_t,size_t> position = resolveLocation();
        //getContext
        std::cout << position.first<<":"<<position.second<<": " << errorMsg<<std::endl;
        std::cout << "\t";
        size_t line = 1;
        for(const char& c: manager.source){
            if(line == position.first && c!='\n'){
                std::cout<< c;
            }
            if(c=='\n'){
                ++line;
            } else if (line>position.first){
                break;
            }
        }
        size_t pos = 1;
        std::cout<< std::endl;
        std::cout << "\t";
        while(pos<position.second){
            std::cout<< " ";
            ++pos;
        }

        std::cout << "^" ;
        size_t len = 2;
        while(len<=lengthOfString){
            std::cout<<"~";
            len++;
        }
        std::cout << std::endl;
    }
    std::string_view SourceCodeReference::getText() const {
        return manager.source.substr(positionInCode,lengthOfString);
    }
    size_t SourceCodeReference::getPositionInCode() const{
        return positionInCode;
    }
    SourceCodeManager& SourceCodeReference::getManager() const{
        return manager;
    }
    size_t SourceCodeReference::getLengthOfString() const{
        return lengthOfString;
    }
} // namespace sourceCodeManagement
