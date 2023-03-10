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
using AssociationPass = semantic::AssociationPass;

static void evaluate(const std::vector<double>& args, const std::string_view& input, std::optional<double> expectedResult){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer(manager);

    Parser parser = Parser(tokenizer);
    auto parseNode = parser.expectFunctionDefinition();
    SemanticAnalyzer semantic = SemanticAnalyzer();
    auto astNode = semantic.analyzeSemantic(*parseNode);
    semantic::ASTEvaluator evaluator;
    //optimization
    DeadCodeEliminationPass pass = DeadCodeEliminationPass();
    pass.optimize(*astNode);
    AssociationPass assPass = AssociationPass ();
    assPass.optimize(*astNode);
    ConstantPropagationPass constantPropagationPass = ConstantPropagationPass();
    constantPropagationPass.optimize(*astNode);
    if(expectedResult.has_value()){
        EXPECT_EQ(evaluator.evaluateFunction(args,*astNode).value(),expectedResult.value());
    }else {
        EXPECT_EQ(evaluator.evaluateFunction(args,*astNode).has_value(),expectedResult.has_value());
    }
}

static auto setupWithOptimization( const std::string_view& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer(manager);

    Parser parser = Parser(tokenizer);
    auto parseNode = parser.expectFunctionDefinition();
    SemanticAnalyzer semantic = SemanticAnalyzer();
    auto astNode = semantic.analyzeSemantic(*parseNode);
    //optimization
    DeadCodeEliminationPass pass = DeadCodeEliminationPass();
    pass.optimize(*astNode);
    AssociationPass assPass = AssociationPass ();
    assPass.optimize(*astNode);
    ConstantPropagationPass constantPropagationPass = ConstantPropagationPass();
    constantPropagationPass.optimize(*astNode);
    return astNode;
}

TEST(TestEvaluation, ConstantPropagation){
    auto args = std::vector<double>();
    auto astNode = setupWithOptimization("CONST a = 1, b = 2; BEGIN RETURN a+b END.");
    //test statement child nodes
    auto astChild = astNode->root->getChildren();
    ASSERT_EQ(astChild.size(), 1);
    auto astStatement = static_cast<semantic::ASTReturnStatementNode*>(astChild[0].get());
    auto childStatement = astStatement->getChild();
    auto statementChild = static_cast<semantic::ASTLiteralNode*>(childStatement.get());
    EXPECT_EQ(statementChild->getType(), semantic::ASTNode::LiteralConstant);
    EXPECT_EQ(statementChild->getValue(), 3);

    astNode = setupWithOptimization("VAR a, b; BEGIN a := 1; b := a+1; RETURN a+b END.");
    //test statement child nodes
    astChild = astNode->root->getChildren();
    ASSERT_EQ(astChild.size(), 3);
    astStatement = static_cast<semantic::ASTReturnStatementNode*>(astChild[2].get());
    childStatement = astStatement->getChild();
    statementChild = static_cast<semantic::ASTLiteralNode*>(childStatement.get());
    EXPECT_EQ(statementChild->getType(), semantic::ASTNode::LiteralConstant);
    EXPECT_EQ(statementChild->getValue(), 3);

}
TEST(TestEvaluation, DeadCodeElimination){
    auto args = std::vector<double>();
    auto astNode = setupWithOptimization("VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab;ab := 1 ;ab := 1 ;ab := 1  END.");
    //check we only have 2 statements in the optimized tree
    auto children = astNode->root->getChildren();
    EXPECT_EQ(children.size() , 2);
}
TEST(TestEvaluation,MixedOptimization){
    auto args = std::vector<double>();
    auto astNode = setupWithOptimization("VAR a, b; BEGIN a := 1; b := 1; RETURN a *(-b + 55 * (1-(-1))) ;a := 1; b := 1;a := 1; b := 1 END.");
    semantic::ASTEvaluator evaluator;
    auto result = evaluator.evaluateFunction(args,*astNode);
    EXPECT_EQ(result.value(), 109);
}
TEST(TestEvaluation, ValidEvaluation){
    evaluate(std::vector<double>(),"VAR a, b; BEGIN a := 1; b := 1; RETURN (a+b * 2/(4+4)) END.",1.25);
    evaluate(std::vector<double>(),"CONST a = 1, b = 2; BEGIN RETURN a *(-b + 55 * (1-(-1)) ) END.",108);
    evaluate(std::vector<double>(),"VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab END.",1);
    evaluate(std::vector<double>(),"VAR a,b,c ; BEGIN a := 1; b:=6; c:=2 ;RETURN a+b*(c+2) END.",25);
    evaluate(std::vector<double>{50,2},"PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.",53);
    evaluate(std::vector<double>{50,2},"PARAM a,b; VAR c; BEGIN c:=2+b ;RETURN a+b*c END.",58);
    evaluate(std::vector<double>{3,2},"PARAM a,b; VAR c; BEGIN c:=2+b ;RETURN a+b-c+a-b END.",2);
    evaluate(std::vector<double>{},"BEGIN RETURN 1-1-1-1 END.",-2);
    evaluate(std::vector<double>{3,1000, 12},"PARAM a,b,c; VAR d,e; BEGIN a := b*10 + a ; d:=c+1; e:= a+b+c; RETURN (a+b+c+d+e)/5 END.",4408.6);
    auto func = [](double a, double b, double c){
        double d,e;
        const double f = 345000;
        d = a +(b*a) -5;
        a *= 10;
        e = 700000+f;
        c = d+a;
        return a+b+c-d-e+f;
         };
    evaluate(std::vector<double>{3,5, 12},"PARAM a,b,c; VAR d,e; CONST f= 345000;\n"
                                               " BEGIN d:= a+(b*a)-5; a:= a*10; e:=700000+f; "
                                               " c:=d+a; RETURN a+b+c-d-e+f END.",func(3,5,12));
    evaluate(std::vector<double>{}," BEGIN RETURN 2/2/2 END.",0.5);
}
TEST(TestEvaluation, ValidEvaluationTestAssociativity){
    evaluate(std::vector<double>{10,1},"PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+c-(b-c) END.",12);
    evaluate(std::vector<double>{50,2},"PARAM a,b; VAR c; BEGIN c:=2+b ;RETURN a-(b+1+1+1) - b+ c END.",47);
    evaluate(std::vector<double>{3,2},"PARAM a,b; VAR c; BEGIN c:=2+b ;RETURN a+b-(2*2/4*3 -3 +c) END.",1);
    evaluate(std::vector<double>{3,2,10},"PARAM a,b,ab; BEGIN RETURN a+a+(b-2*a*b/10*(3*3+ab)+3)+2*ab END.",8.2);
    evaluate(std::vector<double>{},"BEGIN RETURN 2*2*2/2/2*(1-1+1-1+1*(1-1+1-1+10*10)) END.",200);
}
TEST(TestEvaluation, InvalidEvaluation){
    std::cout << "Testing divide by zero statement:" << std::endl;
    evaluate(std::vector<double>(),"VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1/0 ;RETURN ab END.",std::optional<double>());
    evaluate(std::vector<double>(),"VAR a, b; BEGIN a := 1; b := 1; RETURN (a+b * 2/(4-4))END.",std::optional<double>());
    evaluate(std::vector<double>{0,2},"PARAM a,b; VAR c; BEGIN c:=2+b ;RETURN b*c/a END.",std::optional<double>());
    evaluate(std::vector<double>{50,2},"PARAM a,b; CONST c=2; BEGIN a:=a+c ;RETURN a/(b-c) END.",std::optional<double>());
    evaluate(std::vector<double>{50,2},"PARAM a,b; CONST c=2; BEGIN a:=a+c ;RETURN a/(b-2) END.",std::optional<double>());
    std::cout << "=========================================================" << std::endl;
}
TEST(TestEvaluation, AnalyzeFunctionNoInitialization){
    std::cout << "Testing no initialization for constant and parameter:" << std::endl;
    evaluate(std::vector<double>(),"PARAM b; CONST a=1; BEGIN b := a +2; RETURN b END.",std::optional<double>());
    evaluate(std::vector<double>{1,1},"PARAM b; CONST a=1; BEGIN b := a +2; RETURN b END.",std::optional<double>());
    evaluate(std::vector<double>{1,1},"VAR b; CONST a=1; BEGIN b := a +2; RETURN b END.",std::optional<double>());
    std::cout << "=========================================================" << std::endl;
}
