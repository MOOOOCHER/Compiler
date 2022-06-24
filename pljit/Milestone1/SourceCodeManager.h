#ifndef FINAL_SOURCECODEMANAGER_H
#define FINAL_SOURCECODEMANAGER_H
#include <cstddef>
#include <string_view>
namespace sourceCodeManagement{
class SourceCodeManager{
    std::string_view source = "";
public:
    explicit SourceCodeManager(std::string_view source);
};
class SourceCodeReference{
    protected:
    size_t line;
    size_t position;
    char* location;
    SourceCodeReference(size_t line, size_t position, char* location): line(line), position(position), location(location){};
    virtual void printContext() = 0;
};
class SourceCodeReferenceLocation: public SourceCodeReference{
    public:
    SourceCodeReferenceLocation(size_t line, size_t position, char* location);
    void printContext() override;

};

class SourceCodeReferenceRange: public SourceCodeReference{
    size_t length;
    public:
    SourceCodeReferenceRange(size_t line, size_t position,size_t length, char* location);
    void printContext() override;
};
}
#endif //FINAL_SOURCECODEMANAGER_H
