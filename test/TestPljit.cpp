#include "pljit/Milestone6/Pljit.h"
#include <gtest/gtest.h>

using Pljit = pljit::Pljit;
template<std::integral ...Args>
static void testValidCode(Pljit& jit, std::string_view input, double expectedResult, Args... args){
    auto func = jit.registerFunction(input);
    auto result = func(args...);
    ASSERT_EQ(result.has_value(), true);
    EXPECT_EQ(result.value(), expectedResult);
}

template<std::integral ...Args>
static void testInvalidCode(Pljit& jit, std::string_view input, Args... args){
    auto func = jit.registerFunction(input);
    auto result = func(args...);
    EXPECT_EQ(result.has_value(), false);
}
TEST(TestPljit, TestValidCode){
    Pljit jit;
    testValidCode(jit,"VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab END.",1);

    //calling twice
    auto func2 = jit.registerFunction("PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.");
    auto result = func2(50,2);
    ASSERT_EQ(result.has_value(), true);
    EXPECT_EQ(result.value(), 53);
    result = func2(1,2);
    ASSERT_EQ(result.has_value(), true);
    EXPECT_EQ(result.value(), 4);
}
TEST(TestPljit, TestInvalidCode){
    Pljit jit;
    std::cout << "Testing invalid tokens/empty code:" << std::endl;
    testInvalidCode(jit, "");
    testInvalidCode(jit, "PARAM a,b; CONST c=1; BEGIN a:=a+c? ;RETURN a+b*c END.");
    testInvalidCode(jit, "PARAM a,&b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.");
    std::cout << "=========================================================" << std::endl;
    std::cout << "Testing wrong parameter/arguments count:" << std::endl;
    testInvalidCode(jit, "PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.");
    testInvalidCode(jit, "PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.",1,1,1);
    std::cout << "=========================================================" << std::endl;
    std::cout << "Testing wrong parameter/arguments count when calling function again:" << std::endl;
    auto func2 = jit.registerFunction("PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.");
    auto result = func2(50,2);
    result = func2(1);
    EXPECT_EQ(result.has_value(), false);
    std::cout << "=========================================================" << std::endl;
}