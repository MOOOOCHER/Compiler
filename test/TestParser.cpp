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
static auto setup(const std::string_view& input, auto (Parser::*func)(size_t&)){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);

    Parser parser = Parser(manager,tokenizer.getTokens());
    size_t currentPosition = 0;
    return (parser.*func)(currentPosition);
}
static void checkInvalid(const std::string& input, auto (Parser::*func)(size_t&)){
    auto result = setup(input,func);
    EXPECT_EQ(result, nullptr);
}

static void checkGenericNode(const std::string& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);

    Parser parser = Parser(manager,tokenizer.getTokens());
    size_t currentPosition = 0;
    auto result = parser.expectGenericNode(input,currentPosition,false);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getInformation(), input);
}
static void checkDeclaratorList(const std::string& input,const size_t expectedSize){
    auto result = setup(input, &Parser::expectDeclaratorList);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), expectedSize);
    EXPECT_EQ(result->getType(), Node::Types::DeclaratorList);
    for(size_t i=0;i<expectedSize; i++){
        if(i%2 == 0){
            EXPECT_EQ(result->getChildren()[i]->getType(), Node::Types::Identifier);
            parser::IdentifierNode* idNode = dynamic_cast<parser::IdentifierNode*>(result->getChildren()[i]);
            EXPECT_EQ(idNode->getInformation(), "identifier");
        } else {
            EXPECT_EQ(result->getChildren()[i]->getType(), Node::Types::Generic);
            parser::GenericNode* equalNode = dynamic_cast<parser::GenericNode*>(result->getChildren()[i]);
            EXPECT_EQ(equalNode->getInformation(), ",");
        }
    }
}
static void checkInitDeclaratorList(const std::string& input, size_t expectedSize){
    auto result = setup(input,&Parser::expectInitDeclaratorList);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), expectedSize);
    EXPECT_EQ(result->getType(), Node::Types::InitDeclaratorList);
    for(size_t i=0;i<expectedSize; i++){
        if(i%2 == 0){
            EXPECT_EQ(result->getChildren()[i]->getType(), Node::Types::InitDeclarator);
            parser::NonTerminalNode* idNode = dynamic_cast<parser::NonTerminalNode*>(result->getChildren()[i]);
            EXPECT_EQ(idNode->getType(), Node::Types::InitDeclarator);
        } else {
            EXPECT_EQ(result->getChildren()[i]->getType(), Node::Types::Generic);
            parser::GenericNode* equalNode = dynamic_cast<parser::GenericNode*>(result->getChildren()[i]);
            EXPECT_EQ(equalNode->getInformation(), ",");
        }
    }
}
static void checkStatementList(const std::string& input, size_t expectedSize){
    auto result = setup(input,&Parser::expectStatementList);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), expectedSize);
    EXPECT_EQ(result->getType(), Node::Types::StatementList);
    for(size_t i=0;i<expectedSize; i++){
        if(i%2 == 0){
            EXPECT_EQ(result->getChildren()[i]->getType(), Node::Types::Statement);
        } else {
            EXPECT_EQ(result->getChildren()[i]->getType(), Node::Types::Generic);
            parser::GenericNode* equalNode = dynamic_cast<parser::GenericNode*>(result->getChildren()[i]);
            EXPECT_EQ(equalNode->getInformation(), ";");
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
TEST(TestParser, ExpectIdentifierNodeValid){
    std::string_view view("hello");
    auto result = setup(view, &Parser::expectIdentifierNode);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::Identifier);
    EXPECT_EQ(result->getInformation(), view);
}
TEST(TestParser, ExpectIdentifierNodeInvalid){
    std::cout << "Testing invalid identifier:" << std::endl;
    checkInvalid("1234",  &Parser::expectIdentifierNode);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectLiteralNodeValid){
    std::string_view view("1234");
    auto result = setup(view, &Parser::expectLiteralNode);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::Literal);
    EXPECT_EQ(result->getInformation(), 1234);
}
TEST(TestParser, ExpectLiteralNodeInvalid){
    std::cout << "Testing invalid literal:" << std::endl;
    checkInvalid("hello",  &Parser::expectLiteralNode);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectGenericNodeValid){
    checkGenericNode(";");
    checkGenericNode(",");
    checkGenericNode(",");
    checkGenericNode("+");
    checkGenericNode("-");
    checkGenericNode("*");
    checkGenericNode("/");
    checkGenericNode("=");
    checkGenericNode(":=");
    checkGenericNode("PARAM");
    checkGenericNode("VAR");
    checkGenericNode("CONST");
    checkGenericNode("BEGIN");
    checkGenericNode("END");
    checkGenericNode("RETURN");
}
TEST(TestParser, ExpectGenericNodeInvalid){
    std::cout << "Testing invalid generic node:" << std::endl;
    std::string_view view(".");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);

    Parser parser = Parser(manager,tokenizer.getTokens());
    size_t currentPosition = 0;
    auto result = parser.expectGenericNode(";",currentPosition, false);
    EXPECT_EQ(result, nullptr);
    std::cout << "=========================================================" << std::endl;
}
//Non Terminal symbols -------------------------------------------------------------------------------------------
TEST(TestParser, ExpectInitDeclaratorValid){
    std::string_view view("identifier = 1");
    auto result = setup(view, &Parser::expectInitDeclarator);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::InitDeclarator);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Identifier);
    parser::IdentifierNode* idNode = dynamic_cast<parser::IdentifierNode*>(result->getChildren()[0]);
    EXPECT_EQ(idNode->getInformation(), "identifier");
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::Generic);
    parser::GenericNode* equalNode = dynamic_cast<parser::GenericNode*>(result->getChildren()[1]);
    EXPECT_EQ(equalNode->getInformation(), "=");
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::Literal);
    parser::LiteralNode* literal = dynamic_cast<parser::LiteralNode*>(result->getChildren()[2]);
    EXPECT_EQ(literal->getInformation(), 1);
}
TEST(TestParser, ExpectInitDeclaratorInvalid){
    std::cout << "Testing invalid init declarator:" << std::endl;
    checkInvalid("identifier =", &Parser::expectInitDeclarator);
    checkInvalid("identifier = init", &Parser::expectInitDeclarator);
    checkInvalid("12 = 1234", &Parser::expectInitDeclarator);
    checkInvalid("identifier 123", &Parser::expectInitDeclarator);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectDeclaratorListValid){
    checkDeclaratorList("identifier",1);
    checkDeclaratorList("identifier, identifier",3);
    checkDeclaratorList("identifier, identifier, identifier",5);
}
TEST(TestParser, ExpectDeclaratorListInvalid){
    std::cout << "Testing invalid declarator list:" << std::endl;
    checkInvalid("identifier,",  &Parser::expectDeclaratorList);
    checkInvalid("identifier, identifier,",  &Parser::expectDeclaratorList);
    checkInvalid("identifier, 1234, identifier",  &Parser::expectDeclaratorList);
    checkInvalid("identifier identifier",  &Parser::expectDeclaratorList);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectInitDeclaratorListValid){
    checkInitDeclaratorList("identifier = 1",1);
    checkInitDeclaratorList("identifier = 1, identifier = 2",3);
    checkInitDeclaratorList("identifier = 1, identifier = 2, identifier = 3",5);
}
TEST(TestParser, ExpectInitDeclaratorListInvalid){
    std::cout << "Testing invalid init declarator list:" << std::endl;
    checkInvalid("identifier = 1,",  &Parser::expectInitDeclaratorList);
    checkInvalid("identifier = 1, identifier = 1,",  &Parser::expectInitDeclaratorList);
    checkInvalid("identifier = 1, 1234, identifier = 1",  &Parser::expectInitDeclaratorList);
    checkInvalid("identifier = 1, 1234 = 1,",  &Parser::expectInitDeclaratorList);
    checkInvalid("identifier = 1 identifier = 1",  &Parser::expectInitDeclaratorList);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectParamDeclarationValid){
    auto result = setup("PARAM a;", &Parser::expectParameterDeclaration);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::DeclaratorList);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::Generic);
    result = setup("PARAM a,b;", &Parser::expectParameterDeclaration);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::DeclaratorList);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::Generic);
}
TEST(TestParser, ExpectParamDeclarationInvalid){
    std::cout << "Testing invalid init parameter declarations:" << std::endl;
    checkInvalid("PARAM a",  &Parser::expectParameterDeclaration);
    checkInvalid("PARA a,b;",  &Parser::expectParameterDeclaration);
    checkInvalid("PARAM a b;",  &Parser::expectParameterDeclaration);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectVariableDeclarationValid){
    auto result = setup("VAR a;", &Parser::expectVariableDeclaration);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::DeclaratorList);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::Generic);
    result = setup("VAR a,b;", &Parser::expectVariableDeclaration);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::DeclaratorList);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::Generic);
}
TEST(TestParser, ExpectVariableDeclarationInvalid){
    std::cout << "Testing invalid init variable declarations:" << std::endl;
    checkInvalid("VAR a",  &Parser::expectVariableDeclaration);
    checkInvalid("VA a,b;",  &Parser::expectVariableDeclaration);
    checkInvalid("VAR a b;",  &Parser::expectVariableDeclaration);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectConstantDeclarationValid){
    auto result = setup("CONST a=1;", &Parser::expectConstantDeclaration);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::InitDeclaratorList);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::Generic);
    result = setup("CONST a=1,b=2;", &Parser::expectConstantDeclaration);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::InitDeclaratorList);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::Generic);
}
TEST(TestParser, ExpectConstantDeclarationInvalid){
    std::cout << "Testing invalid init constant declarations:" << std::endl;
    checkInvalid("CONST a=5",  &Parser::expectConstantDeclaration);
    checkInvalid("CONST a;",  &Parser::expectConstantDeclaration);
    checkInvalid("CONS a = 4,b = 5;",  &Parser::expectConstantDeclaration);
    checkInvalid("CONST a=4 b=5;",  &Parser::expectConstantDeclaration);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectPrimaryExpressionValid){
    auto result = setup("identifier",&Parser::expectPrimaryExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 1);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Identifier);
    result = setup("1234",&Parser::expectPrimaryExpression);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 1);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Literal);
    result = setup("(1234)",&Parser::expectPrimaryExpression);
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
    std::cout << "=========================================================" << std::endl;
}
TEST(TestParser, ExpectCompoundStatementValid){
    auto result = setup("BEGIN a := 1 END", &Parser::expectCompoundStatement);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::StatementList);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::Generic);
    result = setup("BEGIN a := 1; RETURN a+a END", &Parser::expectCompoundStatement);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getChildren().size(), 3);
    EXPECT_EQ(result->getChildren()[0]->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getChildren()[1]->getType(), Node::Types::StatementList);
    EXPECT_EQ(result->getChildren()[2]->getType(), Node::Types::Generic);
}
TEST(TestParser, ExpectCompoundStatementInvalid){
    std::cout << "Testing invalid compound statement:" << std::endl;
    checkInvalid("BEGIN a = 1 END", &Parser::expectCompoundStatement);
    checkInvalid("a := 1 END;", &Parser::expectCompoundStatement);
    checkInvalid("BEGIN a := 1; a := 1", &Parser::expectCompoundStatement);
    std::cout << "=========================================================" << std::endl;
}



