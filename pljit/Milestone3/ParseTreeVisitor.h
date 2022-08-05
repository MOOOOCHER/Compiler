#ifndef PLJIT_PARSETREEVISITOR_H
#define PLJIT_PARSETREEVISITOR_H
#include "Node.h"
namespace parser{
    class ParseTreeVisitor{
        public:
        virtual ~ParseTreeVisitor() = default;
        virtual void visit(const parser::GenericNode& node) = 0;
        virtual void visit(const parser::LiteralNode& node) = 0;
        virtual void visit(const parser::IdentifierNode& node) = 0;
        virtual void visit(const parser::FunctionDefinitionNode& node) = 0;
        virtual void visit(const parser::ParameterDeclarationNode& node) = 0;
        virtual void visit(const parser::VariableDeclarationNode& node) = 0;
        virtual void visit(const parser::ConstantDeclarationNode& node) = 0;
        virtual void visit(const parser::DeclaratorListNode& node) = 0;
        virtual void visit(const parser::InitDeclaratorListNode& node) = 0;
        virtual void visit(const parser::InitDeclaratorNode& node) = 0;
        virtual void visit(const parser::CompoundStatementNode& node) = 0;
        virtual void visit(const parser::StatementListNode& node) = 0;
        virtual void visit(const parser::StatementNode& node) = 0;
        virtual void visit(const parser::AssignmentExpressionNode& node) = 0;
        virtual void visit(const parser::AdditiveExpressionNode& node) = 0;
        virtual void visit(const parser::MultiplicativeExpressionNode& node) = 0;
        virtual void visit(const parser::UnaryExpressionNode& node) = 0;
        virtual void visit(const parser::PrimaryExpressionNode& node) = 0;
    };
} // namespace parser
#endif //PLJIT_PARSETREEVISITOR_H
