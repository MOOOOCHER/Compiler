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
        ~TerminalNode() override = default;
        void accept(ParseTreeVisitor& visitor) const override = 0;
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
        protected:
        std::vector<std::unique_ptr<Node>> children;
        NonTerminalNode(Node::Types type,std::vector<std::unique_ptr<Node>> children);
        /*
         * this function computes the source code Reference for a non-terminal node (e.g. Function Definition), given a vector of child nodes
         */
        static SourceCodeReference computeSourceCodeReferenceFromChildren(std::vector<std::unique_ptr<Node>>& children);
        public:
        ~NonTerminalNode() override = default;
        std::vector<std::unique_ptr<Node>> getChildren();
        void accept(ParseTreeVisitor& visitor) const override = 0;
    };
    class FunctionDefinitionNode: public NonTerminalNode{
        public:
        explicit FunctionDefinitionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(FunctionDefinition,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class ParameterDeclarationNode: public NonTerminalNode{
        public:
        explicit ParameterDeclarationNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(ParameterDeclaration,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class VariableDeclarationNode: public NonTerminalNode{
        public:
        explicit VariableDeclarationNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(VariableDeclaration,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class ConstantDeclarationNode: public NonTerminalNode{
        public:
        explicit ConstantDeclarationNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(ConstantDeclaration,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class DeclaratorListNode: public NonTerminalNode{
        public:
        explicit DeclaratorListNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(DeclaratorList,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class InitDeclaratorListNode: public NonTerminalNode{
        public:
        explicit InitDeclaratorListNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(InitDeclaratorList,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class InitDeclaratorNode: public NonTerminalNode{
        public:
        explicit InitDeclaratorNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(InitDeclarator,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    //-----------------------------------------------------------------------------------------------------------------------
    class CompoundStatementNode: public NonTerminalNode{
        public:
        explicit CompoundStatementNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(CompoundStatement,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class StatementListNode: public NonTerminalNode{
        public:
        explicit StatementListNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(StatementList,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class StatementNode: public NonTerminalNode{
        public:
        explicit StatementNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(Statement,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class AssignmentExpressionNode: public NonTerminalNode{
        public:
        explicit AssignmentExpressionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(AssignmentExpression,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class AdditiveExpressionNode: public NonTerminalNode{
        public:
        explicit AdditiveExpressionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(AdditiveExpression,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class MultiplicativeExpressionNode: public NonTerminalNode{
        public:
        explicit MultiplicativeExpressionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(MultiplicativeExpression,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class UnaryExpressionNode: public NonTerminalNode{
        public:
        explicit UnaryExpressionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(UnaryExpression,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class PrimaryExpressionNode: public NonTerminalNode{
        public:
        explicit PrimaryExpressionNode(std::vector<std::unique_ptr<Node>> children): NonTerminalNode(PrimaryExpression,std::move(children)){}
        void accept(ParseTreeVisitor& visitor) const override;
    };
} // namespace parser
#endif //PLJIT_NODE_H
