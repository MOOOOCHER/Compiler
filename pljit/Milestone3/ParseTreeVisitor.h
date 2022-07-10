#ifndef PLJIT_PARSETREEVISITOR_H
#define PLJIT_PARSETREEVISITOR_H
#include "Node.h"
namespace parser{
    class ParseTreeVisitor{
        public:
        virtual ~ParseTreeVisitor() = default;
        virtual void visit(const parser::NonTerminalNode& node) = 0;
        virtual void visit(const parser::GenericNode& node) = 0;
        virtual void visit(const parser::LiteralNode& node) = 0;
        virtual void visit(const parser::IdentifierNode& node) = 0;
    };
}
#endif //PLJIT_PARSETREEVISITOR_H
