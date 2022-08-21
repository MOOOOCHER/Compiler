#include "ParseTreePrintVisitor.h"
#include <iostream>
namespace parser{
static std::string convertTypeToString(Node::Types type){
    switch (type) {
        case Node::Identifier: return "Identifier";
        case Node::Literal: return "Literal";
        case Node::FunctionDefinition: return"FunctionDefinition";
        case Node::ParameterDeclaration: return "ParameterDeclaration";
        case Node::VariableDeclaration: return "VariableDeclaration";
        case Node::ConstantDeclaration: return "ConstantDeclaration";
        case Node::DeclaratorList: return "DeclaratorList";
        case Node::InitDeclaratorList: return "InitDeclaratorList";
        case Node::InitDeclarator: return "InitDeclarator";
        case Node::CompoundStatement: return "CompoundStatement";
        case Node::StatementList: return "StatementList";
        case Node::Statement: return "Statement";
        case Node::AssignmentExpression: return "AssignmentExpression";
        case Node::AdditiveExpression: return "AdditiveExpression";
        case Node::MultiplicativeExpression: return "MultiplicativeExpression";
        case Node::UnaryExpression: return "UnaryExpression";
        case Node::PrimaryExpression: return "PrimaryExpression";
        case Node::Dot: return ".";
        case Node::Comma: return ",";
        case Node::Semicolon: return ";";
        case Node::InitEquals: return "=";
        case Node::AssignEquals: return ":=";
        case Node::OpenBracket: return "(";
        case Node::CloseBracket: return ")";
        case Node::PlusOperator: return "+";
        case Node::MinusOperator: return "-";
        case Node::MulOperator: return "*";
        case Node::DivOperator: return "/";
        case Node::RETURN: return "RETURN";
        case Node::VAR: return "VAR";
        case Node::PARAM: return "PARAM";
        case Node::CONST: return "CONST";
        case Node::BEGIN: return "BEGIN";
        case Node::END: return "END";
        default: return "";
    }
}


void ParseTreePrintVisitor::visit(const LiteralNode& node) {
    std::cout << "\t" << index+1 << " [label=\"" << node.getValue()<< "\"];" << std::endl;
    std::cout << "\t" << index << " -> " << index+1 << ";" << std::endl;
    index++;
}
void ParseTreePrintVisitor::visit(const IdentifierNode& node) {
    std::cout << "\t" << index+1 << " [label=\"" << node.getText()<< "\"];" << std::endl;
    std::cout << "\t" << index << " -> " << index+1 << ";" << std::endl;
    index++;
}
void ParseTreePrintVisitor::printTree(const Node& node){
    std::cout << "digraph {" << std::endl;
    std::cout << "\t" << index << " [label=\"" << convertTypeToString(node.getType())<< "\"];" << std::endl;
    node.accept(*this);
    std::cout << "}" << std::endl;
}
void ParseTreePrintVisitor::visit(const parser::FunctionDefinitionNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::ParameterDeclarationNode& node){
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::VariableDeclarationNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::ConstantDeclarationNode& node){
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::DeclaratorListNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::InitDeclaratorListNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::InitDeclaratorNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::CompoundStatementNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::StatementListNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::StatementNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::AssignmentExpressionNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::AdditiveExpressionNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::MultiplicativeExpressionNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::UnaryExpressionNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visit(const parser::PrimaryExpressionNode& node) {
    visitRefactor(node);
}
void ParseTreePrintVisitor::visitRefactor(const NonTerminalNode& node) {
    size_t currentIndex = index;
    for(auto& child: node.children){
        std::cout << "\t" << index+1 << " [label=\"" << convertTypeToString(child->getType())<< "\"];" << std::endl;
        std::cout << "\t" << currentIndex << " -> " << index+1 << ";" << std::endl;
        ++index;
        child->accept(*this);
    }
}
} // namespace parser
