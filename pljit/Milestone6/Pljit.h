#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H
#include <string>
#include <vector>
#include "../Milestone1/SourceCodeManager.h"
#include "../Milestone4/ASTNode.h"
#include "../Milestone2/Token.h"
#include "../Milestone3/Parser.h"
#include "../Milestone4/SemanticAnalyzer.h"
#include "../Milestone5/ASTEvaluator.h"

namespace pljit {
using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
using Tokenizer = lexer::Tokenizer;
using Parser = parser::Parser;
using SemanticAnalyzer = semantic::SemanticAnalyzer;
using ASTEvaluator = semantic::ASTEvaluator;

class PljitHandle;

class Pljit {
    friend class PljitHandle;
    class PljitStatus{
        friend class Pljit;
        friend class PljitHandle;

        std::string_view code;
        std::unique_ptr<semantic::ASTNode> astNode;
        PljitStatus(std::string_view code, std::unique_ptr<semantic::ASTNode> astNode):code(code), astNode(std::move(astNode)){}
    };
    std::vector<PljitStatus> functionStatus;
    public:
    PljitHandle registerFunction(std::string_view code);
};
class PljitHandle{
    friend class Pljit;
    Pljit::PljitStatus& jit;

    explicit PljitHandle(Pljit::PljitStatus& jit): jit(jit){}
    public:
    template <typename... Args>
    auto operator()(Args... args){
        static_assert((std::is_same<Args, double>::value && ...));
        std::vector<double> vec = {args...};
        if(jit.astNode == nullptr){
            //compile new
            SourceCodeManager manager(jit.code);
            Tokenizer tokenizer = Tokenizer(manager);

            Parser parser = Parser(tokenizer);
            auto parseNode = parser.expectFunctionDefinition();
            SemanticAnalyzer semantic = SemanticAnalyzer();
            auto node = semantic.analyzeFunction(vec,*parseNode);
            //TODO: optimization
            jit.astNode = std::move(node);
        }
        ASTEvaluator evaluator = ASTEvaluator();
        auto result = evaluator.evaluateFunction(*jit.astNode);
        if(!result.has_value()){
            return;
        }
        return evaluator.evaluateFunction(*jit.astNode).value();
    }
};
} // namespace pljit

#endif //PLJIT_PLJIT_H
