#ifndef PLJIT_PARSETREEPRINTVISITOR_H
#define PLJIT_PARSETREEPRINTVISITOR_H
#include "ParseTreeVisitor.h"
namespace parser{
    class ParseTreePrintVisitor: public ParseTreeVisitor{
        size_t index = 0;
        public:
        void visit(const LiteralNode& node) override;
        void visit(const IdentifierNode& node) override;
        void visit(const parser::FunctionDefinitionNode& node) override;
        void visit(const parser::ParameterDeclarationNode& node)override;
        void visit(const parser::VariableDeclarationNode& node) override;
        void visit(const parser::ConstantDeclarationNode& node) override;
        void visit(const parser::DeclaratorListNode& node) override;
        void visit(const parser::InitDeclaratorListNode& node)override;
        void visit(const parser::InitDeclaratorNode& node) override;
        void visit(const parser::CompoundStatementNode& node) override;
        void visit(const parser::StatementListNode& node) override;
        void visit(const parser::StatementNode& node) override;
        void visit(const parser::AssignmentExpressionNode& node) override;
        void visit(const parser::AdditiveExpressionNode& node) override;
        void visit(const parser::MultiplicativeExpressionNode& node) override;
        void visit(const parser::UnaryExpressionNode& node) override;
        void visit(const parser::PrimaryExpressionNode& node) override;


        void visit(const parser::DotNode& ) override{};
        void visit(const parser::CommaNode& ) override{};
        void visit(const parser::SemicolonNode& ) override{};
        void visit(const parser::CloseBracketNode& ) override{};
        void visit(const parser::OpenBracketNode& ) override{};
        void visit(const parser::InitEqualsNode& )  override{};
        void visit(const parser::AssignEqualsNode& ) override{};
        void visit(const parser::PlusOperatorNode& ) override{};
        void visit(const parser::MinusOperatorNode& ) override{};
        void visit(const parser::MultiplicationOperatorNode& ) override{};
        void visit(const parser::DivisionOperatorNode& ) override{};
        void visit(const parser::ReturnNode& ) override{};
        void visit(const parser::VariableKeywordNode& ) override{};
        void visit(const parser::ParameterKeywordNode& ) override{};
        void visit(const parser::ConstantKeywordNode& ) override{};
        void visit(const parser::BeginNode& ) override{};
        void visit(const parser::EndNode& ) override{};

        void visitRefactor(const NonTerminalNode& node);
        void printTree(const Node& node);
    };
} // namespace parser
#endif //PLJIT_PARSETREEPRINTVISITOR_H
