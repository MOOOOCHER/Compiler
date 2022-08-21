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
    std::string SourceCodeReference::getLineString(size_t line) const {
        std::string ret = "";
        size_t index = 1;
        for(const char& c: manager.source){
            if(index == line && c!='\n'){
                ret+=c;
            }
            if(c=='\n'){
                ++index;
            } else if (index>line){
                break;
            }
        }
        return ret;
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
        std::cout << "\t| ";
        std::string lineCode = getLineString(position.first);
        std::cout << lineCode <<std::endl;

        std::cout << "\t  ";
        size_t pos = 1;
        while(pos<position.second){
            if(lineCode[pos-1] == '\t'){
                std::cout<< "\t";
            }else{
                std::cout<< " ";
            }
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
