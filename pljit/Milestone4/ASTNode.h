#ifndef PLJIT_ASTNODE_H
#define PLJIT_ASTNODE_H
#include <memory>
#include <utility>
#include <vector>
#include <optional>
#include "../Milestone1/SourceCodeManager.h"

namespace semantic{
    class SemanticAnalyzer;
    class ASTTreePrintVisitor;
    class ASTTreeVisitor;
    class ASTEvaluator;
    class ConstantPropagationPass;
    class ASTSymbolTable;
    class DeadCodeEliminationPass;
    using SourceCodeReference = sourceCodeManagement::SourceCodeReference;
    class ASTNode{
        friend class SemanticAnalyzer;
        friend class ASTSymbolTable;
        friend class ASTTreePrintVisitor;
        public:
        enum ASTNodeType{
            PlusOperator,
            MinusOperator,
            MulOperator,
            DivOperator,
            Constant,
            Variable,
            Parameter,
            LiteralConstant,
            FunctionDefinition,
            AssignStatement,
            ReturnStatement,
            UnaryPlus,
            UnaryMinus,
            PrimaryExpression,
            AssignmentExpression
        };
        protected:
        ASTNodeType type;
        public:
        ASTNodeType getType() const{ return type;}
        explicit ASTNode(ASTNodeType type): type(type){}
        virtual ~ASTNode() = default;
        virtual void accept(ASTTreeVisitor& visitor) const = 0;
        virtual std::optional<double> acceptEvaluation(ASTEvaluator& visitor) = 0;
    };
    //----------------------------------------------------------------------
    class ASTFunctionNode: public ASTNode{
        friend class SemanticAnalyzer;
        friend class DeadCodeEliminationPass;
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        std::vector<std::unique_ptr<ASTNode>> children;
        public:
        ASTFunctionNode();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
        void pop_back_child();
        std::vector<std::unique_ptr<ASTNode>> getChildren();
    };

    //------------------------------------------------------------------------------------------------------------------------------------
    class ASTIdentifierNode: public ASTNode{
        protected:
        std::string_view value;
        public:
        ASTIdentifierNode(ASTNodeType type, std::string_view value);
        std::string_view getValue() const;
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTLiteralNode: public ASTNode{
        double value;
        public:
        explicit ASTLiteralNode(double value);
        double getValue() const;
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    //----------------------------------------------------------------------------------------------------------------------------
    class ASTStatementNode: public ASTNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        protected:
        std::unique_ptr<ASTNode> child;
        public:
        ASTStatementNode(ASTNodeType type, std::unique_ptr<ASTNode> child);
        void accept(ASTTreeVisitor& visitor) const override = 0;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override = 0;
        std::unique_ptr<ASTNode> getChild();
    };
    class ASTReturnStatementNode: public ASTStatementNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        explicit ASTReturnStatementNode(std::unique_ptr<ASTNode> child): ASTStatementNode(ReturnStatement,std::move(child)){}
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTAssignmentStatementNode: public ASTStatementNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        explicit ASTAssignmentStatementNode(std::unique_ptr<ASTNode> child): ASTStatementNode(AssignStatement,std::move(child)){}
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    //-------------------------------------------------------------------------------------------------------------
    class ASTExpressionNode: public ASTNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        explicit ASTExpressionNode(ASTNodeType type): ASTNode(type){}
        void accept(ASTTreeVisitor& visitor) const override = 0;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override = 0;
    };
    class ASTOperationExpressionNode: public ASTExpressionNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        std::unique_ptr<ASTNode> leftChild;
        std::unique_ptr<ASTNode> rightChild;
        SourceCodeReference sourceCodeReference;
        public:
        ASTOperationExpressionNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right, SourceCodeReference sourceCodeReference);
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTUnaryExpression: public ASTExpressionNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        std::unique_ptr<ASTNode> child;
        //type should only be UnaryPlus or UnaryMinus
        public:
        ASTUnaryExpression(ASTNodeType type,std::unique_ptr<ASTNode> child);
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTAssignmentExpression: public ASTExpressionNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        std::unique_ptr<ASTNode> leftChild;
        std::unique_ptr<ASTNode> rightChild;
        public:
        ASTAssignmentExpression(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right);
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
} // namespace semantic
#endif //PLJIT_ASTNODE_H
