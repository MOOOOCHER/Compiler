#include "pljit/Milestone1/SourceCodeManager.h"
#include "pljit/Milestone2/Token.h"
#include <gtest/gtest.h>

using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
using Tokenizer = lexer::Tokenizer;
using SourceCodeReference = sourceCodeManagement::SourceCodeReference;

TEST(TestLexer, PrintContext){
    std::string_view view("PARAM a b;");
    SourceCodeManager manager(view);
    SourceCodeReference range = SourceCodeReference(0,manager,5);
    range.printContext("Testing Range");
    SourceCodeReference location = SourceCodeReference(0,manager, 1);
    location.printContext("Testing Location");
}
TEST(TestLexer, SimpleTextLexer){
    std::string_view view("PARAM a b");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::PARAM);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
}
TEST(TestLexer, twoLiterates){
    std::string_view view("BEGIN 12 34 END;");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::BEGIN);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Literal);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Literal);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::END);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Semicolon);
}
TEST(TestLexer, LiteralIdentifier){
    std::string_view view("BEGIN VAR b 1234a END;");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::BEGIN);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::VAR);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Literal);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::END);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Semicolon);
}
TEST(TestLexer, InvalidSymbol) {
    std::string_view view("CONST hello? = 4*4.");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::CONST);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Invalid);

}
TEST(TestLexer, InvalidSymbol2) {
    std::string_view view("VAR hello_bye.");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::VAR);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Invalid);
}
TEST(TestLexer, InvalidSymbol3) {
    std::string_view view("VAR hello bye?");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::VAR);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Invalid); //? is seen as part of bye
}
TEST(TestLexer, ParamLexer){
    std::string_view view("PARAM wid, hei, dep;");
    SourceCodeManager manager(view);
        Tokenizer tokenizer = Tokenizer(manager);
        auto token = tokenizer.next();
        EXPECT_EQ(token.getType(), lexer::TokenTypes::PARAM);
        token = tokenizer.next();
        EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
        token = tokenizer.next();
        EXPECT_EQ(token.getType(), lexer::TokenTypes::Comma);
        token = tokenizer.next();
        EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
        token = tokenizer.next();
        EXPECT_EQ(token.getType(), lexer::TokenTypes::Comma);
        token = tokenizer.next();
        EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
        token = tokenizer.next();
        EXPECT_EQ(token.getType(), lexer::TokenTypes::Semicolon);
}
TEST(TestLexer, Operator){
    std::string_view view("volume :=width * height / depth;");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::AssignEquals);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::MulOperator);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::DivOperator);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
    token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Semicolon);
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
    std::cout << "=========================================================" << std::endl;
}
