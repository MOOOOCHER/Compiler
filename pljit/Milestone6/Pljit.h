#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H
#include <string>
#include <vector>
#include <iostream>
#include "../Milestone1/SourceCodeManager.h"
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
//TODO:Multithreading
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
    template <std::integral... Args>
    std::optional<double> operator()(Args... args) {
        std::vector<long> vec = {args...};
        if(jit.astNode == nullptr){
            //compile new
            if(jit.code.empty()){
                std::cout << "Please insert code!" << std::endl;
            }
            SourceCodeManager manager(jit.code);
            Parser parser = Parser(Tokenizer(manager));
            auto parseNode = parser.expectFunctionDefinition();
            if(!parseNode){
                //Parsing failed
                return std::optional<double>();
            }
            SemanticAnalyzer semantic = SemanticAnalyzer();
            auto semanticNode = semantic.analyzeFunction(vec,*parseNode);
            if(!semanticNode){
                //semantic analyze failed
                return std::optional<double>();
            }
            //optimization
            DeadCodeEliminationPass pass = DeadCodeEliminationPass();
            pass.optimize(*semanticNode);
            ConstantPropagationPass constantPropagationPass = ConstantPropagationPass();
            constantPropagationPass.optimize(*semanticNode);
            jit.astNode = std::move(semanticNode);
        }
        ASTEvaluator evaluator = ASTEvaluator();
        auto result = evaluator.evaluateFunction(vec,*jit.astNode);
        if(!result.has_value()){
            return std::optional<double>();
        }
        return result.value();
    }
};
} // namespace pljit

#endif //PLJIT_PLJIT_H
