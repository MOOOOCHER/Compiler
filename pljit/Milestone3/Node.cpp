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

SourceCodeReference NonTerminalNode::computeSourceCodeReferenceFromChildren(std::vector<std::unique_ptr<Node>>& children){
    auto firstChildRef = children[0]->getReference();
    auto lastChildRef = children[children.size()-1]->getReference();
    size_t startingPosition = firstChildRef.getPositionInCode();
    size_t length = lastChildRef.getPositionInCode() - firstChildRef.getPositionInCode() + lastChildRef.getLengthOfString();
    return {startingPosition,children[0]->getReference().getManager(),length};
}
//------------------------------------------------------------------------
FunctionDefinitionNode::FunctionDefinitionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(FunctionDefinition,std::move(children)){}
ParameterDeclarationNode::ParameterDeclarationNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(ParameterDeclaration,std::move(children)){}
VariableDeclarationNode::VariableDeclarationNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(VariableDeclaration,std::move(children)){}
ConstantDeclarationNode::ConstantDeclarationNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(ConstantDeclaration,std::move(children)){}
DeclaratorListNode::DeclaratorListNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(DeclaratorList,std::move(children)){}
InitDeclaratorListNode::InitDeclaratorListNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(InitDeclaratorList,std::move(children)){}
InitDeclaratorNode::InitDeclaratorNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(InitDeclarator,std::move(children)){}
CompoundStatementNode::CompoundStatementNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(CompoundStatement,std::move(children)){}
StatementListNode::StatementListNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(StatementList,std::move(children)){}
StatementNode::StatementNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(Statement,std::move(children)){}
AssignmentExpressionNode::AssignmentExpressionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(AssignmentExpression,std::move(children)){}
AdditiveExpressionNode::AdditiveExpressionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(AdditiveExpression,std::move(children)){}
MultiplicativeExpressionNode::MultiplicativeExpressionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(MultiplicativeExpression,std::move(children)){}
UnaryExpressionNode::UnaryExpressionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(UnaryExpression,std::move(children)){}
PrimaryExpressionNode::PrimaryExpressionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(PrimaryExpression,std::move(children)){}
//Node--------------------------------------------------------------------
void FunctionDefinitionNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void ParameterDeclarationNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void VariableDeclarationNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void ConstantDeclarationNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void DeclaratorListNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void InitDeclaratorListNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void InitDeclaratorNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void CompoundStatementNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void StatementListNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void StatementNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void AssignmentExpressionNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void AdditiveExpressionNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void MultiplicativeExpressionNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void UnaryExpressionNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void PrimaryExpressionNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
} // namespace parser
