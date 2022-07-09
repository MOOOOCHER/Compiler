#include "pljit/Milestone1/SourceCodeManager.h"
#include "pljit/Milestone2/Token.h"
#include <gtest/gtest.h>

using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
using Token = lexer::Token;
using Tokenizer = lexer::Tokenizer;
using SourceCodeReference = sourceCodeManagement::SourceCodeReference;

TEST(TestLexer, PrintContext){
    std::string_view view("PARAM a b;");
    SourceCodeManager manager(view);
    SourceCodeReference range = SourceCodeReference(view.data(),manager,5);
    range.printContext("Testing Range");
    SourceCodeReference location = SourceCodeReference(view.data(),manager, 1);
    location.printContext("Testing Location");
}
TEST(TestLexer, SimpleTextLexer){
    std::string_view view("PARAM a b");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::PARAM);
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::Identifier);
    auto token3 = tokenizer.next();
    EXPECT_EQ(token3.getType(), lexer::TokenTypes::Identifier);
}
TEST(TestLexer, twoLiterates){
    std::string_view view("BEGIN 12 34 END;");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::BEGIN);
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::Literal);
    auto token3 = tokenizer.next();
    EXPECT_EQ(token3.getType(), lexer::TokenTypes::Literal);
    auto token4 = tokenizer.next();
    EXPECT_EQ(token4.getType(), lexer::TokenTypes::END);
    auto token5 = tokenizer.next();
    EXPECT_EQ(token5.getType(), lexer::TokenTypes::Semicolon);
}
TEST(TestLexer, LiteralIdentifier){
    std::string_view view("BEGIN VAR b 1234a END;");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::BEGIN);
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::VAR);
    auto token3 = tokenizer.next();
    EXPECT_EQ(token3.getType(), lexer::TokenTypes::Identifier);
    auto token4 = tokenizer.next();
    EXPECT_EQ(token4.getType(), lexer::TokenTypes::Literal);
    auto token5 = tokenizer.next();
    EXPECT_EQ(token5.getType(), lexer::TokenTypes::Identifier);
    auto token6 = tokenizer.next();
    EXPECT_EQ(token6.getType(), lexer::TokenTypes::END);
    auto token7 = tokenizer.next();
    EXPECT_EQ(token7.getType(), lexer::TokenTypes::Semicolon);
}
TEST(TestLexer, InvalidSymbol) {
    std::string_view view("CONST hello? = 4*4.");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::CONST);
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::Invalid);

}
TEST(TestLexer, InvalidSymbol2) {
    std::string_view view("VAR hello_bye.");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::VAR);
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::Invalid);
}
TEST(TestLexer, ParamLexer){
    std::string_view view("PARAM wid, hei, dep;");
    SourceCodeManager manager(view);
        Tokenizer tokenizer = Tokenizer(manager);
        auto token = tokenizer.next();
        EXPECT_EQ(token.getType(), lexer::TokenTypes::PARAM);
        auto token2 = tokenizer.next();
        EXPECT_EQ(token2.getType(), lexer::TokenTypes::Identifier);
        auto token3 = tokenizer.next();
        EXPECT_EQ(token3.getType(), lexer::TokenTypes::Comma);
        auto token4 = tokenizer.next();
        EXPECT_EQ(token4.getType(), lexer::TokenTypes::Identifier);
        auto token5 = tokenizer.next();
        EXPECT_EQ(token5.getType(), lexer::TokenTypes::Comma);
        auto token6 = tokenizer.next();
        EXPECT_EQ(token6.getType(), lexer::TokenTypes::Identifier);
        auto token7 = tokenizer.next();
        EXPECT_EQ(token7.getType(), lexer::TokenTypes::Semicolon);
}
TEST(TestLexer, Operator){
    std::string_view view("volume :=width * height / depth;");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::AssignEquals);
    auto token3 = tokenizer.next();
    EXPECT_EQ(token3.getType(), lexer::TokenTypes::Identifier);
    auto token4 = tokenizer.next();
    EXPECT_EQ(token4.getType(), lexer::TokenTypes::MulOperator);
    auto token5 = tokenizer.next();
    EXPECT_EQ(token5.getType(), lexer::TokenTypes::Identifier);
    auto token6 = tokenizer.next();
    EXPECT_EQ(token6.getType(), lexer::TokenTypes::DivOperator);
    auto token7 = tokenizer.next();
    EXPECT_EQ(token7.getType(), lexer::TokenTypes::Identifier);
    auto token8 = tokenizer.next();
    EXPECT_EQ(token8.getType(), lexer::TokenTypes::Semicolon);
}
TEST(TestLexer, ComplexLexer){
    std::string_view view("PARAM width, height, depth;\nVAR volume;\nCONST density = 2400;\nBEGIN\nvolume :=width * height * depth;\nRETURN density*volume\nEND.");
    SourceCodeManager manager(view);
    size_t iterations = 0;
    Tokenizer tokenizer = Tokenizer(manager);
    while(tokenizer.hasNext()){
        tokenizer.next();
        ++iterations;
    }
    EXPECT_EQ(iterations, 7+3+5+1+8+4+2);

}
