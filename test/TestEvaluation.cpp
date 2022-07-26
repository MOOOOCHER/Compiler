#include "pljit/Milestone1/SourceCodeManager.h"
#include "pljit/Milestone2/Token.h"
#include "pljit/Milestone3/Parser.h"
#include "pljit/Milestone4/SemanticAnalyzer.h"
#include "pljit/Milestone4/ASTTreePrintVisitor.h"
#include "pljit/Milestone5/ASTEvaluator.h"
#include <gtest/gtest.h>

using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
using Tokenizer = lexer::Tokenizer;
using Parser = parser::Parser;
using Node = parser::Node;
using SemanticAnalyzer = semantic::SemanticAnalyzer;

static auto setup(const std::string_view& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer(manager);

    Parser parser = Parser(tokenizer);
    auto parseNode = parser.expectFunctionDefinition();
    SemanticAnalyzer semantic = SemanticAnalyzer();
    semantic::ASTEvaluator evaluator = semantic::ASTEvaluator();
    return evaluator.evaluateFunction(*semantic.analyzeFunction(std::vector<double>(),*parseNode));
}
TEST(TestEvaluation, ValidEvaluation){
    auto result = setup("VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab END.");
    EXPECT_EQ(result.value(), 1);
    result = setup("VAR a,b,c ; BEGIN a := 1; b:=6; c:=2 ;RETURN a+b*(c+2) END.");
    EXPECT_EQ(result.value(), 25);
}
TEST(TestEvaluation, DeadCodeElimination){
    auto result = setup("VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab;ab := 1 ;ab := 1 ;ab := 1  END.");
    EXPECT_EQ(result.value(), 1);
    result = setup("VAR a, b; BEGIN a := 1; b := 1; RETURN (a+b * 2/(4+4));a := 1; b := 1;a := 1; b := 1 END.");
    EXPECT_EQ(result.value(), 1.25);
    result = setup("VAR a, b; BEGIN a := 1; b := 1; RETURN a *(-b + 55 * (1-(-1)) ) ;a := 1; b := 1;a := 1; b := 1 END.");
    EXPECT_EQ(result.value(), 109);
}
TEST(TestEvaluation, ConstantPropagation){
    auto result = setup("VAR a, b; BEGIN a := 1; b := 1; RETURN (a+b * 2/(4+4));a := 1; b := 1;a := 1; b := 1 END.");
    EXPECT_EQ(result.value(), 1.25);
    result = setup("VAR a, b; BEGIN a := 1; b := 1; RETURN a *(-b + 55 * (1-(-1)) ) ;a := 1; b := 1;a := 1; b := 1 END.");
    EXPECT_EQ(result.value(), 109);
}
TEST(TestEvaluation, InvalidEvaluation){
    std::cout << "Testing divide by zero statement:" << std::endl;
    auto result = setup("VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1/0 ;RETURN ab END.");
    EXPECT_EQ(result.has_value(), false);
    result = setup("VAR a, b; BEGIN a := 1; b := 1; RETURN (a+b * 2/(4-4))END.");
    EXPECT_EQ(result.has_value(), false);
    std::cout << "=========================================================" << std::endl;
}
