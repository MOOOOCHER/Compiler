#include "Pljit.h"
#include <utility>
namespace pljit {
    Pljit::PljitStatus::PljitStatus(std::string code):code(std::move(code)){}

    PljitHandle Pljit::registerFunction(std::string code){
        auto status = std::make_unique<PljitStatus>(std::move(code));
        auto ptr = status.get();
        functionStatus.push_back(std::move(status));
        return PljitHandle(ptr);
    }

    PljitHandle::PljitHandle(Pljit::PljitStatus* jit): jit(jit){}
} // namespace pljit