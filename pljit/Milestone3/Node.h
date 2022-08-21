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
        sourceCodeManagement::SourceCodeReference sourceCodeReference;
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
    class DotNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit DotNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class CommaNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit CommaNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class SemicolonNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit SemicolonNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class CloseBracketNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit CloseBracketNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class OpenBracketNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit OpenBracketNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class InitEqualsNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit InitEqualsNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class AssignEqualsNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit AssignEqualsNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class PlusOperatorNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit PlusOperatorNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class MinusOperatorNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit MinusOperatorNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class MultiplicationOperatorNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit MultiplicationOperatorNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class DivisionOperatorNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit DivisionOperatorNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class ReturnNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit ReturnNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class VariableKeywordNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit VariableKeywordNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class ParameterKeywordNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit ParameterKeywordNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class ConstantKeywordNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit ConstantKeywordNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class BeginNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit BeginNode(SourceCodeReference sourceCodeReference);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class EndNode: public TerminalNode{
        friend class ParseTreePrintVisitor;
        public:
        explicit EndNode(SourceCodeReference sourceCodeReference);
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
        explicit FunctionDefinitionNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class ParameterDeclarationNode: public NonTerminalNode{
        public:
        explicit ParameterDeclarationNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class VariableDeclarationNode: public NonTerminalNode{
        public:
        explicit VariableDeclarationNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class ConstantDeclarationNode: public NonTerminalNode{
        public:
        explicit ConstantDeclarationNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class DeclaratorListNode: public NonTerminalNode{
        public:
        explicit DeclaratorListNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class InitDeclaratorListNode: public NonTerminalNode{
        public:
        explicit InitDeclaratorListNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class InitDeclaratorNode: public NonTerminalNode{
        public:
        explicit InitDeclaratorNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    //-----------------------------------------------------------------------------------------------------------------------
    class CompoundStatementNode: public NonTerminalNode{
        public:
        explicit CompoundStatementNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class StatementListNode: public NonTerminalNode{
        public:
        explicit StatementListNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class StatementNode: public NonTerminalNode{
        public:
        explicit StatementNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class AssignmentExpressionNode: public NonTerminalNode{
        public:
        explicit AssignmentExpressionNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class AdditiveExpressionNode: public NonTerminalNode{
        public:
        explicit AdditiveExpressionNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class MultiplicativeExpressionNode: public NonTerminalNode{
        public:
        explicit MultiplicativeExpressionNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class UnaryExpressionNode: public NonTerminalNode{
        public:
        explicit UnaryExpressionNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
    class PrimaryExpressionNode: public NonTerminalNode{
        public:
        explicit PrimaryExpressionNode(std::vector<std::unique_ptr<Node>> children);
        void accept(ParseTreeVisitor& visitor) const override;
    };
} // namespace parser
#endif //PLJIT_NODE_H
