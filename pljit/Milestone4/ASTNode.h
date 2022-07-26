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
            NoOperator,
            Constant,
            Parameter,
            Variable,
            LiteralConstant,
            FunctionDefinition,
            DeclaratorList,
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
    class ASTUnaryNode: public ASTNode{
        friend class SemanticAnalyzer;
        friend class ConstantPropagationPass;

        std::unique_ptr<ASTNode> child;
        protected:
        ASTUnaryNode(ASTNodeType type,std::unique_ptr<ASTNode> child): ASTNode(type),child(std::move(child)){}
        public:
        ASTNode* getChild() const{
            return child.get();
        }//only for testing
    };
    class ASTBinaryNode: public ASTNode {
        protected:
        std::unique_ptr<ASTNode> leftChild;
        std::unique_ptr<ASTNode> rightChild;
        ASTBinaryNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTNode(type),leftChild(std::move(left)),rightChild(std::move(right)){}
        public:
        ASTNode* getLeftChild() const{
            return leftChild.get();
        }//only for testing
        ASTNode* getRightChild() const{
            return rightChild.get();
        }//only for testing
    };
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
        }//only for testing
    };
    template<typename T>
    class ASTValueNode: public ASTNode{
        T value;
        public:
        ASTValueNode(ASTNodeType type, T value) : ASTNode(type), value(std::move(value)){};
        T getValue() const{return value;}
    };
    //------------------------------------------------------------------------------------------------------------------------------------
    class ASTFunctionNode: public MultiASTNode{
        friend class SemanticAnalyzer;
        public:
        ASTFunctionNode(): MultiASTNode(FunctionDefinition){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTIdentifierNode: public ASTValueNode<std::string>{
        public:
        ASTIdentifierNode(ASTNodeType type, std::string value): ASTValueNode<std::string>(type, std::move(value)){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    class ASTParamIdentifierNode: public ASTValueNode<std::string>{
        double paramValue;
        public:
        ASTParamIdentifierNode(std::string value, double paramValue): ASTValueNode<std::string>(Parameter, std::move(value)), paramValue(paramValue){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
        double getParamValue();
    };
    class ASTLiteralNode: public ASTValueNode<unsigned long>{
        public:
        explicit ASTLiteralNode(unsigned long value): ASTValueNode(LiteralConstant, value){};
        void accept(ASTTreeVisitor& visitor) const override;
        std::optional<double> acceptEvaluation(ASTEvaluator& visitor) override;
    };
    //----------------------------------------------------------------------------------------------------------------------------

    class ASTDeclaratorListNode: public MultiASTNode{
        public:
        ASTDeclaratorListNode(): MultiASTNode(DeclaratorList){};
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
    class ASTCompoundStatement: public MultiASTNode{
        public:
        ASTCompoundStatement(): MultiASTNode(CompoundStatement){};
        void pop_back_child(){ children.pop_back(); };
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

    class ASTOperationExpressionNode: public ASTBinaryNode{
        friend class ConstantPropagationPass;
        public:
        //rightChild could be nullptr
        /*
         * constructor with two children
         */
        ASTOperationExpressionNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTBinaryNode(type,std::move(left), std::move(right)){};
        /*
         * constructor with one children, if the optional component doesn't exist
         */
        explicit ASTOperationExpressionNode(std::unique_ptr<ASTNode> left): ASTBinaryNode(NoOperator,std::move(left), nullptr) {};
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
