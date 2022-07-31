#ifndef PLJIT_NODE_H
#define PLJIT_NODE_H
#include <memory>
#include <utility>
#include <vector>
#include "../Milestone1/SourceCodeManager.h"
namespace parser{
using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
class ParseTreeVisitor;
class ParseTreePrintVisitor;

    class Node{
        public:
        enum Types{
            Identifier,
            Literal,
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
        Types getType() const{ return type; };
        virtual ~Node() = default;
        virtual void accept(ParseTreeVisitor& visitor) const = 0;
        protected:
        Types type;
        explicit Node( Types type): type(type){};
    };

    //Terminal Node---------------------------------------------------------------------------------------------------------------------
    class TerminalNode: public Node{
        protected:
        sourceCodeManagement::SourceCodeReference sourceCodeReference;
        TerminalNode(sourceCodeManagement::SourceCodeReference sourceCodeReference, typename Node::Types type): Node(type), sourceCodeReference(std::move(sourceCodeReference)){}
        public:
        sourceCodeManagement::SourceCodeReference getReference(){
            return sourceCodeReference;
        }
    };
    class IdentifierNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        std::string_view text;

        public:
        IdentifierNode(sourceCodeManagement::SourceCodeReference sourceCodeReference,  std::string_view name): TerminalNode(std::move(sourceCodeReference), Node::Types::Identifier), text(name){}
        std::string_view getText() const{ return text; }
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class LiteralNode: public TerminalNode{
        unsigned long value;
        public:
        LiteralNode(sourceCodeManagement::SourceCodeReference sourceCodeReference, unsigned long value): TerminalNode(std::move(sourceCodeReference), Node::Types::Literal), value(value){}
        unsigned long getValue() const{ return value;}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class GenericNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        GenericNode(sourceCodeManagement::SourceCodeReference sourceCodeReference,  Types type): TerminalNode(std::move(sourceCodeReference), type){}
        void accept(ParseTreeVisitor& visitor) const override;
    };

    //Non-Terminal Node-----------------------------------------------------------------------------------------------------------------------
    class NonTerminalNode: public Node{
        friend class Parser;
        std::vector<std::unique_ptr<Node>> children;
        public:
        explicit NonTerminalNode(typename Node::Types type): Node(type) {}
        std::vector<Node*> getChildren() const{
            std::vector<Node*> vec;
            for(auto& child: children){
                vec.push_back(child.get());
            }
            return vec;
        }//only for testing
        void accept(ParseTreeVisitor& visitor) const override;
    };
} // namespace parser
#endif //PLJIT_NODE_H
