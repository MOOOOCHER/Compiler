#ifndef PLJIT_PARSETREEPRINTVISITOR_H
#define PLJIT_PARSETREEPRINTVISITOR_H
#include "ParseTreeVisitor.h"
namespace parser{
    class ParseTreePrintVisitor: public ParseTreeVisitor{
        public:
        void visit(const NonTerminalNode& node)override;
        void visit(const GenericNode& node) override;
        void visit(const LiteralNode& node) override;
        void visit(const IdentifierNode& node) override;
        void printTree(const Node& node);
    };
}
#endif //PLJIT_PARSETREEPRINTVISITOR_H
