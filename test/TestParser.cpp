#include "pljit/Milestone1/SourceCodeManager.h"
#include "pljit/Milestone2/Token.h"
#include "pljit/Milestone3/Parser.h"
#include "pljit/Milestone3/Node.h"
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
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::Generic);
    parser::GenericNode* genericNode = dynamic_cast<parser::GenericNode*>(result->getChildren()[1]);
    EXPECT_EQ(genericNode->getInformation(), ".");
    result = setup("BEGIN RETURN 1 END.");
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::FunctionDefinition);
    EXPECT_EQ(result->getChildren().size(), 2);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::CompoundStatement);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::Generic);
}
TEST(TestParser, ExpectCompoundStatementInvalid){
    std::cout << "Testing invalid compound statement declarator:" << std::endl;
    checkInvalid("RETURN 1 END.");
    checkInvalid("BEGIN RETURN 1 .");
    checkInvalid("CONST RETURN 1 END.");
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
    EXPECT_EQ(declListChild->getChildren()[1]->getType(), Node::Types::Generic);
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
}
TEST(TestParser, ExpectConstantDeclarationInvalid){
    checkInvalid("CONST a=1 BEGIN a:=1 END.");
    checkInvalid("CONST a 1; BEGIN a:=1 END.");
    checkInvalid("CONST a=a, b=2;BEGIN a:=1 END.");
    checkInvalid("CONST a=1 b=2;BEGIN a:=1 END.");
    checkInvalid("CONST a=a, b=2;BEGIN a:=1 END.");
}
/*

 TEST(TestParser, ExpectPrimaryExpressionValid){
   auto result = setup("identifier");
   EXPECT_NE(result, nullptr);
   EXPECT_EQ(result->getChildren().size(), 1);
   EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Identifier);
   result = setup("1234");
   EXPECT_NE(result, nullptr);
   EXPECT_EQ(result->getChildren().size(), 1);
   EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Literal);
   result = setup("(1234)");
   EXPECT_NE(result, nullptr);
   EXPECT_EQ(result->getChildren().size(), 3);
   EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
   EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::AdditiveExpression);
   EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::Generic);
}
TEST(TestParser, ExpectPrimaryExpressionInvalid){
    std::cout << "Testing invalid primary expression:" << std::endl;
    checkInvalid("RETURN", &Parser::expectPrimaryExpression);
    checkInvalid("(1234", &Parser::expectPrimaryExpression);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectUnaryExpressionValid){
    auto result = setup("+ identifier",&Parser::expectUnaryExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 2);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::PrimaryExpression);

    result = setup("-1234",&Parser::expectUnaryExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 2);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::PrimaryExpression);

    result = setup("(1234)",&Parser::expectUnaryExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 1);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::PrimaryExpression);
}
TEST(TestParser, ExpectUnaryExpressionInvalid){
    std::cout << "Testing invalid unary expression:" << std::endl;
    checkInvalid("+RETURN", &Parser::expectUnaryExpression);
    checkInvalid("-(identifier", &Parser::expectUnaryExpression);
    checkInvalid("+ (1234", &Parser::expectUnaryExpression);
    checkInvalid("-- (1234", &Parser::expectUnaryExpression);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectMultiplicativeExpressionValid){
    auto result = setup("+a",&Parser::expectMultiplicativeExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 1);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::UnaryExpression);

    result = setup("a * a * a",&Parser::expectMultiplicativeExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::UnaryExpression);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::MultiplicativeExpression);

    result = setup("a * (a + a * (super))",&Parser::expectMultiplicativeExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::UnaryExpression);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::MultiplicativeExpression);
}
TEST(TestParser, ExpectMultiplicativeExpressionInvalid){
    std::cout << "Testing invalid multiplicative expression:" << std::endl;
    checkInvalid("--adas", &Parser::expectMultiplicativeExpression);
    checkInvalid("a * (a * a + (super)", &Parser::expectMultiplicativeExpression);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectAdditiveExpressionValid){
    auto result = setup("a + a - b",&Parser::expectAdditiveExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::MultiplicativeExpression);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::AdditiveExpression);
    result = setup("a * a",&Parser::expectAdditiveExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 1);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::MultiplicativeExpression);
}
TEST(TestParser, ExpectAdditiveExpressionInvalid){
    std::cout << "Testing invalid additive expression:" << std::endl;
    checkInvalid("a + a +*b",&Parser::expectAdditiveExpression);
    checkInvalid("((a + a) -b",&Parser::expectAdditiveExpression);
    checkInvalid("a + a * b -1234+",&Parser::expectAdditiveExpression);
    std::cout << "=========================================================" << std::endl;
}

TEST(TestParser, ExpectAssignmentExpressionValid){
    auto result = setup("a := a * (a * a + (super))",&Parser::expectAssignmentExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Identifier);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::AdditiveExpression);
    result = setup("a := 1234",&Parser::expectAssignmentExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Identifier);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::AdditiveExpression);
}
TEST(TestParser, ExpectAssignmentExpressionInvalid){
    std::cout << "Testing invalid assignment expression:" << std::endl;
    checkInvalid(" a := --adas", &Parser::expectAssignmentExpression);
    checkInvalid("a := a * (a * a + (super)", &Parser::expectAssignmentExpression);
    checkInvalid(" a = 1", &Parser::expectAssignmentExpression);
    checkInvalid("1234 := a * (a * a + (super))", &Parser::expectAssignmentExpression);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectStatementValid){
    auto result = setup("a := 1",&Parser::expectStatement);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 1);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::AssignmentExpression);
    result = setup("RETURN (a+b * 2/(123+4))",&Parser::expectStatement);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 2);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::AdditiveExpression);
}
TEST(TestParser, ExpectStatementInvalid){
    std::cout << "Testing invalid statement:" << std::endl;
    checkInvalid(" a := --adas", &Parser::expectStatement);
    checkInvalid("a := a * (a * a + (super)", &Parser::expectStatement);
    checkInvalid(" RETURN a + a +*b", &Parser::expectStatement);
    checkInvalid(" BEGIN a + a +*b", &Parser::expectStatement);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectStatementListValid){
    checkStatementList("a := 1", 1);
    checkStatementList("RETURN (a+b * 2/(123+4))", 1);
    checkStatementList("a := 1; RETURN (a+b * 2/(123+4))", 3);
    checkStatementList("a := 1; a := 1; RETURN (a+b * 2/(123+4))", 5);
}
TEST(TestParser, ExpectStatementListInvalid){
    std::cout << "Testing invalid statement:" << std::endl;
    checkInvalid(" a = 1", &Parser::expectStatementList);
    checkInvalid("a := 1;", &Parser::expectStatementList);
    checkInvalid(" a := 1; a := 1;", &Parser::expectStatementList);
    checkInvalid(" a := 1, a := 1", &Parser::expectStatementList);
    checkInvalid(" a := 1 a := 1", &Parser::expectStatementList);
    checkInvalid(" a := 1 RETURN a", &Parser::expectStatementList);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectFunctionDefinitionValid){
    auto result = setup("BEGIN a := 1 END.", &Parser::expectFunctionDefinition);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 2);
    result = setup("CONST c= 0; BEGIN a := 1 END.", &Parser::expectFunctionDefinition);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    result = setup("VAR b; CONST c= 0; BEGIN a := 1 END.", &Parser::expectFunctionDefinition);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 4);
    result = setup("PARAM width, height, depth;\nVAR volume;\nCONST density = 2400;\nBEGIN\nvolume :=width * height * depth;\nRETURN density*volume\nEND.", &Parser::expectFunctionDefinition);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 5);
}
*/


