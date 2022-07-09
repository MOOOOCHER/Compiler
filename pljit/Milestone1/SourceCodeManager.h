#ifndef FINAL_SOURCECODEMANAGER_H
#define FINAL_SOURCECODEMANAGER_H
#include <cstddef>
#include <string_view>
namespace sourceCodeManagement{
class SourceCodeReference;

struct SourceCodeManager{
    std::string_view source = "";
    explicit SourceCodeManager(std::string_view source);
    SourceCodeManager(const SourceCodeManager& other) = default;
    SourceCodeManager& operator=(const SourceCodeManager& other)= default;
};
class SourceCodeReference{
    friend struct SourceCodeManager;
    const char* location;
    SourceCodeManager& manager;
    size_t lengthOfString = 0;
    /*
     * this function returns the corresponding line and position within the line for a given pointer to the source code
     */
    std::pair<size_t ,size_t> resolveLocation() const;
    public:
    /*
     * This function takes an error message and the length of the marked string (if >1 then we have a range)
     */
    void printContext(std::string_view errorMsg) const;
    std::string getText();
    /*
     * standard construct pointing to the last element of the code
     */
    SourceCodeReference(SourceCodeManager& manager): location(manager.source.data()+manager.source.size()-1), manager(manager){}
    SourceCodeReference(const char* location,SourceCodeManager& manager, size_t lengthOfString): location(location), manager(manager), lengthOfString(lengthOfString){}

    SourceCodeReference(const SourceCodeReference& other) = default;
    SourceCodeReference& operator=(const SourceCodeReference& other){
        if(&other!= this){
            location = other.location;
            manager = other.manager;
            lengthOfString = other.lengthOfString;
        }
        return *this;
    }

};
} // namespace sourceCodeManagement
#endif //FINAL_SOURCECODEMANAGER_H
