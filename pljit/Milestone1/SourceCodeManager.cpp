#include "SourceCodeManager.h"
namespace sourceCodeManagement{
    SourceCodeManager::SourceCodeManager(std::string_view source): source(source) {}

    SourceCodeReferenceLocation::SourceCodeReferenceLocation(size_t line, size_t position, char* location): SourceCodeReference(line,position,location) {}

    SourceCodeReferenceRange::SourceCodeReferenceRange(size_t line, size_t position, size_t length, char* location): SourceCodeReference(line,position,location),length(length) {}
}
