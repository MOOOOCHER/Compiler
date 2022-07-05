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
            PrimaryExpression
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
        //for testing purposes
        public:
        T getInformation() { return information;}

        protected:
        T information;
        TerminalNode(T information, typename Node::Types type, SourceCodeManager manager): Node( type, manager), information(information){}
    };
    class IdentifierNode: public TerminalNode<std::string>{
        public:
        IdentifierNode(std::string information, SourceCodeManager manager): TerminalNode<std::string>(std::move(information), Node::Types::Identifier, manager){}
    };
    class LiteralNode: public TerminalNode<unsigned long>{
        public:
        LiteralNode(unsigned long information, SourceCodeManager manager): TerminalNode<unsigned long>(information, Node::Types::Literal, manager){}
    };
    class GenericNode: public TerminalNode<std::string>{
        public:
        GenericNode(std::string information, SourceCodeManager manager): TerminalNode<std::string>(std::move(information), Node::Types::Generic, manager){}
    };

    //Non-Terminal Node-----------------------------------------------------------------------------------------------------------------------
    class NonTerminalNode: public Node{
        friend class Parser;
        std::vector<std::unique_ptr<Node>> vec;

        public:
        NonTerminalNode(typename Node::Types type, SourceCodeManager manager): Node(type, manager) {}
        std::vector<Node*> getChildren(){
            std::vector<Node*> children;
            for(auto& child: vec){
                children.push_back(child.get());
            }
            return children;
        }//only for testing
    };
} // namespace parser
#endif //PLJIT_NODE_H
