#include "Pljit.h"
namespace pljit {
    PljitHandle Pljit::registerFunction(const std::string_view code){
        auto status = PljitStatus(code, nullptr);
        functionStatus.push_back(std::move(status));
        return PljitHandle(functionStatus.back());
    }
} // namespace pljit