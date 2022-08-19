#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include "../Milestone4/ASTNode.h"
#include "../Milestone2/Token.h"
#include "../Milestone3/Parser.h"
#include "../Milestone4/SemanticAnalyzer.h"
#include "../Milestone5/ASTEvaluator.h"
#include "../Milestone5/OptimizationPass.h"
namespace pljit {
using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
using Tokenizer = lexer::Tokenizer;
using Parser = parser::Parser;
using SemanticAnalyzer = semantic::SemanticAnalyzer;
using ASTEvaluator = semantic::ASTEvaluator;
using DeadCodeEliminationPass = semantic::DeadCodeEliminationPass;
using ConstantPropagationPass = semantic::ConstantPropagationPass;

class PljitHandle;
class Pljit {
    friend class PljitHandle;
    class PljitStatus{
        friend class Pljit;
        friend class PljitHandle;

        std::string_view code;
        std::unique_ptr<semantic::ASTTree> astNode;
        PljitStatus(std::string_view code, std::unique_ptr<semantic::ASTTree> astNode);
    };
    std::vector<PljitStatus> functionStatus;
    public:
    PljitHandle registerFunction(std::string_view code);
};

class PljitHandle{
    friend class Pljit;
    Pljit::PljitStatus& jit;
    mutable std::mutex mutex;

    explicit PljitHandle(Pljit::PljitStatus& jit): jit(jit){}

    public:
    template <std::floating_point ... Args>
    std::optional<double> operator()(Args... args) {
        if(jit.code.empty()){
            std::cout << "Please insert code!" << std::endl;
            return {};
        }

        std::unique_lock lock(mutex);
            if (jit.astNode == nullptr) {
                //compile new
                SourceCodeManager manager(jit.code);
                Parser parser = Parser(Tokenizer(manager));
                auto parseNode = parser.expectFunctionDefinition();
                if (!parseNode) {
                    //Parsing failed
                    return {};
                }
                SemanticAnalyzer semantic = SemanticAnalyzer();
                auto semanticNode = semantic.analyzeSemantic(*parseNode);
                if (!semanticNode->root) {
                    //semantic analyze failed
                    return {};
                }
                //optimization
                DeadCodeEliminationPass pass = DeadCodeEliminationPass();
                pass.optimize(*semanticNode);
                ConstantPropagationPass constantPropagationPass = ConstantPropagationPass();
                constantPropagationPass.optimize(*semanticNode);
                jit.astNode = std::move(semanticNode);
            }
        std::vector<double> vec = {args...};
        ASTEvaluator evaluator = ASTEvaluator(jit.astNode->getTable());
        return evaluator.evaluateFunction(vec,*jit.astNode); // the ast tree will not be modified in the evaluation, hence no need of synchronization
    }
};
} // namespace pljit

#endif //PLJIT_PLJIT_H
