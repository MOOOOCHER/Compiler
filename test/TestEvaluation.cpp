#include "pljit/Milestone1/SourceCodeManager.h"
#include "pljit/Milestone2/Token.h"
#include "pljit/Milestone3/Parser.h"
#include "pljit/Milestone4/SemanticAnalyzer.h"
#include "pljit/Milestone4/ASTTreePrintVisitor.h"
#include "pljit/Milestone5/ASTEvaluator.h"
#include "pljit/Milestone5/OptimizationPass.h"
#include <gtest/gtest.h>

using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
using Tokenizer = lexer::Tokenizer;
using Parser = parser::Parser;
using Node = parser::Node;
using SemanticAnalyzer = semantic::SemanticAnalyzer;
using DeadCodeEliminationPass = semantic::DeadCodeEliminationPass;
using ConstantPropagationPass = semantic::ConstantPropagationPass;

static auto setup(std::vector<double> args, const std::string_view& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer(manager);

    Parser parser = Parser(tokenizer);
    auto parseNode = parser.expectFunctionDefinition();
    SemanticAnalyzer semantic = SemanticAnalyzer();
    semantic::ASTEvaluator evaluator = semantic::ASTEvaluator();
    return evaluator.evaluateFunction(args,*semantic.analyzeFunction(args,*parseNode));
}
static auto setupWithOptimization(std::vector<double> args, const std::string_view& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer(manager);

    Parser parser = Parser(tokenizer);
    auto parseNode = parser.expectFunctionDefinition();
    SemanticAnalyzer semantic = SemanticAnalyzer();
    auto astNode = semantic.analyzeFunction(args,*parseNode);
    //optimization
    DeadCodeEliminationPass pass = DeadCodeEliminationPass();
    pass.optimize(*astNode);
    ConstantPropagationPass constantPropagationPass = ConstantPropagationPass();
    constantPropagationPass.optimize(*astNode);
    return astNode;
    //TODO:test optimizations
}
TEST(TestEvaluation, ValidEvaluationWithoutOptimization){
    auto result = setup(std::vector<double>(),"VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab END.");
    EXPECT_EQ(result.value(), 1);
    result = setup(std::vector<double>(),"VAR a,b,c ; BEGIN a := 1; b:=6; c:=2 ;RETURN a+b*(c+2) END.");
    EXPECT_EQ(result.value(), 25);
}
TEST(TestEvaluation, DeadCodeElimination){
    auto args = std::vector<double>();
    auto astNode = setupWithOptimization(args,"VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab;ab := 1 ;ab := 1 ;ab := 1  END.");
    ASSERT_EQ(astNode->getType(), semantic::ASTNode::FunctionDefinition);
    //check we only have 2 statements in the optimized tree
    auto funcDef = static_cast<semantic::ASTFunctionNode*>(astNode.get());
    EXPECT_EQ(funcDef->getChildren().size() , 2);
    auto compoundStatement = static_cast<semantic::ASTCompoundStatement*>(funcDef->getChildren()[1]);
    EXPECT_EQ(compoundStatement->getChildren().size() , 2);

    semantic::ASTEvaluator evaluator = semantic::ASTEvaluator();
    auto result = evaluator.evaluateFunction(args,*astNode);
    EXPECT_EQ(result.value(), 1);
}
TEST(TestEvaluation, ConstantPropagation){
    auto args = std::vector<double>();
    auto astNode = setupWithOptimization(args,"CONST a = 1, b = 2; BEGIN RETURN a+b END.");
    semantic::ASTEvaluator evaluator = semantic::ASTEvaluator();
    auto result = evaluator.evaluateFunction(args,*astNode);
    EXPECT_EQ(result.value(), 3);
    astNode = setupWithOptimization(args,"VAR a, b; BEGIN a := 1; b := a+1; RETURN a+b END.");
    result = evaluator.evaluateFunction(args,*astNode);
    EXPECT_EQ(result.value(), 3);
    astNode = setupWithOptimization(args,"VAR a, b; BEGIN a := 1; b := 1; RETURN (a+b * 2/(4+4)) END.");
    result = evaluator.evaluateFunction(args,*astNode);
    EXPECT_EQ(result.value(), 1.25);
    astNode = setupWithOptimization(args,"CONST a = 1, b = 2; BEGIN RETURN a *(-b + 55 * (1-(-1)) ) END.");
    result = evaluator.evaluateFunction(args,*astNode);
    EXPECT_EQ(result.value(), 109);
}
TEST(TestEvaluation,MixedOptimization){
    auto args = std::vector<double>();
    auto astNode = setupWithOptimization(args,"VAR a, b; BEGIN a := 1; b := 1; RETURN a *(-b + 55 * (1-(-1))) ;a := 1; b := 1;a := 1; b := 1 END.");
    semantic::ASTEvaluator evaluator = semantic::ASTEvaluator();
    auto result = evaluator.evaluateFunction(args,*astNode);
    EXPECT_EQ(result.value(), 109);
}
TEST(TestEvaluation, InvalidEvaluation){
    std::cout << "Testing divide by zero statement:" << std::endl;
    auto result = setup(std::vector<double>(),"VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1/0 ;RETURN ab END.");
    EXPECT_EQ(result.has_value(), false);
    result = setup(std::vector<double>(),"VAR a, b; BEGIN a := 1; b := 1; RETURN (a+b * 2/(4-4))END.");
    EXPECT_EQ(result.has_value(), false);
    std::cout << "=========================================================" << std::endl;
}
