#include "pljit/Milestone1/SourceCodeManager.h"
#include "pljit/Milestone2/Token.h"
#include "pljit/Milestone3/Parser.h"
#include "pljit/Milestone3/Node.h"
#include <gtest/gtest.h>

using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
using Token = lexer::Token;
using Tokenizer = lexer::Tokenizer;
using Parser = parser::Parser;
using Node = parser::Node;

static void checkInvalid(const std::string& input, auto (*func)(size_t, std::vector<Token>&)){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    Parser parser = Parser(manager);
    size_t currentPosition = 0;

    auto result = func(currentPosition,tokenizer.getTokens());
    EXPECT_EQ(result, nullptr);
}
static void checkInitDeclarationInvalid(const std::string& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    Parser parser = Parser(manager);
    size_t currentPosition = 0;

    auto result = parser.expectInitDeclarator(currentPosition, tokenizer.getTokens());
    EXPECT_EQ(result, nullptr);
}
static void checkGenericNode(const std::string& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);

    Parser parser = Parser(manager);
    size_t currentPosition = 0;
    auto result = parser.expectGenericNode(input,currentPosition,tokenizer.getTokens());
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getInformation(), input);
}
static void checkDeclaratorList(const std::string& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    Parser parser = Parser(manager);
    size_t currentPosition = 0;
    size_t expectedSize = tokenizer.getTokens().size();

    auto result = parser.expectDeclaratorList(currentPosition,tokenizer.getTokens());
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
static void checkDeclaratorListInvalid(const std::string& input){
    SourceCodeManager manager(input);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    Parser parser = Parser(manager);
    size_t currentPosition = 0;

    auto result = parser.expectDeclaratorList(currentPosition,tokenizer.getTokens());
    EXPECT_EQ(result, nullptr);
}

TEST(TestLexer, ExpectIdentifierNodeValid){
    std::string_view view("hello");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);

    Parser parser = Parser(manager);
    size_t currentPosition = 0;
    auto result = parser.expectIdentifierNode(currentPosition,tokenizer.getTokens());
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::Identifier);
    EXPECT_EQ(result->getInformation(), view);
}
TEST(TestLexer, ExpectIdentifierNodeInvalid){
    std::string_view view("1234");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);

    Parser parser = Parser(manager);
    size_t currentPosition = 0;
    auto result = parser.expectIdentifierNode(currentPosition,tokenizer.getTokens());
    EXPECT_EQ(result, nullptr);
}
TEST(TestLexer, ExpectLiteralNodeValid){
    std::string_view view("1234");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);

    Parser parser = Parser(manager);
    size_t currentPosition = 0;
    auto result = parser.expectLiteralNode(currentPosition,tokenizer.getTokens());
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::Literal);
    EXPECT_EQ(result->getInformation(), 1234);
}
TEST(TestLexer, ExpectLiteralNodeInvalid){
    std::string_view view("hello");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);

    Parser parser = Parser(manager);
    size_t currentPosition = 0;
    auto result = parser.expectLiteralNode(currentPosition,tokenizer.getTokens());
    EXPECT_EQ(result, nullptr);
}
TEST(TestLexer, ExpectGenericNodeValid){
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
TEST(TestLexer, ExpectGenericNodeInvalid){
    std::string_view view(".");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);

    Parser parser = Parser(manager);
    size_t currentPosition = 0;
    auto result = parser.expectGenericNode(";",currentPosition,tokenizer.getTokens());
    EXPECT_EQ(result, nullptr);
}
//Non Terminal symbols -------------------------------------------------------------------------------------------
TEST(TestLexer, ExpectDeclaratorListValid){
    checkDeclaratorList("identifier");
    checkDeclaratorList("identifier, identifier");
    checkDeclaratorList("identifier, identifier, identifier");
}
TEST(TestLexer, ExpectDeclaratorListInvalid){
    checkDeclaratorListInvalid("identifier,");
    checkDeclaratorListInvalid("identifier, identifier,");
    checkDeclaratorListInvalid("identifier, 1234, identifier");
}

TEST(TestLexer, ExpectInitDeclaratorValid){
    std::string_view view("identifier = 1");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    Parser parser = Parser(manager);
    size_t currentPosition = 0;

    auto result = parser.expectInitDeclarator(currentPosition,tokenizer.getTokens());
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
TEST(TestLexer, ExpectInitDeclaratorInvalid){
    checkInitDeclarationInvalid("identifier =");
    checkInitDeclarationInvalid("identifier = init");
    checkInitDeclarationInvalid("12 = 1234");
    checkInitDeclarationInvalid("");
}

