#ifndef PLJIT_ASTNODE_H
#define PLJIT_ASTNODE_H
namespace semantic{
    class ASTNode{
        enum ASTNodeType{
            PlusOperator,
            MinusOperator,
            MulOperator,
            DivOperator,
            InitEquals,
            AssignEquals,
        };
        ASTNodeType type;
        public:
        virtual ~ASTNode();
        ASTNodeType getType() { return type;}
    };

    class ASTFunctionNode: public ASTNode{

    };
    class ASTStatementNode: public ASTNode{

    };
    class ASTExpressionNode: public ASTNode{

    };
    class ASTOperationNode: public ASTNode{

    };
    class ASTAdditiveExpression: public ASTNode{

    };
    class AStMultiplicativeExpression: public ASTNode{

    };
} // namespace semantic
#endif //PLJIT_ASTNODE_H
