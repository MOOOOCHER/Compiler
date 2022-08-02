#ifndef PLJIT_NODE_H
#define PLJIT_NODE_H
#include <memory>
#include <utility>
#include <vector>
#include "../Milestone1/SourceCodeManager.h"
namespace parser{
using SourceCodeManager = sourceCodeManagement::SourceCodeManager;
using SourceCodeReference = sourceCodeManagement::SourceCodeReference;
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
        SourceCodeReference getReference() const { return sourceCodeReference; }
        virtual ~Node() = default;
        virtual void accept(ParseTreeVisitor& visitor) const = 0;
        protected:
        Types type;
        SourceCodeReference sourceCodeReference;
        explicit Node(Types type,SourceCodeReference sourceCodeReference): type(type), sourceCodeReference(std::move(sourceCodeReference)){};
    };

    //Terminal Node---------------------------------------------------------------------------------------------------------------------
    class TerminalNode: public Node{
        protected:

        TerminalNode(SourceCodeReference sourceCodeReference, typename Node::Types type);
        public:

    };
    //Terminal Node subclasses----------------------------------------------------------------------------------------------------------
    class IdentifierNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        std::string_view text;

        public:
        IdentifierNode(SourceCodeReference sourceCodeReference, std::string_view name);
        std::string_view getText() const;
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class LiteralNode: public TerminalNode{
        unsigned value;
        public:
        LiteralNode(SourceCodeReference sourceCodeReference, unsigned value);
        unsigned getValue() const;
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class GenericNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        GenericNode(SourceCodeReference sourceCodeReference, Types type);
        void accept(ParseTreeVisitor& visitor) const override;
    };

    //Non-Terminal Node-----------------------------------------------------------------------------------------------------------------------
    class NonTerminalNode: public Node{
        friend class Parser;
        friend class ParseTreePrintVisitor;
        std::vector<std::unique_ptr<Node>> children;
        /*
         * this function computes the source code Reference for a non-terminal node (e.g. Function Definition), given a vector of child nodes
         */
        static SourceCodeReference computeSourceCodeReferenceFromChildren(std::vector<std::unique_ptr<Node>>& children);
        public:
        explicit NonTerminalNode(typename Node::Types type,std::vector<std::unique_ptr<Node>> children): Node(type,computeSourceCodeReferenceFromChildren(children)), children(std::move(children)) {}
        std::vector<std::unique_ptr<Node>> getChildren();
        void accept(ParseTreeVisitor& visitor) const override;
    };
} // namespace parser
#endif //PLJIT_NODE_H
