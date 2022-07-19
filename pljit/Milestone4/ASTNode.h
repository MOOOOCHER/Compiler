#ifndef PLJIT_ASTNODE_H
#define PLJIT_ASTNODE_H
#include <memory>
#include <vector>
namespace semantic{
    class SemanticAnalyzer;
    class ASTNode{
        friend class SemanticAnalyzer;
        friend class ASTSymbolTable;
        protected:
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
        ASTNodeType type;

        public:
        virtual ~ASTNode() = default;
        ASTNodeType getType() { return type;}
        ASTNode(ASTNodeType type): type(type){}
    };
    class ASTUnaryNode: public ASTNode{
        friend class SemanticAnalyzer;
        std::unique_ptr<ASTNode> child;
        protected:
        ASTUnaryNode(ASTNodeType type,std::unique_ptr<ASTNode> child): ASTNode(type),child(std::move(child)){}
    };
    class BinaryASTNode: public ASTNode {
        protected:
        std::unique_ptr<ASTNode> leftChild;
        std::unique_ptr<ASTNode> rightChild;
        BinaryASTNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): ASTNode(type),leftChild(std::move(left)),rightChild(std::move(right)){}
    };
    class MultiASTNode: public ASTNode {
        friend class SemanticAnalyzer;
        protected:
        std::vector<std::unique_ptr<ASTNode>> children;
        MultiASTNode(ASTNodeType type): ASTNode(type){}
    };
    template<typename T>
    class ASTValueNode: public ASTNode{
        T value;
        public:
        ASTValueNode(ASTNodeType type, T value) : ASTNode(type), value(value){};
        T getValue(){return value;}
    };
    //------------------------------------------------------------------------------------------------------------------------------------
    class ASTFunctionNode: public MultiASTNode{
        friend class SemanticAnalyzer;
        public:
        ASTFunctionNode(): MultiASTNode(FunctionDefinition){};
    };
    class ASTIdentifierNode: public ASTValueNode<std::string>{
        public:
        ASTIdentifierNode(ASTNodeType type, std::string value): ASTValueNode<std::string>(type, std::move(value)){};
    };
    class ASTLiteralNode: public ASTValueNode<unsigned long>{
        public:
        ASTLiteralNode(unsigned long value): ASTValueNode(LiteralConstant, std::move(value)){};
    };
    //----------------------------------------------------------------------------------------------------------------------------

    class ASTDeclaratorListNode: public MultiASTNode{
        public:
        ASTDeclaratorListNode(): MultiASTNode(DeclaratorList){};
    };
    class ASTInitDeclaratorListNode: public MultiASTNode{
        public:
        ASTInitDeclaratorListNode(): MultiASTNode(InitDeclaratorList){};
    };
    class ASTInitDeclaratorNode: public BinaryASTNode{
        public:
        ASTInitDeclaratorNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): BinaryASTNode(InitDeclarator,std::move(left), std::move(right)){};
    };
    class ASTCompoundStatement: public MultiASTNode{
        public:
        ASTCompoundStatement(): MultiASTNode(CompoundStatement){};
    };
    class ASTStatementNode: public ASTUnaryNode{
        public:
        ASTStatementNode(ASTNodeType type, std::unique_ptr<ASTNode> child): ASTUnaryNode(type, std::move(child)){};
    };

    class ASTOperationExpressionNode: public BinaryASTNode{
        //rightChild could be nullptr
        public:
        /*
         * constructor with two children
         */
        ASTOperationExpressionNode(ASTNodeType type,std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): BinaryASTNode(type,std::move(left), std::move(right)){};
        /*
         * constructor with one children, if the optional component doesn't exist
         */
        ASTOperationExpressionNode(std::unique_ptr<ASTNode> left): BinaryASTNode(NoOperator,std::move(left), nullptr) {};
    };
    class ASTAssignmentExpression: public BinaryASTNode{
        public:
        ASTAssignmentExpression(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode>right): BinaryASTNode(AssignmentExpression,std::move(left), std::move(right)){}
    };
    class ASTUnaryExpression: public ASTUnaryNode{
        //type should only be UnaryPlus or UnaryMinus
        public:
        ASTUnaryExpression(ASTNodeType type,std::unique_ptr<ASTNode> child): ASTUnaryNode(type, std::move(child)){};
    };
    class ASTPrimaryExpression: public ASTUnaryNode{
        public:
        ASTPrimaryExpression(std::unique_ptr<ASTNode> child): ASTUnaryNode(PrimaryExpression, std::move(child)){};
    };
} // namespace semantic
#endif //PLJIT_ASTNODE_H
