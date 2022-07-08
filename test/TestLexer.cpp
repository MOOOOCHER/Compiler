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
    SourceCodeReference range = SourceCodeReference(view.data(),manager);
    range.printContext("Testing Range",5);
    SourceCodeReference location = SourceCodeReference(view.data(),manager);
    location.printContext("Testing Location", 1);
}
TEST(TestLexer, SimpleTextLexer){
    std::string_view view("PARAM a b");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(token.getText(), "PARAM");
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(token2.getText(), "a");
    auto token3 = tokenizer.next();
    EXPECT_EQ(token3.getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(token3.getText(), "b");
}
TEST(TestLexer, twoLiterates){
    std::string_view view("BEGIN 12 34 END;");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(token.getText(), "BEGIN");
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::Literal);
    EXPECT_EQ(token2.getText(), "12");
    auto token3 = tokenizer.next();
    EXPECT_EQ(token3.getType(), lexer::TokenTypes::Literal);
    EXPECT_EQ(token3.getText(), "34");
    auto token4 = tokenizer.next();
    EXPECT_EQ(token4.getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(token4.getText(), "END");
    auto token5 = tokenizer.next();
    EXPECT_EQ(token5.getType(), lexer::TokenTypes::Separator);
    EXPECT_EQ(token5.getText(), ";");
}
TEST(TestLexer, LiteralIdentifier){
    std::string_view view("BEGIN VAR b 1234a END;");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(token.getText(), "BEGIN");
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(token2.getText(), "VAR");
    auto token3 = tokenizer.next();
    EXPECT_EQ(token3.getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(token3.getText(), "b");
    auto token4 = tokenizer.next();
    EXPECT_EQ(token4.getType(), lexer::TokenTypes::Literal);
    EXPECT_EQ(token4.getText(), "1234");
    auto token5 = tokenizer.next();
    EXPECT_EQ(token5.getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(token5.getText(), "a");
    auto token6 = tokenizer.next();
    EXPECT_EQ(token6.getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(token6.getText(), "END");
    auto token7 = tokenizer.next();
    EXPECT_EQ(token7.getType(), lexer::TokenTypes::Separator);
    EXPECT_EQ(token7.getText(), ";");
}
TEST(TestLexer, InvalidSymbol) {
    std::string_view view("CONST hello? = 4*4.");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(token.getText(), "CONST");
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::Invalid);

}
TEST(TestLexer, InvalidSymbol2) {
    std::string_view view("VAR hello_bye.");
    SourceCodeManager manager(view);
    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(token.getText(), "VAR");
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::Invalid);
}
TEST(TestLexer, ParamLexer){
    std::string_view view("PARAM wid, hei, dep;");
    SourceCodeManager manager(view);
        Tokenizer tokenizer = Tokenizer(manager);
        auto token = tokenizer.next();
        EXPECT_EQ(token.getType(), lexer::TokenTypes::Keyword);
        EXPECT_EQ(token.getText(), "PARAM");
        auto token2 = tokenizer.next();
        EXPECT_EQ(token2.getType(), lexer::TokenTypes::Identifier);
        EXPECT_EQ(token2.getText(), "wid");
        auto token3 = tokenizer.next();
        EXPECT_EQ(token3.getType(), lexer::TokenTypes::Separator);
        EXPECT_EQ(token3.getText(), ",");
        auto token4 = tokenizer.next();
        EXPECT_EQ(token4.getType(), lexer::TokenTypes::Identifier);
        EXPECT_EQ(token4.getText(), "hei");
        auto token5 = tokenizer.next();
        EXPECT_EQ(token5.getType(), lexer::TokenTypes::Separator);
        EXPECT_EQ(token5.getText(), ",");
        auto token6 = tokenizer.next();
        EXPECT_EQ(token6.getType(), lexer::TokenTypes::Identifier);
        EXPECT_EQ(token6.getText(), "dep");
        auto token7 = tokenizer.next();
        EXPECT_EQ(token7.getType(), lexer::TokenTypes::Separator);
        EXPECT_EQ(token7.getText(), ";");
}
TEST(TestLexer, Operator){
    std::string_view view("volume :=width * height / depth;");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer(manager);
    auto token = tokenizer.next();
    EXPECT_EQ(token.getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(token.getText(), "volume");
    auto token2 = tokenizer.next();
    EXPECT_EQ(token2.getType(), lexer::TokenTypes::Operator);
    EXPECT_EQ(token2.getText(), ":=");
    auto token3 = tokenizer.next();
    EXPECT_EQ(token3.getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(token3.getText(), "width");
    auto token4 = tokenizer.next();
    EXPECT_EQ(token4.getType(), lexer::TokenTypes::Operator);
    EXPECT_EQ(token4.getText(), "*");
    auto token5 = tokenizer.next();
    EXPECT_EQ(token5.getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(token5.getText(), "height");
    auto token6 = tokenizer.next();
    EXPECT_EQ(token6.getType(), lexer::TokenTypes::Operator);
    EXPECT_EQ(token6.getText(), "/");
    auto token7 = tokenizer.next();
    EXPECT_EQ(token7.getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(token7.getText(), "depth");
    auto token8 = tokenizer.next();
    EXPECT_EQ(token8.getType(), lexer::TokenTypes::Separator);
    EXPECT_EQ(token8.getText(), ";");
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
