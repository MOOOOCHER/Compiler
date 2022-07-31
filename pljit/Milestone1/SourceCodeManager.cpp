#include "SourceCodeManager.h"
#include <iostream>
namespace sourceCodeManagement{
    SourceCodeManager::SourceCodeManager(std::string_view source): source(source) {}
    //SourceCodeReference-----------------------------------------------------------------------------------------------------------------------------------------
    SourceCodeReference::SourceCodeReference(SourceCodeManager& manager): location(nullptr), manager(manager){}

    SourceCodeReference::SourceCodeReference(const SourceCodeReference& other) = default;
    SourceCodeReference& SourceCodeReference::operator=(const SourceCodeReference& other){
        if(&other!= this){
            location = other.location;
            positionInCode = other.positionInCode;
            manager = other.manager;
            lengthOfString = other.lengthOfString;
        }
        return *this;
    }

    SourceCodeReference::SourceCodeReference(SourceCodeReference&& other)noexcept: location(other.location), positionInCode(other.positionInCode),manager(other.manager), lengthOfString(other.lengthOfString){
        other.location = nullptr;
        other.positionInCode = 0;
        other.lengthOfString = 0;
    }
    SourceCodeReference& SourceCodeReference::operator=(SourceCodeReference&& other) noexcept {
        location = nullptr;
        if(&other!= this){
            location = other.location;
            positionInCode = other.positionInCode;
            manager = other.manager;
            lengthOfString = other.lengthOfString;
        }
        other.location = nullptr;
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
            if(&c == location){
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
} // namespace sourceCodeManagement
