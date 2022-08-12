#include "pljit/Milestone1/SourceCodeManager.h"
#include "pljit/Milestone2/Token.h"
#include "pljit/Milestone3/Parser.h"
#include "pljit/Milestone4/SemanticAnalyzer.h"
#include "pljit/Milestone4/ASTTreePrintVisitor.h"
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
    return semantic.analyzeFunction(*parseNode);
}
TEST(TestSemantic, AnalyzeFunctionSimpleValid){
    auto result = setup("PARAM ab; BEGIN ab := 1 ;RETURN ab END.");
    EXPECT_NE(result, nullptr);
}
TEST(TestSemantic, AnalyzeFunctionDoubleDeclaration){
    std::cout << "Testing double declaration:" << std::endl;
    auto result = setup("PARAM ab, ab; BEGIN ab := 1; RETURN ab END.");
    EXPECT_EQ(result, nullptr);
    result = setup("PARAM c; VAR ab,ab; CONST b= 1; BEGIN ab := 1; RETURN ab END.");
    EXPECT_EQ(result, nullptr);
    result = setup("PARAM a; VAR ab; CONST c= 1, c= 1; BEGIN ab := 1; RETURN ab END.");
    EXPECT_EQ(result, nullptr);
    result = setup("PARAM ab; VAR ab; BEGIN ab := 1; RETURN ab END.");
    EXPECT_EQ(result, nullptr);
    result = setup("PARAM c; VAR ab; CONST c= 1; BEGIN ab := 1; RETURN ab END.");
    EXPECT_EQ(result, nullptr);
    result = setup("PARAM c; VAR ab; CONST ab= 1; BEGIN ab := 1; RETURN ab END.");
    EXPECT_EQ(result, nullptr);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestSemantic, AnalyzeFunctionUndeclaredIdentifier){
    std::cout << "Testing undeclared identifier:" << std::endl;
    auto result = setup("PARAM ab, a; BEGIN c := 1; RETURN c END.");
    EXPECT_EQ(result, nullptr);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestSemantic, AnalyzeFunctionAssignToConstant){
    std::cout << "Testing assign to constant:" << std::endl;
    auto result = setup("CONST a = 1; BEGIN a := 2; RETURN a END.");
    EXPECT_EQ(result, nullptr);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestSemantic, AnalyzeFunctionNoReturn){
    std::cout << "Testing no return statement:" << std::endl;
    auto result = setup("VAR a; BEGIN a := 2 END.");
    EXPECT_EQ(result, nullptr);
    result = setup("VAR a; BEGIN a := 2 ; a:=a *5 END.");
    EXPECT_EQ(result, nullptr);
    std::cout << "=========================================================" << std::endl;
}

TEST(TestSemantic, AnalyzeFunctionValid){
    auto result = setup("VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab END.");
    EXPECT_NE(result, nullptr);
    result = setup("PARAM p, pp ,ppp ,pppp; VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab END.");
    EXPECT_NE(result, nullptr);
    result = setup("PARAM p, pp ,ppp ,pppp; VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; CONST ee=1, eee= 2, eeee= 3; BEGIN ab := 1 ;RETURN ab END.");
    EXPECT_NE(result, nullptr);
    result = setup("PARAM p, pp ,ppp ,pppp; VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; CONST ee=1, eee= 2, eeee= 3; BEGIN ab := 1 ;RETURN ab END.");
    EXPECT_NE(result, nullptr);
    result = setup("PARAM width, height, depth;\nVAR volume;\nCONST density = 2400;\nBEGIN\nvolume :=width * height * depth;\nRETURN density*volume\nEND.");
    EXPECT_NE(result, nullptr);
    auto visitor = semantic::ASTTreePrintVisitor();
    visitor.printTree(*result);
    result = setup("VAR a, b; BEGIN a := 1; b := 1; RETURN (a+b * 2/(123+4))END.");
    EXPECT_NE(result, nullptr);
    result = setup("VAR a, b; BEGIN a := 1; b := 1; RETURN a *(-b + 55 * (1-(-1)) )END.");
    EXPECT_NE(result, nullptr);
    visitor = semantic::ASTTreePrintVisitor();
    visitor.printTree(*result);
}
