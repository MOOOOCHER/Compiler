#ifndef PLJIT_ASTNODE_H
#define PLJIT_ASTNODE_H
#include <memory>
#include <utility>
#include <vector>
#include <optional>
namespace semantic{
    class SemanticAnalyzer;
    class ASTTreePrintVisitor;
    class ASTTreeVisitor;
    class ASTEvaluator;
    class ConstantPropagationPass;
    class DeadCodeEliminationPass;

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
            Parameter,
            Variable,
            LiteralConstant,
            FunctionDefinition,
            ParamDeclaratorList,
            VarDeclaratorList,
            InitDeclaratorList,
            InitDeclarator,
            CompoundStatement,
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
    /*
     * AST node class for nodes with one child
     */
    class ASTUnaryNode: public ASTNode{
        protected:
        std::unique_ptr<ASTNode> child;
        ASTUnaryNode(ASTNodeType type,std::unique_ptr<ASTNode> child);
        public:
        std::unique_ptr<ASTNode> getChild(){
            return std::move(child);
        }
        void accept(ASTTreeVisitor& visitor) const override = 0;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override = 0;
    };
    /*
     * AST node class for nodes with multiple children
     */
    class MultiASTNode: public ASTNode{
        friend class SemanticAnalyzer;
        friend class ASTTreePrintVisitor;
        protected:
        std::vector<std::unique_ptr<ASTNode>> children;
        explicit MultiASTNode(ASTNodeType type);
        public:
        std::vector<std::unique_ptr<ASTNode>> getChildren(){
            //Only for testing
            return std::move(children);
        }
        void accept(ASTTreeVisitor& visitor) const override = 0;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override = 0;
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
    class ASTFunctionNode: public MultiASTNode{
        friend class SemanticAnalyzer;
        friend class DeadCodeEliminationPass;
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        ASTFunctionNode();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTParamDeclaratorListNode: public MultiASTNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        explicit ASTParamDeclaratorListNode();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTVarDeclaratorListNode: public MultiASTNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        explicit ASTVarDeclaratorListNode();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTInitDeclaratorListNode: public MultiASTNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        ASTInitDeclaratorListNode();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTInitDeclaratorNode: public ASTNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        std::unique_ptr<ASTNode> leftChild;
        std::unique_ptr<ASTNode> rightChild;
        public:
        ASTInitDeclaratorNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right);
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    //---------------------------------------------------------------------------------------
    class ASTCompoundStatement: public MultiASTNode{
        friend class ConstantPropagationPass;
        friend class DeadCodeEliminationPass;
        friend class ASTTreePrintVisitor;
        void pop_back_child();
        public:
        ASTCompoundStatement();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTStatementNode: public ASTUnaryNode{
        friend class ConstantPropagationPass;
        friend class ASTTreePrintVisitor;
        public:
        ASTStatementNode(ASTNodeType type, std::unique_ptr<ASTNode> child);
        void accept(ASTTreeVisitor& visitor) const override = 0;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override = 0;
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
        public:
        ASTOperationExpressionNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right);
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
