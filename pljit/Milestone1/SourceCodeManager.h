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
    protected:
    const char* location;
    SourceCodeManager& manager;
    SourceCodeReference(const char* location,SourceCodeManager& manager): location(location), manager(manager){};
    virtual void printContext() = 0;
    /*
     * this function returns the corresponding line and position within the line for a given pointer to the source code
     */
    std::pair<size_t ,size_t> resolveLocation();
};
class SourceCodeReferenceLocation: public SourceCodeReference{
    public:
    SourceCodeReferenceLocation(const char* location, SourceCodeManager& manager);
    void printContext() override;

};

class SourceCodeReferenceRange: public SourceCodeReference{
    size_t length;
    public:
    SourceCodeReferenceRange(const char* location,size_t length, SourceCodeManager& manager);
    void printContext() override;
};
} // namespace sourceCodeManagement
#endif //FINAL_SOURCECODEMANAGER_H
