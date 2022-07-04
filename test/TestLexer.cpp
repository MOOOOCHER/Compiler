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

    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    EXPECT_EQ(tokenizer.getTokens().size(), 3);
    EXPECT_EQ(tokenizer.getTokens()[0].getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(tokenizer.getTokens()[0].getText(), "PARAM");
    EXPECT_EQ(tokenizer.getTokens()[1].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[1].getText(), "a");
    EXPECT_EQ(tokenizer.getTokens()[2].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[2].getText(), "b");
}
TEST(TestLexer, twoLiterates){
    std::string_view view("BEGIN 12 34 END;");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    EXPECT_EQ(tokenizer.getTokens().size(), 5);
    EXPECT_EQ(tokenizer.getTokens()[0].getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(tokenizer.getTokens()[0].getText(), "BEGIN");
    EXPECT_EQ(tokenizer.getTokens()[1].getType(), lexer::TokenTypes::Literal);
    EXPECT_EQ(tokenizer.getTokens()[1].getText(), "12");
    EXPECT_EQ(tokenizer.getTokens()[2].getType(), lexer::TokenTypes::Literal);
    EXPECT_EQ(tokenizer.getTokens()[2].getText(), "34");
    EXPECT_EQ(tokenizer.getTokens()[3].getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(tokenizer.getTokens()[3].getText(), "END");
    EXPECT_EQ(tokenizer.getTokens()[4].getType(), lexer::TokenTypes::Separator);
    EXPECT_EQ(tokenizer.getTokens()[4].getText(), ";");
}
TEST(TestLexer, LiteralIdentifier){
    std::string_view view("BEGIN VAR b 1234a END;");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    EXPECT_EQ(tokenizer.getTokens().size(), 7);
    EXPECT_EQ(tokenizer.getTokens()[0].getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(tokenizer.getTokens()[0].getText(), "BEGIN");
    EXPECT_EQ(tokenizer.getTokens()[1].getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(tokenizer.getTokens()[1].getText(), "VAR");
    EXPECT_EQ(tokenizer.getTokens()[2].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[2].getText(), "b");
    EXPECT_EQ(tokenizer.getTokens()[3].getType(), lexer::TokenTypes::Literal);
    EXPECT_EQ(tokenizer.getTokens()[3].getText(), "1234");
    EXPECT_EQ(tokenizer.getTokens()[4].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[4].getText(), "a");
    EXPECT_EQ(tokenizer.getTokens()[5].getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(tokenizer.getTokens()[5].getText(), "END");
    EXPECT_EQ(tokenizer.getTokens()[6].getType(), lexer::TokenTypes::Separator);
    EXPECT_EQ(tokenizer.getTokens()[6].getText(), ";");
}
TEST(TestLexer, InvalidSymbol) {
    std::string_view view("CONST hello? = 4*4.");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    EXPECT_EQ(tokenizer.getTokens().size(), 0);
}
TEST(TestLexer, InvalidSymbol2) {
    std::string_view view("VAR hello:bye.");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    EXPECT_EQ(tokenizer.getTokens().size(), 0);
}
TEST(TestLexer, ParamLexer){
    std::string_view view("PARAM wid, hei, dep;");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    EXPECT_EQ(tokenizer.getTokens().size(), 7);
    EXPECT_EQ(tokenizer.getTokens()[0].getType(), lexer::TokenTypes::Keyword);
    EXPECT_EQ(tokenizer.getTokens()[0].getText(), "PARAM");
    EXPECT_EQ(tokenizer.getTokens()[1].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[1].getText(), "wid");
    EXPECT_EQ(tokenizer.getTokens()[2].getType(), lexer::TokenTypes::Separator);
    EXPECT_EQ(tokenizer.getTokens()[2].getText(), ",");
    EXPECT_EQ(tokenizer.getTokens()[3].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[3].getText(), "hei");
    EXPECT_EQ(tokenizer.getTokens()[4].getType(), lexer::TokenTypes::Separator);
    EXPECT_EQ(tokenizer.getTokens()[4].getText(), ",");
    EXPECT_EQ(tokenizer.getTokens()[5].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[5].getText(), "dep");
    EXPECT_EQ(tokenizer.getTokens()[6].getType(), lexer::TokenTypes::Separator);
    EXPECT_EQ(tokenizer.getTokens()[6].getText(), ";");

}
TEST(TestLexer, Operator){
    std::string_view view("volume :=width * height / depth;");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    EXPECT_EQ(tokenizer.getTokens().size(), 8);
    EXPECT_EQ(tokenizer.getTokens()[0].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[0].getText(), "volume");
    EXPECT_EQ(tokenizer.getTokens()[1].getType(), lexer::TokenTypes::Operator);
    EXPECT_EQ(tokenizer.getTokens()[1].getText(), ":=");
    EXPECT_EQ(tokenizer.getTokens()[2].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[2].getText(), "width");
    EXPECT_EQ(tokenizer.getTokens()[3].getType(), lexer::TokenTypes::Operator);
    EXPECT_EQ(tokenizer.getTokens()[3].getText(), "*");
    EXPECT_EQ(tokenizer.getTokens()[4].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[4].getText(), "height");
    EXPECT_EQ(tokenizer.getTokens()[5].getType(), lexer::TokenTypes::Operator);
    EXPECT_EQ(tokenizer.getTokens()[5].getText(), "/");
    EXPECT_EQ(tokenizer.getTokens()[6].getType(), lexer::TokenTypes::Identifier);
    EXPECT_EQ(tokenizer.getTokens()[6].getText(), "depth");
    EXPECT_EQ(tokenizer.getTokens()[7].getType(), lexer::TokenTypes::Separator);
    EXPECT_EQ(tokenizer.getTokens()[7].getText(), ";");
}
TEST(TestLexer, ComplexLexer){
    std::string_view view("PARAM width, height, depth;\nVAR volume;\nCONST density = 2400;\nBEGIN\nvolume :=width * height * depth;\nRETURN density*volume\nEND.");
    SourceCodeManager manager(view);

    Tokenizer tokenizer = Tokenizer();
    tokenizer.parse(manager);
    EXPECT_EQ(tokenizer.getTokens().size(), 7+3+5+1+8+4+2);

}
