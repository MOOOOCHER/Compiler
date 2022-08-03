#include "Node.h"
#include "ParseTreePrintVisitor.h"
namespace parser{
//TerminalNode-----------------------------------------------------------
TerminalNode::TerminalNode(sourceCodeManagement::SourceCodeReference sourceCodeReference, typename Node::Types type): Node(type,std::move(sourceCodeReference)){}

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
NonTerminalNode::NonTerminalNode(Node::Types type,std::vector<std::unique_ptr<Node>> children): Node(type,computeSourceCodeReferenceFromChildren(children)), children(std::move(children)) {}
void NonTerminalNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
SourceCodeReference NonTerminalNode::computeSourceCodeReferenceFromChildren(std::vector<std::unique_ptr<Node>>& children){
    auto firstChildRef = children[0]->getReference();
    auto lastChildRef = children[children.size()-1]->getReference();
    size_t startingPosition = firstChildRef.getPositionInCode();
    size_t length = lastChildRef.getPositionInCode() - firstChildRef.getPositionInCode() + lastChildRef.getLengthOfString();
    return {startingPosition,children[0]->getReference().getManager(),length};
}
//Node--------------------------------------------------------------------
} // namespace parser
