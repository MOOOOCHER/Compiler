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

static auto setupWithOptimizationShort(const std::vector<double>& args, const std::string_view& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer(manager);

    Parser parser = Parser(tokenizer);
    auto parseNode = parser.expectFunctionDefinition();
    SemanticAnalyzer semantic = SemanticAnalyzer();
    semantic::ASTEvaluator evaluator = semantic::ASTEvaluator();
    auto astNode = semantic.analyzeFunction(*parseNode);
    //optimization
    DeadCodeEliminationPass pass = DeadCodeEliminationPass();
    pass.optimize(*astNode);
    ConstantPropagationPass constantPropagationPass = ConstantPropagationPass();
    constantPropagationPass.optimize(*astNode);
    return evaluator.evaluateFunction(args,*astNode);
}
static auto setupWithOptimization( const std::string_view& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer(manager);

    Parser parser = Parser(tokenizer);
    auto parseNode = parser.expectFunctionDefinition();
    SemanticAnalyzer semantic = SemanticAnalyzer();
    auto astNode = semantic.analyzeFunction(*parseNode);
    //optimization
    DeadCodeEliminationPass pass = DeadCodeEliminationPass();
    pass.optimize(*astNode);
    ConstantPropagationPass constantPropagationPass = ConstantPropagationPass();
    constantPropagationPass.optimize(*astNode);
    return astNode;
}

TEST(TestEvaluation, ConstantPropagation){
    auto args = std::vector<double>();
    auto astNode = setupWithOptimization("CONST a = 1, b = 2; BEGIN RETURN a+b END.");
    //test statement child nodes
    auto astChild = astNode->getChildren();
    ASSERT_EQ(astChild.size(), 2);
    auto astCompoundStatement = static_cast<semantic::ASTCompoundStatement*>(astChild[1].get());
    auto astCompoundStatementChild = astCompoundStatement->getChildren();
    EXPECT_EQ(astCompoundStatementChild.size(),1);
    auto astStatement = static_cast<semantic::ASTReturnStatementNode*>(astCompoundStatementChild[0].get());
    auto childStatement = astStatement->getChild();
    auto statementChild = static_cast<semantic::ASTLiteralNode*>(childStatement.get());
    EXPECT_EQ(statementChild->getType(), semantic::ASTNode::LiteralConstant);
    EXPECT_EQ(statementChild->getValue(), 3);

    astNode = setupWithOptimization("VAR a, b; BEGIN a := 1; b := a+1; RETURN a+b END.");
    //test statement child nodes
    astChild = astNode->getChildren();
    ASSERT_EQ(astChild.size(), 2);
    astCompoundStatement = static_cast<semantic::ASTCompoundStatement*>(astChild[1].get());
    astCompoundStatementChild = astCompoundStatement->getChildren();
    EXPECT_EQ(astCompoundStatementChild.size(),3);
    astStatement = static_cast<semantic::ASTReturnStatementNode*>(astCompoundStatementChild[2].get());
    childStatement = astStatement->getChild();
    statementChild = static_cast<semantic::ASTLiteralNode*>(childStatement.get());
    EXPECT_EQ(statementChild->getType(), semantic::ASTNode::LiteralConstant);
    EXPECT_EQ(statementChild->getValue(), 3);

    astNode = setupWithOptimization("VAR a, b; BEGIN a := 1; b := 1; RETURN (a+b * 2/(4+4)) END.");
    auto evaluator =   semantic::ASTEvaluator ();
    auto result = evaluator.evaluateFunction(args,*astNode);
    EXPECT_EQ(result.value(), 1.25);
    astNode = setupWithOptimization("CONST a = 1, b = 2; BEGIN RETURN a *(-b + 55 * (1-(-1)) ) END.");
    result = evaluator.evaluateFunction(args,*astNode);
    EXPECT_EQ(result.value(), 108);
}
TEST(TestEvaluation, DeadCodeElimination){
    auto args = std::vector<double>();
    auto astNode = setupWithOptimization("VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab;ab := 1 ;ab := 1 ;ab := 1  END.");
    ASSERT_EQ(astNode->getType(), semantic::ASTNode::FunctionDefinition);
    //check we only have 2 statements in the optimized tree
    auto funcDef = static_cast<semantic::ASTFunctionNode*>(astNode.get());
    auto children = funcDef->getChildren();
    EXPECT_EQ(children.size() , 2);
    auto compoundStatement = static_cast<semantic::ASTCompoundStatement*>(children[1].get());
    auto compoundStatementChild = compoundStatement->getChildren();
    EXPECT_EQ(compoundStatementChild.size() , 2);
}
TEST(TestEvaluation,MixedOptimization){
    auto args = std::vector<double>();
    auto astNode = setupWithOptimization("VAR a, b; BEGIN a := 1; b := 1; RETURN a *(-b + 55 * (1-(-1))) ;a := 1; b := 1;a := 1; b := 1 END.");
    semantic::ASTEvaluator evaluator = semantic::ASTEvaluator();
    auto result = evaluator.evaluateFunction(args,*astNode);
    EXPECT_EQ(result.value(), 109);
}
TEST(TestEvaluation, ValidEvaluation){
    auto result = setupWithOptimizationShort(std::vector<double>(),"VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab END.");
    EXPECT_EQ(result.value(), 1);
    result = setupWithOptimizationShort(std::vector<double>(),"VAR a,b,c ; BEGIN a := 1; b:=6; c:=2 ;RETURN a+b*(c+2) END.");
    EXPECT_EQ(result.value(), 25);
    result = setupWithOptimizationShort(std::vector<double>{50,2},"PARAM a,b; VAR c; BEGIN c:=2+b ;RETURN a+b*c END.");
    EXPECT_EQ(result.value(), 58);
    result = setupWithOptimizationShort(std::vector<double>{50,2},"PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.");
    EXPECT_EQ(result.value(), 53);
}
TEST(TestEvaluation, InvalidEvaluation){
    std::cout << "Testing divide by zero statement:" << std::endl;
    auto result = setupWithOptimizationShort(std::vector<double>(),"VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1/0 ;RETURN ab END.");
    EXPECT_EQ(result.has_value(), false);
    result = setupWithOptimizationShort(std::vector<double>(),"VAR a, b; BEGIN a := 1; b := 1; RETURN (a+b * 2/(4-4))END.");
    EXPECT_EQ(result.has_value(), false);
    result = setupWithOptimizationShort(std::vector<double>{0,2},"PARAM a,b; VAR c; BEGIN c:=2+b ;RETURN b*c/a END.");
    EXPECT_EQ(result.has_value(), false);
    result = setupWithOptimizationShort(std::vector<double>{50,2},"PARAM a,b; CONST c=2; BEGIN a:=a+c ;RETURN a/(b-c) END.");
    EXPECT_EQ(result.has_value(), false);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestEvaluation, AnalyzeFunctionNoInitialization){
    std::cout << "Testing no initialization for constant and parameter:" << std::endl;
    auto result = setupWithOptimizationShort(std::vector<double>(),"PARAM b; CONST a=1; BEGIN b := a +2; RETURN b END.");
    EXPECT_EQ(result.has_value(), false);
    result = setupWithOptimizationShort(std::vector<double>{1,1},"PARAM b; CONST a=1; BEGIN b := a +2; RETURN b END.");
    EXPECT_EQ(result.has_value(), false);
    std::cout << "=========================================================" << std::endl;
}
