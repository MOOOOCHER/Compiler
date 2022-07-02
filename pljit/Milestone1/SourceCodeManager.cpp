#include "SourceCodeManager.h"
#include <iostream>
namespace sourceCodeManagement{
    SourceCodeManager::SourceCodeManager(std::string_view source): source(source) {}
    /*
     * this function returns the corresponding line and position within the line for a given pointer to the source code
     */
    std::pair<size_t ,size_t> SourceCodeReference::resolveLocation(){
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

    SourceCodeReferenceLocation::SourceCodeReferenceLocation(const char* location, SourceCodeManager& manager): SourceCodeReference(location,manager) {}
    void SourceCodeReferenceLocation::printContext() {
        std::pair<size_t,size_t> position = resolveLocation();
        //getContext
        std::cout << position.first<<":"<<position.second<<": An error has occurred!"<<std::endl;
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
        while(pos<position.second){
            std::cout<< " ";
            ++pos;
        }

        std::cout << "^" << std::endl;
    }

    SourceCodeReferenceRange::SourceCodeReferenceRange(const char* location, size_t length, SourceCodeManager& manager): SourceCodeReference(location, manager),length(length) {}
    void SourceCodeReferenceRange::printContext() {
        std::pair<size_t,size_t> position = resolveLocation();
        //getContext
        size_t line = 0;
        for(const char& c: manager.source){
            if(line == position.first && c!='\n'){
                std::cout<< c;
            }
            if(c=='\n'){
                ++line;
            } else if (line>=position.first){
                break;
            }
        }
        size_t pos = 0;
        std::cout<< std::endl;
        while(pos<position.second){
            std::cout<< " ";
            ++pos;
        }

        std::cout << "^" << std::endl;
        size_t len = 1;
        while(len<=length){
            std::cout<<"~";
            len++;
        }
        std::cout << std::endl;
    }
} // namespace sourceCodeManagement
