#include "Node.h"
#include "ParseTreePrintVisitor.h"
namespace parser{
//TerminalNode-----------------------------------------------------------
TerminalNode::TerminalNode(sourceCodeManagement::SourceCodeReference sourceCodeReference, typename Node::Types type): Node(type), sourceCodeReference(std::move(sourceCodeReference)){}
SourceCodeReference TerminalNode::getReference(){
    return sourceCodeReference;
}
//IdentifierNode-----------------------------------------------------------
IdentifierNode::IdentifierNode(sourceCodeManagement::SourceCodeReference sourceCodeReference,  std::string_view name): TerminalNode(std::move(sourceCodeReference), Node::Types::Identifier), text(name){}
std::string_view IdentifierNode::getText() const{ return text; }
void IdentifierNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
//GenericNode--------------------------------------------------------------
GenericNode::GenericNode(sourceCodeManagement::SourceCodeReference sourceCodeReference, Types type): TerminalNode(std::move(sourceCodeReference), type){}
void GenericNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
//LiteralNode--------------------------------------------------------------
LiteralNode::LiteralNode(sourceCodeManagement::SourceCodeReference sourceCodeReference, unsigned value): TerminalNode(std::move(sourceCodeReference), Node::Types::Literal), value(value){}
unsigned LiteralNode::getValue() const{ return value;}
void LiteralNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
//NonTerminalNode----------------------------------------------------------
std::vector<std::unique_ptr<Node>> NonTerminalNode::getChildren(){
    return std::move(children);
}
void NonTerminalNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
//Node--------------------------------------------------------------------
} // namespace parser
