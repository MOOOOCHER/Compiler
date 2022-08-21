set(PLJIT_SOURCES
    # add your source files here
        Milestone1/SourceCodeManager.cpp Milestone2/Token.cpp
        Milestone3/Node.cpp Milestone3/Parser.cpp Milestone3/ParserTreePrintVisitor.cpp
        ASTDataStructures/ASTNode.cpp Milestone4/ASTSymbolTable.cpp Milestone4/SemanticAnalyzer.cpp Milestone4/ASTTreePrintVisitor.cpp
        Milestone5/ASTEvaluator.cpp Milestone5/OptimizationPass.cpp
        Milestone6/Pljit.cpp ASTDataStructures/ASTTree.cpp ASTDataStructures/ASTTree.h)

add_library(pljit_core ${PLJIT_SOURCES})
target_include_directories(pljit_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_pljit_core ${PLJIT_SOURCES})
add_dependencies(lint lint_pljit_core)

add_executable(pljit main.cpp)
target_link_libraries(pljit PUBLIC pljit_core)
