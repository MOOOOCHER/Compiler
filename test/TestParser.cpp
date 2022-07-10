#include "pljit/Milestone1/SourceCodeManager.h"
#include "pljit/Milestone2/Token.h"
#include "pljit/Milestone3/Parser.h"
#include "pljit/Milestone3/Node.h"
#include "pljit/Milestone3/ParseTreePrintVisitor.h"
#include <gtest/gtest.h>
#include <iostream>

using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
using Token = lexer::Token;
using Tokenizer = lexer::Tokenizer;
using Parser = parser::Parser;
using Node = parser::Node;

//helper functions-----------------------------------------------------------------------------------------
static auto setup(const std::string_view& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer(manager);

    Parser parser = Parser(tokenizer);
    return parser.expectFunctionDefinition();
}
static void checkInvalid(const std::string& input){
    auto result = setup(input);
    EXPECT_EQ(result, nullptr);
}
//--------------------------------------------------------------------------------------------------------------------
TEST(TestParser, ExpectFunctionDefinitionValid){
    auto result = setup("BEGIN a := 1 END.");
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 2);
    result = setup("CONST c= 0; BEGIN a := 1 END.");
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    result = setup("VAR b; CONST c= 0; BEGIN a := 1 END.");
    EXPECT_NE(result, nullptr);
    result = setup("PARAM a; VAR b; CONST c= 0; BEGIN a := 1 END.");
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 5);
}
TEST(TestParser, ExpectFunctionDefinitionInvalid){
    std::cout << "Testing invalid function definition declarator:" << std::endl;
    checkInvalid("BEGIN a := 1 END");
    checkInvalid("VAR b; PARAM a; .");
    checkInvalid("VAR b; PARAM a; CONST c= 0; BEGIN a := 1 END.");
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectCompoundStatementValid){
    auto result = setup("BEGIN a:=1 END.");
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::FunctionDefinition);
    EXPECT_EQ(result->getChildren().size(), 2);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::CompoundStatement);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::Dot);
    result = setup("BEGIN RETURN 1 END.");
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::FunctionDefinition);
    EXPECT_EQ(result->getChildren().size(), 2);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::CompoundStatement);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::Dot);
}
TEST(TestParser, ExpectCompoundStatementInvalid){
    std::cout << "Testing invalid compound statement declarator:" << std::endl;
    checkInvalid("RETURN 1 END.");
    checkInvalid("BEGIN RETURN 1 .");
    checkInvalid("VAR a; RETURN 1 END.");
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectParamDeclarationValid){
    auto result = setup("PARAM a;BEGIN a:=1 END.");
    EXPECT_NE(result, nullptr);
    auto paramChild = static_cast<parser::NonTerminalNode*>(result->getChildren()[0]);
    EXPECT_EQ(paramChild->getChildren().size(), 3);
    result = setup("PARAM a,b;BEGIN a:=1 END.");
    EXPECT_NE(result, nullptr);
    paramChild = static_cast<parser::NonTerminalNode*>(result->getChildren()[0]);
    EXPECT_EQ(paramChild->getChildren().size(), 3);
    result = setup("PARAM a,b,c;BEGIN a:=1 END.");
    EXPECT_NE(result, nullptr);
    paramChild = static_cast<parser::NonTerminalNode*>(result->getChildren()[0]);
    EXPECT_EQ(paramChild->getChildren().size(), 3);
    auto declListChild = static_cast<parser::NonTerminalNode*>(paramChild->getChildren()[1]);
    EXPECT_EQ(declListChild->getType(), Node::Types::DeclaratorList);
    EXPECT_EQ(declListChild->getChildren().size(), 5);
    EXPECT_EQ(declListChild->getChildren()[0]->getType(), Node::Types::Identifier);
    EXPECT_EQ(declListChild->getChildren()[1]->getType(), Node::Types::Comma);
}
TEST(TestParser, ExpectParamDeclarationInvalid){
    std::cout << "Testing invalid init parameter declarations:" << std::endl;
    checkInvalid("PARAM a");
    checkInvalid("PARA a,b;");
    checkInvalid("PARAM a b;");
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectVariableDeclarationValid){
    auto result = setup("VAR a;BEGIN a:=1 END.");
    EXPECT_NE(result, nullptr);
    auto child = static_cast<parser::NonTerminalNode*>(result->getChildren()[0]);
    EXPECT_EQ(child->getChildren().size(), 3);
    result = setup("VAR a,b;BEGIN a:=1 END.");
    EXPECT_NE(result, nullptr);
    child = static_cast<parser::NonTerminalNode*>(result->getChildren()[0]);
    EXPECT_EQ(child->getChildren().size(), 3);
}
TEST(TestParser, ExpectVariableDeclarationInvalid){
    std::cout << "Testing invalid init variable declarations:" << std::endl;
    checkInvalid("VAR a BEGIN a:=1 END.");
    checkInvalid("VA a,b;BEGIN a:=1 END.");
    checkInvalid("VAR a b;BEGIN a:=1 END.");
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectConstantDeclarationValid){
    auto result = setup("CONST a=1; BEGIN a:=1 END.");
    EXPECT_NE(result, nullptr);
    auto child = static_cast<parser::NonTerminalNode*>(result->getChildren()[0]);
    EXPECT_EQ(child->getChildren().size(), 3);
    result = setup("CONST a=1,b=2;BEGIN a:=1 END.");
    EXPECT_NE(result, nullptr);
    child = static_cast<parser::NonTerminalNode*>(result->getChildren()[0]);
    EXPECT_EQ(child->getChildren().size(), 3);
    auto declListChild = static_cast<parser::NonTerminalNode*>(child->getChildren()[1]);
    EXPECT_EQ(declListChild->getType(), Node::Types::InitDeclaratorList);
    EXPECT_EQ(declListChild->getChildren().size(), 3);
    EXPECT_EQ(declListChild->getChildren()[0]->getType(), Node::Types::InitDeclarator);
    EXPECT_EQ(declListChild->getChildren()[1]->getType(), Node::Types::Comma);
    EXPECT_EQ(declListChild->getChildren()[2]->getType(), Node::Types::InitDeclarator);
    auto initDeclChild = static_cast<parser::NonTerminalNode*>(declListChild->getChildren()[0]);
    EXPECT_EQ(initDeclChild->getType(), Node::Types::InitDeclarator);
    EXPECT_EQ(initDeclChild->getChildren().size(), 3);
    EXPECT_EQ(initDeclChild->getChildren()[0]->getType(), Node::Types::Identifier);
    EXPECT_EQ(initDeclChild->getChildren()[1]->getType(), Node::Types::InitEquals);
    EXPECT_EQ(initDeclChild->getChildren()[2]->getType(), Node::Types::Literal);
}
TEST(TestParser, ExpectConstantDeclarationInvalid){
    std::cout << "Testing invalid constant declaration:" << std::endl;
    checkInvalid("CONST a;BEGIN a:=1 END.");
    checkInvalid("CONST a=1 BEGIN a:=1 END.");
    checkInvalid("CONST a 1; BEGIN a:=1 END.");
    checkInvalid("CONST a=a, b=2;BEGIN a:=1 END.");
    checkInvalid("CONST a=1 b=2;BEGIN a:=1 END.");

    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectPrimaryExpressionValid){
    auto result = setup("BEGIN RETURN identifier END.");
    EXPECT_NE(result, nullptr);
    result = setup("BEGIN RETURN1234 END.");
    EXPECT_NE(result, nullptr);
    result = setup("BEGIN RETURN (1234) END.");
    EXPECT_NE(result, nullptr);
}
TEST(TestParser, ExpectPrimaryExpressionInvalid){
    std::cout << "Testing invalid primary expression:" << std::endl;
    checkInvalid("BEGIN RETURN END.");
    checkInvalid("BEGIN RETURN (1234 END.");
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectUnaryExpressionValid){
    auto result = setup("BEGIN RETURN + identifier END.");
    EXPECT_NE(result, nullptr);
    result = setup("BEGIN RETURN-1234 END.");
    EXPECT_NE(result, nullptr);
    result = setup("BEGIN a:= -(1234) END.");
    EXPECT_NE(result, nullptr);
    result = setup("BEGIN a:= -(-(1234)) END.");
    EXPECT_NE(result, nullptr);
}
TEST(TestParser, ExpectUnaryExpressionInvalid){
    checkInvalid("BEGIN RETURN -(identifier END.");
    checkInvalid("BEGIN RETURN +(1234 END.");
    checkInvalid("BEGIN RETURN --(1234) END.");
}

TEST(TestParser, ExpectAdditiveExpressionValid){
    auto result = setup("BEGIN RETURN a + a - b END.");
    EXPECT_NE(result, nullptr);
    result = setup("BEGIN a:= a + a - b END.");
    EXPECT_NE(result, nullptr);
    result = setup("BEGIN RETURN a * a END.");
    EXPECT_NE(result, nullptr);
    result = setup("BEGIN a:= a * a END.");
    EXPECT_NE(result, nullptr);
}
TEST(TestParser, ExpectAdditiveExpressionInvalid){
    std::cout << "Testing invalid additive expression:" << std::endl;
    checkInvalid("BEGIN RETURN a + a +*b END.");
    checkInvalid("BEGIN RETURN ((a + a) -b END.");
    checkInvalid("BEGIN RETURN a + a * b -1234+ END.");
    checkInvalid("BEGIN a:= a + a +*b END.");
    checkInvalid("BEGIN a:= ((a + a) -b END.");
    checkInvalid("BEGIN a:= a + a * b -1234+ END.");
    std::cout << "=========================================================" << std::endl;
}

TEST(TestParser, ExpectMultiplicativeExpressionValid){
    auto result = setup("BEGIN RETURN +a END.");
    EXPECT_NE(result, nullptr);

    result = setup("BEGIN RETURN a * a * a END.");
    EXPECT_NE(result, nullptr);

    result = setup("BEGIN RETURN a * (a + a * (super))END.");
    EXPECT_NE(result, nullptr);
    result = setup("BEGIN a:= +a END.");
    EXPECT_NE(result, nullptr);

    result = setup("BEGIN a:= a * a * a END.");
    EXPECT_NE(result, nullptr);

    result = setup("BEGIN a:= a * (a + a * (super))END.");
    EXPECT_NE(result, nullptr);
}
TEST(TestParser, ExpectMultiplicativeExpressionInvalid){
    std::cout << "Testing invalid multiplicative expression:" << std::endl;
    checkInvalid("BEGIN a:= --adas END.");
    checkInvalid("BEGIN a:= a * (a * a + (super) END.");
    checkInvalid("BEGIN RETURN --adas END;");
    checkInvalid("BEGIN RETURN a * (a * a + (super) END.");
    std::cout << "=========================================================" << std::endl;
}

TEST(TestParser, ExpectAssignmentExpressionValid){
    auto result = setup("BEGIN a := a * (a * a + (super)) END.");
    EXPECT_NE(result, nullptr);
}
TEST(TestParser, ExpectAssignmentExpressionInvalid){
    std::cout << "Testing invalid assignment expression:" << std::endl;
    checkInvalid("BEGIN a := --adas END.");
    checkInvalid("BEGIN a := a * (a * a + (super) END.");
    checkInvalid("BEGIN a = 1 END.");
    checkInvalid("BEGIN 1234 := a * (a * a + (super)) END.");
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectStatementListValid){
    auto result = setup("BEGIN RETURN (a+b * 2/(123+4)) END.");
    EXPECT_NE(result, nullptr);
    auto visitor = parser::ParseTreePrintVisitor();
    visitor.printTree(*result);
    result = setup("BEGIN a:= 1; RETURN (a+b * 2/(123+4)) END.");
    EXPECT_NE(result, nullptr);
    result = setup("BEGIN a := 1; a := 1; RETURN (a+b * 2/(123+4))END.");
    EXPECT_NE(result, nullptr);
}
TEST(TestParser, ExpectStatementListInvalid){
    std::cout << "Testing invalid statement:" << std::endl;
    checkInvalid("BEGIN a = 1 END.");
    checkInvalid("BEGIN a := 1; END.");
    checkInvalid("BEGIN a := 1; a := 1; END.");
    checkInvalid("BEGIN a := 1, a := 1 END.");
    checkInvalid("BEGIN a := 1 a := 1 END.");
    checkInvalid("BEGIN a := 1 RETURN a END.");
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectComplexFunctionDefinitionValid){
    auto result = setup("PARAM width, height, depth;\nVAR volume;\nCONST density = 2400;\nBEGIN\nvolume :=width * height * depth;\nRETURN density*volume\nEND.");
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 5);
    //auto visitor = parser::ParseTreePrintVisitor();
    //visitor.printTree(*result);
}


