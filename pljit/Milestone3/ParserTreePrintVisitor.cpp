#include "ParseTreePrintVisitor.h"
#include <iostream>
namespace parser{
static std::string convertTypeToString(Node::Types type){
    switch (type) {
        case Node::Identifier: return "identifier";
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
        case Node::Dot: return "Dot";
        case Node::Comma: return "Comma";
        case Node::Semicolon: return "Semicolon";
        case Node::InitEquals: return "InitEquals";
        case Node::AssignEquals: return "AssignEquals";
        case Node::OpenBracket: return "OpenBracket";
        case Node::CloseBracket: return "CloseBracket";
        case Node::PlusOperator: return "PlusOperator";
        case Node::MinusOperator: return "MinusOperator";
        case Node::MulOperator: return "MulOperator";
        case Node::DivOperator: return "DivOperator";
        case Node::RETURN: return "RETURN";
        case Node::VAR: return "VAR";
        case Node::PARAM: return "PARAM";
        case Node::CONST: return "CONST";
        case Node::BEGIN: return "BEGIN";
        case Node::END: return "END";
        case Node::Invalid: return "Invalid";
        default: return "";
    }
}
void ParseTreePrintVisitor::visit(const NonTerminalNode& node) {
    for(auto child: node.getChildren()){
        std::cout << "\t" << convertTypeToString(node.getType()) << " -> " << convertTypeToString(child->getType()) << ";" << std::endl;
        child->accept(*this);
    }
}
void ParseTreePrintVisitor::visit(const GenericNode&) {
}
void ParseTreePrintVisitor::visit(const LiteralNode& node) {
    std::cout << "\t" << convertTypeToString(node.getType()) << " -> " << node.getValue() << ";" << std::endl;
}
void ParseTreePrintVisitor::visit(const IdentifierNode& node) {
    std::cout << "\t" << convertTypeToString(node.getType()) << " -> " << node.getText() << ";" << std::endl;
}
void ParseTreePrintVisitor::printTree(const Node& node){
    std::cout << "digraph {" << std::endl;
    node.accept(*this);
    std::cout << "}" << std::endl;
}
}
