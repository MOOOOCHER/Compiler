#ifndef FINAL_SOURCECODEMANAGER_H
#define FINAL_SOURCECODEMANAGER_H
#include <cstddef>
#include <string_view>
namespace sourceCodeManagement{
class SourceCodeReference;

struct SourceCodeManager{
    const std::string_view source = "";
    explicit SourceCodeManager(std::string_view source);
};
class SourceCodeReference{
    friend struct SourceCodeManager;
    const char* location;
    SourceCodeManager& manager;
    /*
     * this function returns the corresponding line and position within the line for a given pointer to the source code
     */
    std::pair<size_t ,size_t> resolveLocation() const;
    public:
    /*
     * This function takes an error message and the length of the marked string (if >1 then we have a range)
     */
    void printContext(std::string_view errorMsg, size_t lengthOfString) const;
    /*
     * standard construct pointing to the last element of the code
     */
    SourceCodeReference(SourceCodeManager& manager): location(manager.source.data()+manager.source.size()-1), manager(manager){}
    SourceCodeReference(const char* location,SourceCodeManager& manager): location(location), manager(manager){}
};
} // namespace sourceCodeManagement
#endif //FINAL_SOURCECODEMANAGER_H
