#include "ASTTreePrintVisitor.h"
#include <iostream>
namespace semantic{
static std::string convertTypeToString(ASTNode::ASTNodeType type){
    switch (type) {
        case ASTNode::PlusOperator: return "+";
        case ASTNode::MinusOperator: return "-";
        case ASTNode::MulOperator: return "*";
        case ASTNode::DivOperator: return "/";
        case ASTNode::NoOperator: return "no Op";
        case ASTNode::Constant: return"Constant";
        case ASTNode::Parameter: return"Parameter";
        case ASTNode::Variable: return"Variable";
        case ASTNode::LiteralConstant: return"LiteralConstant";
        case ASTNode::FunctionDefinition: return "Function Definition";
        case ASTNode::DeclaratorList: return "DeclaratorList";
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
    std::cout << "\t" << index+1 << " [label=\"" << node.getValue()<< "\"];" << std::endl;
    std::cout << "\t" << index << " -> " << index+1 << ";" << std::endl;
    index++;
}
void ASTTreePrintVisitor::visit(const semantic::ASTLiteralNode& node){
    std::cout << "\t" << index+1 << " [label=\"" << node.getValue()<< "\"];" << std::endl;
    std::cout << "\t" << index << " -> " << index+1 << ";" << std::endl;
    index++;
}
void ASTTreePrintVisitor::visit(const semantic::ASTOperationExpressionNode& node){
    size_t currentIndex = index;
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.getLeftChild()->getType())<< "\"];" << std::endl;
    std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
    ++index;
    node.getLeftChild()->accept(*this);
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.getRightChild()->getType())<< "\"];" << std::endl;
    std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
    ++index;
    node.getRightChild()->accept(*this);
}
void ASTTreePrintVisitor::visit(const semantic::ASTStatementNode& node){
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.getChild()->getType())<< "\"];" << std::endl;
    std::cout << "\t" << index << " -> " << index+1 << ";" << std::endl;
    index++;
    node.getChild()->accept(*this);
}
void ASTTreePrintVisitor::visit(const semantic::ASTDeclaratorListNode& node){
    size_t currentIndex = index;
    for(auto child: node.getChildren()){
        std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(child->getType())<< "\"];" << std::endl;
        std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
        ++index;
        child->accept(*this);
    }
}
void ASTTreePrintVisitor::visit(const semantic::ASTFunctionNode& node) {
    size_t currentIndex = index;
    for(auto child: node.getChildren()){
        std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(child->getType())<< "\"];" << std::endl;
        std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
        ++index;
        child->accept(*this);
    }
}
void ASTTreePrintVisitor::visit(const semantic::ASTInitDeclaratorNode& node){
    size_t currentIndex = index;
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.getLeftChild()->getType())<< "\"];" << std::endl;
    std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
    ++index;
    node.getLeftChild()->accept(*this);
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.getRightChild()->getType())<< "\"];" << std::endl;
    std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
    ++index;
    node.getRightChild()->accept(*this);
}
void ASTTreePrintVisitor::visit(const semantic::ASTInitDeclaratorListNode& node) {
    size_t currentIndex = index;
    for(auto child: node.getChildren()){
        std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(child->getType())<< "\"];" << std::endl;
        std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
        ++index;
        child->accept(*this);
    }
}
void ASTTreePrintVisitor::visit(const semantic::ASTAssignmentExpression& node){
    size_t currentIndex = index;
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.getLeftChild()->getType())<< "\"];" << std::endl;
    std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
    ++index;
    node.getLeftChild()->accept(*this);
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.getRightChild()->getType())<< "\"];" << std::endl;
    std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
    ++index;
    node.getRightChild()->accept(*this);
}
void ASTTreePrintVisitor::visit(const semantic::ASTCompoundStatement& node) {
    size_t currentIndex = index;
    for(auto child: node.getChildren()){
        std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(child->getType())<< "\"];" << std::endl;
        std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
        ++index;
        child->accept(*this);
    }
}
void ASTTreePrintVisitor::visit(const semantic::ASTPrimaryExpression& node) {
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.getChild()->getType())<< "\"];" << std::endl;
    std::cout << "\t" << index << " -> " << index+1 << ";" << std::endl;
    index++;
    node.getChild()->accept(*this);
}
void ASTTreePrintVisitor::visit(const semantic::ASTUnaryExpression& node) {
    std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(node.getChild()->getType())<< "\"];" << std::endl;
    std::cout << "\t" << index << " -> " << index+1 << ";" << std::endl;
    ++index;
    node.getChild()->accept(*this);
}
void ASTTreePrintVisitor::printTree(const ASTNode& node){
    std::cout << "digraph {" << std::endl;
    std::cout << "\t" << index << " [label=\"" << convertTypeToString(node.getType())<< "\"];" << std::endl;
    node.accept(*this);
    std::cout << "}" << std::endl;
}
} // namespace semantic
