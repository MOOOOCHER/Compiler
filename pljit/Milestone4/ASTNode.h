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
        virtual ~ASTNode() = default;
        ASTNodeType getType() const{ return type;}
        explicit ASTNode(ASTNodeType type): type(type){}
        virtual void accept(ASTTreeVisitor& visitor)const = 0;
        virtual std::optional<double> acceptEvaluation(ASTEvaluator& visitor) = 0;
    };
    //----------------------------------------------------------------------
    /*
     * AST node class for nodes with one child
     */
    class ASTUnaryNode: public ASTNode{
        friend class SemanticAnalyzer;
        friend class ConstantPropagationPass;
        std::unique_ptr<ASTNode> child;
        protected:
        ASTUnaryNode(ASTNodeType type,std::unique_ptr<ASTNode> child): ASTNode(type),child(std::move(child)){}
        public:
        ASTNode* getChild() const{
            return child.get();
        }
    };
    /*
     * AST node class for nodes with two children
     */
    class ASTBinaryNode: public ASTNode {
        protected:
        std::unique_ptr<ASTNode> leftChild;
        std::unique_ptr<ASTNode> rightChild;
        ASTBinaryNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTNode(type),leftChild(std::move(left)),rightChild(std::move(right)){}
        public:
        ASTNode* getLeftChild() const{
            return leftChild.get();
        }
        ASTNode* getRightChild() const{
            return rightChild.get();
        }
    };
    /*
     * AST node class for nodes with multiple children
     */
    class MultiASTNode: public ASTNode {
        friend class SemanticAnalyzer;
        protected:
        std::vector<std::unique_ptr<ASTNode>> children;
        explicit MultiASTNode(ASTNodeType type): ASTNode(type){}
        public:
        std::vector<ASTNode*> getChildren() const{
            std::vector<ASTNode*> vec;
            for(auto& child: children){
                vec.push_back(child.get());
            }
            return vec;
        }
    };
    //------------------------------------------------------------------------------------------------------------------------------------
    class ASTIdentifierNode: public ASTNode{
        std::string_view value;
        public:
        ASTIdentifierNode(ASTNodeType type, std::string_view value): ASTNode(type), value(value){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
        std::string_view getValue() const { return value; }
    };
    class ASTParamIdentifierNode: public ASTIdentifierNode{
        friend class ASTEvaluator;
        double paramValue = 0;
        public:
        explicit ASTParamIdentifierNode(std::string_view value): ASTIdentifierNode(Parameter, value){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTLiteralNode: public ASTNode{
        double value;
        public:
        explicit ASTLiteralNode(double value): ASTNode(LiteralConstant), value(value){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
        double getValue() const{ return value; }
    };
    //----------------------------------------------------------------------------------------------------------------------------
    class ASTFunctionNode: public MultiASTNode{
        friend class SemanticAnalyzer;
        public:
        ASTFunctionNode(): MultiASTNode(FunctionDefinition){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTDeclaratorListNode: public MultiASTNode{
        public:
        explicit ASTDeclaratorListNode(ASTNodeType type): MultiASTNode(type){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTInitDeclaratorListNode: public MultiASTNode{
        public:
        ASTInitDeclaratorListNode(): MultiASTNode(InitDeclaratorList){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTInitDeclaratorNode: public ASTBinaryNode{
        public:
        ASTInitDeclaratorNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTBinaryNode(InitDeclarator,std::move(left), std::move(right)){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    //---------------------------------------------------------------------------------------
    class ASTCompoundStatement: public MultiASTNode{
        friend class DeadCodeEliminationPass;
        void pop_back_child(){ children.pop_back(); };
        public:
        ASTCompoundStatement(): MultiASTNode(CompoundStatement){};
        /*
         * public function
         */
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTStatementNode: public ASTUnaryNode{
        friend class ConstantPropagationPass;
        public:
        ASTStatementNode(ASTNodeType type, std::unique_ptr<ASTNode> child): ASTUnaryNode(type, std::move(child)){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    //-------------------------------------------------------------------------------------------------------------
    class ASTOperationExpressionNode: public ASTBinaryNode{
        friend class ConstantPropagationPass;
        public:
        ASTOperationExpressionNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTBinaryNode(type,std::move(left), std::move(right)){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTAssignmentExpression: public ASTBinaryNode{
        friend class ConstantPropagationPass;
        public:
        ASTAssignmentExpression(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTBinaryNode(AssignmentExpression,std::move(left), std::move(right)){}
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTUnaryExpression: public ASTUnaryNode{
        //type should only be UnaryPlus or UnaryMinus
        public:
        ASTUnaryExpression(ASTNodeType type,std::unique_ptr<ASTNode> child): ASTUnaryNode(type, std::move(child)){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
} // namespace semantic
#endif //PLJIT_ASTNODE_H
