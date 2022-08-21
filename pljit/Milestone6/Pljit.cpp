#include "Pljit.h"
namespace pljit {
    Pljit::PljitStatus::PljitStatus(std::string_view code, std::unique_ptr<semantic::ASTTree> astNode):code(code), astNode(std::move(astNode)){}

    PljitHandle Pljit::registerFunction(const std::string_view code){
        auto status = PljitStatus(code, nullptr);
        functionStatus.push_back(std::move(status));
        return PljitHandle(functionStatus.back());
    }

    PljitHandle::PljitHandle(Pljit::PljitStatus& jit): jit(jit){}
} // namespace pljit