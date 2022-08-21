#include "pljit/Pljit/Pljit.h"
#include <thread>
#include <gtest/gtest.h>
#include <unordered_set>
#include <utility>
using Pljit = pljit::Pljit;
template<std::floating_point ...Args>
static void testValidCode(Pljit& jit, std::string input, double expectedResult, Args... args){
    auto func = jit.registerFunction(std::move(input));
    auto result = func(args...);
    ASSERT_EQ(result.has_value(), true);
    EXPECT_EQ(result.value(), expectedResult);
}

template<std::floating_point ...Args>
static void testInvalidCode(Pljit& jit, std::string input, Args... args){
    auto func = jit.registerFunction(std::move(input));
    auto result = func(args...);
    EXPECT_EQ(result.has_value(), false);
}
TEST(TestPljit, TestValidCodeSimple){
    Pljit jit;
    //calling twice
    auto func2 = jit.registerFunction("PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.");
    auto result = func2(50.0,2.0);
    ASSERT_EQ(result.has_value(), true);
    EXPECT_EQ(result.value(), 53);
    result = func2(1.0,2.0);
    ASSERT_EQ(result.has_value(), true);
    EXPECT_EQ(result.value(), 4);
    auto func = jit.registerFunction("PARAM hello, world; BEGIN RETURN hello/10+world END.");
    result = func(10.0,5.0);
    ASSERT_EQ(result.has_value(), true);
    EXPECT_EQ(result.value(), 6);
    result = func2(111.3,22341.0);
    ASSERT_EQ(result.has_value(), true);
    EXPECT_EQ(result.value(), 22453.3);
    result = func(10.0,5.0);
    ASSERT_EQ(result.has_value(), true);
    EXPECT_EQ(result.value(), 6);
}
TEST(TestPljit, TestMultiThread){
    Pljit jit;
    std::vector<std::thread> threads;
    auto func = jit.registerFunction("PARAM a; CONST c=5; BEGIN RETURN a*c END.");
    for(uint32_t i = 0; i<10;++i){
        threads.emplace_back([&func](){
            for(double value = 0; value<5;++value){
                auto result = func(value);
                ASSERT_EQ(result.has_value(), true);
                EXPECT_EQ(result.value(), 5*value);
            }
        });
    }
    for(auto& thread: threads){
        thread.join();
    }
}
TEST(TestPljit, TestInvalidCode){
    Pljit jit;
    std::cout << "Testing invalid tokens/empty code:" << std::endl;
    testInvalidCode(jit, "");
    testInvalidCode(jit, "PARAM a,b; CONST c=1; BEGIN a:=a+c? ;RETURN a+b*c END.");
    testInvalidCode(jit, "PARAM a,&b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.");
    testInvalidCode(jit, "PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END._");
    std::cout << "=========================================================" << std::endl;
    std::cout << "Testing invalid syntax:" << std::endl;
    testInvalidCode(jit, "PARAM a;b; CONST c=1; BEGIN ;RETURN a+b*c END.");
    testInvalidCode(jit, "PARAM a,b; VAR a v; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.");
    testInvalidCode(jit, "PARAM a,b; CONST c:=1; BEGIN a:=a+c ;RETURN a+b*c END._");
    std::cout << "Testing invalid semantic:" << std::endl;
    testInvalidCode(jit, "PARAM a; CONST c=1; BEGIN c:=2;RETURN a+b*c END.");
    testInvalidCode(jit, "PARAM a,b; CONST c=1; BEGIN z:=a+c ;RETURN a+b*c END.");
    testInvalidCode(jit, "PARAM a,b; CONST b=1; BEGIN a:=a+a ;RETURN a+b*a END.");
    testInvalidCode(jit, "PARAM a,b; CONST c=1; BEGIN b:=a+c END.");
    testInvalidCode(jit, "VAR aaa,b; CONST c=1; BEGIN b:=aaa+c; RETURN b END.");
    std::cout << "=========================================================" << std::endl;
    std::cout << "Testing wrong parameter/arguments count:" << std::endl;
    testInvalidCode(jit, "PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.");
    testInvalidCode(jit, "PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.",1.0,1.0,1.0);
    std::cout << "=========================================================" << std::endl;
    std::cout << "Testing wrong parameter/arguments count when calling function again:" << std::endl;
    auto func2 = jit.registerFunction("PARAM a,b; CONST c=1; BEGIN a:=a+c ;RETURN a+b*c END.");
    auto result = func2(50.0,2.0);
    result = func2(1.0);
    EXPECT_EQ(result.has_value(), false);
    std::cout << "=========================================================" << std::endl;
}
TEST(TestPljit, TestManyHandles){
    Pljit jit;
    //calling twice
    std::vector<pljit::PljitHandle> handles;
    for(size_t i=0;i<50;i++){
        auto func= jit.registerFunction("PARAM a,b; BEGIN RETURN a+b END.");
        handles.push_back(std::move(func));
    }
    for(size_t i=0;i<150;i++){
        auto result = handles[i%50].operator()(static_cast<double>(i),10.0);
        EXPECT_EQ(result.value(),i+10);
    }
}
TEST(TestPljit, TestValidCodeComplex){
    Pljit jit;
    testValidCode(jit,"VAR a,b,c,d,e,f,z,g,i,j,ab,aaa ; BEGIN ab := 1 ;RETURN ab END.",1);
    testValidCode(jit,"VAR a,b,c,d,e,f,z,g,i,j,aa,aaa,aaaa,aaaaa,bb,bbb,bbbb,cc,ccc,cccc,dd,ddd,dddd,ee,eee,eeee,ff,fff,ffff,gg,ggg,gggg;"
                       "BEGIN a := 1; b := 1; c := 1;d := 1;e := 1;f := 1;z := 1;g := 1;i := 1;j := 1;aa := 1;aaa := 1;"
                       "\nb := 1; b:=b+a; b:=b+a; b:=b+a; b:=b+a; b:=b+a; b:=b+a; b:=b+a; b:=b+a; b:=b+a; b:=b+a; b:=b+a;"
                       "\naaa :=1;aaaa:=1;aaaaa:=1;bb:=1;bbb:=1;bbbb:=1;cc:=1;ccc:=1;cccc:=1;dd:=1;ddd:=1;dddd:=1;ee:=1;eee:=1;eeee:=1;ff:=1;fff:=1;"
                       "\nffff:=1;ffff:=1;gg:=1;ggg:=1;gggg:=1;b:=b+dd;b:=b+b*g;"
                       "\nRETURN b END.",26);
}