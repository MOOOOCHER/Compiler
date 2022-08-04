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
        friend class SemanticAnalyzer;
        friend class ConstantPropagationPass;
        protected:
        std::unique_ptr<ASTNode> child;
        ASTUnaryNode(ASTNodeType type,std::unique_ptr<ASTNode> child);
        public:
        ASTNode* getChild() const;
    };
    /*
     * AST node class for nodes with two children
     */
    class ASTBinaryNode: public ASTNode {
        protected:
        std::unique_ptr<ASTNode> leftChild;
        std::unique_ptr<ASTNode> rightChild;
        ASTBinaryNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right);
        public:
        ASTNode* getLeftChild() const;
        ASTNode* getRightChild() const;
    };
    /*
     * AST node class for nodes with multiple children
     */
    class MultiASTNode: public ASTNode{
        friend class SemanticAnalyzer;

        protected:
        std::vector<std::unique_ptr<ASTNode>> children;
        explicit MultiASTNode(ASTNodeType type);

        public:
        std::vector<ASTNode*> getChildren() const;
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
        public:
        ASTFunctionNode();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTParamDeclaratorListNode: public MultiASTNode{
        public:
        explicit ASTParamDeclaratorListNode();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTVarDeclaratorListNode: public MultiASTNode{
        public:
        explicit ASTVarDeclaratorListNode();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTInitDeclaratorListNode: public MultiASTNode{
        public:
        ASTInitDeclaratorListNode();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTInitDeclaratorNode: public ASTBinaryNode{
        public:
        ASTInitDeclaratorNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right);
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    //---------------------------------------------------------------------------------------
    class ASTCompoundStatement: public MultiASTNode{
        friend class DeadCodeEliminationPass;
        void pop_back_child();
        public:
        ASTCompoundStatement();
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTStatementNode: public ASTUnaryNode{
        friend class ConstantPropagationPass;
        public:
        ASTStatementNode(ASTNodeType type, std::unique_ptr<ASTNode> child);
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    //-------------------------------------------------------------------------------------------------------------
    class ASTOperationExpressionNode: public ASTBinaryNode{
        friend class ConstantPropagationPass;
        public:
        ASTOperationExpressionNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right);
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTAssignmentExpression: public ASTBinaryNode{
        friend class ConstantPropagationPass;
        public:
        ASTAssignmentExpression(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right);
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTUnaryExpression: public ASTUnaryNode{
        //type should only be UnaryPlus or UnaryMinus
        public:
        ASTUnaryExpression(ASTNodeType type,std::unique_ptr<ASTNode> child);
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
} // namespace semantic
#endif //PLJIT_ASTNODE_H
