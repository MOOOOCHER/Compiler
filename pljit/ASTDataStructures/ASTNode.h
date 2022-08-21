#ifndef PLJIT_ASTNODE_H
#define PLJIT_ASTNODE_H
#include "pljit/Milestone1/SourceCodeManager.h"
#include <memory>
#include <optional>
#include <vector>

namespace semantic{
    class SemanticAnalyzer;
    class ASTTreePrintVisitor;
    class ASTTreeVisitor;
    class ASTEvaluator;
    class ASTSymbolTable;
    class ConstantPropagationPass;
    class DeadCodeEliminationPass;
    class AssociationPass;
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
            AssignmentExpression,
            BracketExpression
        };
        protected:
        ASTNodeType type;
        public:
        explicit ASTNode(ASTNodeType type): type(type){}
        virtual ~ASTNode() = default;
        virtual void accept(ASTTreeVisitor& visitor) const = 0;
        virtual std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const = 0;
        ASTNodeType getType() const{ return type;}
    };
    //------------------------------------------------------------------------------------------------------------------------------------
    class ASTIdentifierNode: public ASTNode{
        std::string_view value;
        public:
        ASTIdentifierNode(ASTNodeType type, std::string_view value);

        std::string_view getValue() const;
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override;
    };
    class ASTLiteralNode: public ASTNode{
        double value;
        public:
        explicit ASTLiteralNode(double value);

        double getValue() const;
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override;
    };
    //----------------------------------------------------------------------
    class ASTFunctionNode: public ASTNode{
        friend class SemanticAnalyzer;
        friend class DeadCodeEliminationPass;
        friend class ConstantPropagationPass;
        friend class AssociationPass;
        friend class ASTTreePrintVisitor;

        std::vector<std::unique_ptr<ASTNode>> children;
        public:
        ASTFunctionNode();

        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override;
        std::vector<std::unique_ptr<ASTNode>> getChildren();
        void pop_back_child();
    };
    //----------------------------------------------------------------------------------------------------------------------------
    class ASTStatementNode: public ASTNode{
        friend class ConstantPropagationPass;
        friend class AssociationPass;
        friend class ASTTreePrintVisitor;
        protected:
        std::unique_ptr<ASTNode> child;
        public:
        ASTStatementNode(ASTNodeType type, std::unique_ptr<ASTNode> child);

        void accept(ASTTreeVisitor& visitor) const override = 0;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override = 0;
        std::unique_ptr<ASTNode> getChild();
    };
    class ASTReturnStatementNode: public ASTStatementNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        explicit ASTReturnStatementNode(std::unique_ptr<ASTNode> child);

        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override;
    };
    class ASTAssignmentStatementNode: public ASTStatementNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        explicit ASTAssignmentStatementNode(std::unique_ptr<ASTNode> child);

        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override;
    };
    //-------------------------------------------------------------------------------------------------------------
    class ASTExpressionNode: public ASTNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        explicit ASTExpressionNode(ASTNodeType type);

        void accept(ASTTreeVisitor& visitor) const override = 0;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override = 0;
    };
    class ASTOperationExpressionNode: public ASTExpressionNode{
        friend class ConstantPropagationPass;
        friend class AssociationPass;
        friend class ASTTreePrintVisitor;
        std::unique_ptr<ASTNode> leftChild;
        std::unique_ptr<ASTNode> rightChild;
        public:
        ASTOperationExpressionNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right);
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override;
    };
    /*
     * this expression type is used for unary-plus, unary-minus and expression containing (expr)
     */
    class ASTUnaryExpression: public ASTExpressionNode{
        friend class ConstantPropagationPass;
        friend class AssociationPass;
        friend class ASTTreePrintVisitor;
        std::unique_ptr<ASTNode> child;
        //type should only be UnaryPlus or UnaryMinus
        public:
        ASTUnaryExpression(ASTNodeType type,std::unique_ptr<ASTNode> child);

        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override;
    };
    /*
     * this expression type is used for unary-plus, unary-minus and expression containing (expr)
     */
    class ASTBracketExpression: public ASTExpressionNode{
        friend class ConstantPropagationPass;
        friend class AssociationPass;
        friend class ASTTreePrintVisitor;
        std::unique_ptr<ASTNode> child;
        public:
        explicit ASTBracketExpression(std::unique_ptr<ASTNode> child);

        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override;
    };
    class ASTAssignmentExpression: public ASTExpressionNode{
        friend class ConstantPropagationPass;
        friend class AssociationPass;
        friend class ASTTreePrintVisitor;
        std::unique_ptr<ASTNode> leftChild;
        std::unique_ptr<ASTNode> rightChild;
        public:
        ASTAssignmentExpression(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right);

        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) const override;
    };
} // namespace semantic
#endif //PLJIT_ASTNODE_H
