#include "Node.h"
#include "ParseTreePrintVisitor.h"
namespace parser{
void IdentifierNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void GenericNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void LiteralNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void NonTerminalNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
//Node--------------------------------------------------------------------
} // namespace parser
