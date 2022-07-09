#ifndef PLJIT_NODE_H
#define PLJIT_NODE_H
#include <memory>
#include <utility>
#include <vector>
#include "../Milestone1/SourceCodeManager.h"
namespace parser{
using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
    class Node{
        public:
        enum Types{
            Identifier,
            Literal,
            Generic,
            FunctionDefinition,
            ParameterDeclaration,
            VariableDeclaration,
            ConstantDeclaration,
            DeclaratorList,
            InitDeclaratorList,
            InitDeclarator,
            CompoundStatement,
            StatementList,
            Statement,
            AssignmentExpression,
            AdditiveExpression,
            MultiplicativeExpression,
            UnaryExpression,
            PrimaryExpression,
            Dot,
            Comma,
            Semicolon,
            InitEquals,
            AssignEquals,
            OpenBracket,
            CloseBracket,
            PlusOperator,
            MinusOperator,
            MulOperator,
            DivOperator,
            RETURN,
            VAR,
            PARAM,
            CONST,
            BEGIN,
            END,
            Invalid
        };
        Types getType(){
            return type;
        };
        virtual ~Node() = default;
        protected:
        Types type;
        sourceCodeManagement::SourceCodeManager sourceCodeManager;
        Node( Types type, SourceCodeManager manager): type(type), sourceCodeManager(manager){};
    };

    //Terminal Node---------------------------------------------------------------------------------------------------------------------
    template<typename T>
    class TerminalNode: public Node{
        protected:
        T information;
        TerminalNode(T information, typename Node::Types type, SourceCodeManager manager): Node(type, manager), information(information){}
    };
    class IdentifierNode: public TerminalNode<sourceCodeManagement::SourceCodeReference>{
        public:
        IdentifierNode(sourceCodeManagement::SourceCodeReference information, SourceCodeManager manager): TerminalNode<sourceCodeManagement::SourceCodeReference>(information, Node::Types::Identifier, manager){}

    };
    class LiteralNode: public TerminalNode<unsigned long>{
        public:
        LiteralNode(unsigned long information, SourceCodeManager manager): TerminalNode<unsigned long>(information, Node::Types::Literal, manager){}
        unsigned long getInformation() { return information;}
    };
    class GenericNode: public TerminalNode<sourceCodeManagement::SourceCodeReference>{
        public:
        GenericNode(sourceCodeManagement::SourceCodeReference information, SourceCodeManager manager,  Types type): TerminalNode<sourceCodeManagement::SourceCodeReference>(information, type, manager){}
    };

    //Non-Terminal Node-----------------------------------------------------------------------------------------------------------------------
    class NonTerminalNode: public Node{
        friend class Parser;
        std::vector<std::unique_ptr<Node>> children;

        public:
        NonTerminalNode(typename Node::Types type, SourceCodeManager manager): Node(type, manager) {}
        std::vector<Node*> getChildren(){
            std::vector<Node*> vec;
            for(auto& child: children){
                vec.push_back(child.get());
            }
            return vec;
        }//only for testing
    };
} // namespace parser
#endif //PLJIT_NODE_H
