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
//LiteralNode--------------------------------------------------------------
LiteralNode::LiteralNode(sourceCodeManagement::SourceCodeReference sourceCodeReference, unsigned value): TerminalNode(std::move(sourceCodeReference), Node::Types::Literal), value(value){}
unsigned LiteralNode::getValue() const{ return value;}
void LiteralNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
//------------------------------------------------------------------------
DotNode::DotNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), Dot){}
CommaNode::CommaNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), Comma){}
SemicolonNode::SemicolonNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), Semicolon){}
CloseBracketNode::CloseBracketNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), CloseBracket){}
OpenBracketNode::OpenBracketNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), OpenBracket){}
InitEqualsNode::InitEqualsNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), InitEquals){}
AssignEqualsNode::AssignEqualsNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), AssignEquals){}
PlusOperatorNode::PlusOperatorNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), PlusOperator){}
MinusOperatorNode::MinusOperatorNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), MinusOperator){}
MultiplicationOperatorNode::MultiplicationOperatorNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), MulOperator){}
DivisionOperatorNode::DivisionOperatorNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), DivOperator){}
ReturnNode::ReturnNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), RETURN){}
VariableKeywordNode::VariableKeywordNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), VAR){}
ParameterKeywordNode::ParameterKeywordNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), PARAM){}
ConstantKeywordNode::ConstantKeywordNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), CONST){}
BeginNode::BeginNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), BEGIN){}
EndNode::EndNode(SourceCodeReference sourceCodeReference): TerminalNode(std::move(sourceCodeReference), END){}

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
//-----------------------------------------------------------------------
void DotNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void CommaNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void SemicolonNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void CloseBracketNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void OpenBracketNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void InitEqualsNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void AssignEqualsNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void PlusOperatorNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void MinusOperatorNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void MultiplicationOperatorNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void DivisionOperatorNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void ReturnNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void VariableKeywordNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void ParameterKeywordNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void ConstantKeywordNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void BeginNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
void EndNode::accept(ParseTreeVisitor& visitor) const{
    visitor.visit(*this);
}
} // namespace parser
