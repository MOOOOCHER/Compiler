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
    std::string_view view(";");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);

    Parser parser = Parser(manager);
    size_t currentPosition = 0;
    auto result = parser.expectGenericNode(";",currentPosition,tokenizer.getTokens());
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), Node::Types::Generic);
    EXPECT_EQ(result->getInformation(), ";");
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