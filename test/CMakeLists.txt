set(TEST_SOURCES
    # add your source files here
    Tester.cpp TestLexer.cpp TestParser.cpp TestSemantic.cpp TestEvaluation.cpp TestPljit.cpp)

add_executable(tester ${TEST_SOURCES})
target_link_libraries(tester PUBLIC
    pljit_core
    GTest::GTest)
