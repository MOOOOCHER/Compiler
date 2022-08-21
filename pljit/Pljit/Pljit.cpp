#include "Pljit.h"
#include <utility>
namespace pljit {
    Pljit::PljitStatus::PljitStatus(std::string code):code(std::move(code)){}

    PljitHandle Pljit::registerFunction(std::string code){
        auto status = PljitStatus(std::move(code));
        functionStatus.push_back(std::move(status));
        return PljitHandle(this);
    }

    PljitHandle::PljitHandle(Pljit* jit): jit(jit), index(jit->functionStatus.size()-1){}
} // namespace pljit