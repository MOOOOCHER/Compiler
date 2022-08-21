#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H
#include "../Milestone2/Token.h"
#include "../Milestone3/Parser.h"
#include "../Milestone4/SemanticAnalyzer.h"
#include "../Milestone5/ASTEvaluator.h"
#include "../Milestone5/OptimizationPass.h"
#include "pljit/ASTDataStructures/ASTNode.h"
#include <iostream>
#include <mutex>
#include <string>
#include <memory>
#include <vector>
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
        mutable std::mutex mutex;
        std::string code;
        std::unique_ptr<semantic::ASTTree> astTree;

        public:
        explicit PljitStatus(std::string code);
    };
    std::vector<std::unique_ptr<PljitStatus>> functionStatus;
    public:
    PljitHandle registerFunction(std::string code);
};

class PljitHandle{
    friend class Pljit;
    Pljit::PljitStatus* jit;     //raw pointer is needed to make the handle copyable; shared_ptr would be very slow

    explicit PljitHandle(Pljit::PljitStatus* jit);
    public:
    template <std::floating_point ... Args>
    std::optional<double> operator()(Args... args) const{
        if(jit->code.empty()){
            std::cout << "Please insert code!" << std::endl;
            return {};
        }
        {
            //synchronization is only used for compiling the program
            // we don't need synchronization for evaluation, since we then only have a read-only on the handle, (more specifically: the SymbolTable of the ASTTree within PljitStatus)
            // the lock is needed here in order to avoid threads unnecessarily compiling the function again after waiting for the compilation process in a different thread.
            std::unique_lock lock(jit->mutex);
            if (!jit->astTree) {
                //compile new
                SourceCodeManager manager(jit->code);
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
                semantic::AssociationPass associationPass;
                associationPass.optimize(*semanticNode);
                ConstantPropagationPass constantPropagationPass = ConstantPropagationPass();
                constantPropagationPass.optimize(*semanticNode);
                jit->astTree = std::move(semanticNode);
            }
        }
        std::vector<double> vec = {args...};
        ASTEvaluator evaluator = ASTEvaluator();
        return evaluator.evaluateFunction(vec,*jit->astTree); // the ast tree will not be modified in the evaluation, hence no need of synchronization (tradeoff: speed vs memory space)
    }
};
} // namespace pljit

#endif //PLJIT_PLJIT_H
