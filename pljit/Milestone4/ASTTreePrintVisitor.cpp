#include "ASTTreePrintVisitor.h"
#include <iostream>
namespace semantic{
static std::string convertTypeToString(ASTNode::ASTNodeType type){
    switch (type) {
        case ASTNode::PlusOperator: return "+";
        case ASTNode::MinusOperator: return "-";
        case ASTNode::MulOperator: return "*";
        case ASTNode::DivOperator: return "/";
        case ASTNode::Constant: return"Constant";
        case ASTNode::Parameter: return"Parameter";
        case ASTNode::Variable: return"Variable";
        case ASTNode::LiteralConstant: return"LiteralConstant";
        case ASTNode::FunctionDefinition: return "Function Definition";
        case ASTNode::ParamDeclaratorList: return "ParamDeclaratorList";
        case ASTNode::VarDeclaratorList: return "VarDeclaratorList";
        case ASTNode::InitDeclaratorList: return "InitDeclaratorList";
        case ASTNode::InitDeclarator: return "=";
        case ASTNode::CompoundStatement: return "CompoundStatement";
        case ASTNode::AssignStatement: return ":=";
        case ASTNode::ReturnStatement: return "RETURN";
        case ASTNode::UnaryPlus: return "+";
        case ASTNode::UnaryMinus: return "-";
        case ASTNode::PrimaryExpression: return "PrimaryExpression";
        case ASTNode::AssignmentExpression: return ":=";
        default: return "";
    }
}
void ASTTreePrintVisitor::visit(const semantic::ASTIdentifierNode& node){
    printValueAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTLiteralNode& node){
    printValueAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTOperationExpressionNode& node){
    printBinaryAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTAssignmentStatementNode& node){
    printUnaryAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTReturnStatementNode& node){
    printUnaryAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTParamDeclaratorListNode& node){
    printMultiAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTVarDeclaratorListNode& node){
    printMultiAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTFunctionNode& node) {
    printMultiAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTInitDeclaratorNode& node){
    printBinaryAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTInitDeclaratorListNode& node) {
    printMultiAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTAssignmentExpression& node){
    printBinaryAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTCompoundStatement& node) {
    printMultiAST(node);
}
void ASTTreePrintVisitor::visit(const semantic::ASTUnaryExpression& node) {
    printUnaryAST(node);
}
void ASTTreePrintVisitor::printTree(const ASTNode& node){
    std::cout << "digraph {" << std::endl;
    std::cout << "\t" << index << " [label=\"" << convertTypeToString(node.getType())<< "\"];" << std::endl;
    node.accept(*this);
    std::cout << "}" << std::endl;
}
void ASTTreePrintVisitor::printMultiAST(const MultiASTNode& node) {
    size_t currentIndex = index;
    for(auto& child: node.children){
        std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(child->getType())<< "\"];" << std::endl;
        std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
        ++index;
        child->accept(*this);
    }
}
void ASTTreePrintVisitor::printValueAST(const auto& node) {
    std::cout << "\t" << index+1 << " [label=\"" << node.getValue()<< "\"];" << std::endl;
    std::cout << "\t" << index << " -> " << index+1 << ";" << std::endl;
    index++;
}
void ASTTreePrintVisitor::printUnaryAST(const auto& node){
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.child->getType())<< "\"];" << std::endl;
    std::cout << "\t" << index << " -> " << index+1 << ";" << std::endl;
    ++index;
    node.child->accept(*this);
}
void ASTTreePrintVisitor::printBinaryAST(const auto& node){
    size_t currentIndex = index;
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.leftChild->getType())<< "\"];" << std::endl;
    std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
    ++index;
    node.leftChild->accept(*this);
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.rightChild->getType())<< "\"];" << std::endl;
    std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
    ++index;
    node.rightChild->accept(*this);
}
} // namespace semantic
