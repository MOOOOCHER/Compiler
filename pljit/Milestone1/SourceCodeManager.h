#ifndef FINAL_SOURCECODEMANAGER_H
#define FINAL_SOURCECODEMANAGER_H
#include <string_view>
namespace sourceCodeManagement{
class SourceCodeReference;

struct SourceCodeManager{
    std::string_view source;
    explicit SourceCodeManager(std::string_view source);

    SourceCodeManager(const SourceCodeManager& other) = default;
    SourceCodeManager& operator=(const SourceCodeManager& other)= default;
    public:
    SourceCodeManager() = default;
};
class SourceCodeReference{
    size_t positionInCode = 0;
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
    /*
     * this function returns the starting position of the reference
     */
    size_t getPositionInCode() const;
    /*
     * this function returns the length of the referenced string
     */
    size_t getLengthOfString() const;
    /*
     * this function returns the sourceCodeManager
     */
    SourceCodeManager& getManager() const;
    /*
     * this function returns the actual referenced source code
     */
    std::string_view getText() const;
    /*
     * standard constructor pointing to the last element of the code
     */
    explicit SourceCodeReference(SourceCodeManager& manager);
    SourceCodeReference(size_t position, SourceCodeManager& manager, size_t lengthOfString = 1): positionInCode(position), manager(manager), lengthOfString(lengthOfString){}
    //copy semantics
    SourceCodeReference(const SourceCodeReference& other);
    SourceCodeReference& operator=(const SourceCodeReference& other);
    //move semantics
    SourceCodeReference(SourceCodeReference&& other) noexcept;
    SourceCodeReference& operator=(SourceCodeReference&& other)noexcept;

};
} // namespace sourceCodeManagement
#endif //FINAL_SOURCECODEMANAGER_H
