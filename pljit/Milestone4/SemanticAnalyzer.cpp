#include "SemanticAnalyzer.h"
namespace semantic {
using NodeType = parser::Node::Types;

std::unique_ptr<ASTFunctionNode> SemanticAnalyzer::analyzeFunction(parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTFunctionNode> node = std::make_unique<ASTFunctionNode>();
    if(parseNode.getType() == NodeType::FunctionDefinition){
        for(auto child: parseNode.getChildren()){
            if(child->getType() == NodeType::ParameterDeclaration){
                parser::NonTerminalNode* nonTerminalNode = static_cast<parser::NonTerminalNode*>(child);
                auto astParam = analyzeParameterDeclaration(*nonTerminalNode);
                if(!astParam) return nullptr;
                node->children.push_back(std::move(astParam));
            } else if(child->getType() == NodeType::VariableDeclaration){
                parser::NonTerminalNode* nonTerminalNode = static_cast<parser::NonTerminalNode*>(child);
                auto astVar = analyzeVariableDeclaration(*nonTerminalNode);
                if(!astVar) return nullptr;
                node->children.push_back(std::move(astVar));
            } else if(child->getType() == NodeType::ConstantDeclaration){
                parser::NonTerminalNode* nonTerminalNode = static_cast<parser::NonTerminalNode*>(child);
                auto astConst = analyzeConstantDeclaration(*nonTerminalNode);
                if(!astConst) return nullptr;
                node->children.push_back(std::move(astConst));
            } else if(child->getType() == NodeType::CompoundStatement){
                parser::NonTerminalNode* nonTerminalNode = static_cast<parser::NonTerminalNode*>(child);
                auto astComp = analyzeCompoundStatement(*nonTerminalNode);
                if(!astComp) return nullptr;
                node->children.push_back(std::move(astComp));
            }
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<ASTDeclaratorListNode> SemanticAnalyzer::analyzeParameterDeclaration(parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTDeclaratorListNode> node = std::make_unique<ASTDeclaratorListNode>();
    auto declList = parseNode.getChildren()[1];
    if(declList->getType() == NodeType::DeclaratorList){
        parser::NonTerminalNode* decl = static_cast<parser::NonTerminalNode*>(declList);
        for(auto child: decl->getChildren()){
            if(child->getType() == NodeType::Identifier){
                parser::IdentifierNode* identifierNode = static_cast<parser::IdentifierNode*>(child);
                auto astIdentifier = analyzeInitIdentifier(ASTNode::ASTNodeType::Parameter,*identifierNode);
                if(!astIdentifier) return nullptr;
                node->children.push_back(std::move(astIdentifier));
            }
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<ASTDeclaratorListNode> SemanticAnalyzer::analyzeVariableDeclaration(parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTDeclaratorListNode> node = std::make_unique<ASTDeclaratorListNode>();
    auto declList = parseNode.getChildren()[1];
    if(declList->getType() == NodeType::DeclaratorList){
        parser::NonTerminalNode* decl = static_cast<parser::NonTerminalNode*>(declList);
        for(auto child: decl->getChildren()){
            if(child->getType() == NodeType::Identifier){
                parser::IdentifierNode* identifierNode = static_cast<parser::IdentifierNode*>(child);
                auto astIdentifier = analyzeInitIdentifier(ASTNode::ASTNodeType::Variable,*identifierNode);
                if(!astIdentifier) return nullptr;
                node->children.push_back(std::move(astIdentifier));
            }
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<ASTInitDeclaratorListNode> SemanticAnalyzer::analyzeConstantDeclaration(parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTInitDeclaratorListNode> node = std::make_unique<ASTInitDeclaratorListNode>();
    auto declList = parseNode.getChildren()[1];
    if(declList->getType() == NodeType::InitDeclaratorList){
        parser::NonTerminalNode* decl = static_cast<parser::NonTerminalNode*>(declList);
        for(auto child: decl->getChildren()){
            if(child->getType() == NodeType::InitDeclarator){
                parser::NonTerminalNode* identifierNode = static_cast<parser::NonTerminalNode*>(child);
                auto astIdentifier = analyzeInitDeclarator(*identifierNode);
                if(!astIdentifier) return nullptr;
                node->children.push_back(std::move(astIdentifier));
            }
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<ASTNode> SemanticAnalyzer::analyzeInitDeclarator(parser::NonTerminalNode& parseNode){
    if(parseNode.getType() == NodeType::InitDeclarator){
        parser::IdentifierNode* identifierNode = static_cast<parser::IdentifierNode*>(parseNode.getChildren()[0]);
        auto astIdentifier = analyzeInitIdentifier(ASTNode::ASTNodeType::Constant,*identifierNode);
        if(!astIdentifier) return nullptr;

        parser::LiteralNode* literal = static_cast<parser::LiteralNode*>(parseNode.getChildren()[2]);
        auto astLiteral = std::make_unique<ASTLiteralNode>(literal->getValue());
        std::unique_ptr<ASTInitDeclaratorNode> node = std::make_unique<ASTInitDeclaratorNode>(std::move(astIdentifier),std::move(astLiteral));
        return node;
    }
    return nullptr;
}
std::unique_ptr<ASTCompoundStatement> SemanticAnalyzer::analyzeCompoundStatement(parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTCompoundStatement> node = std::make_unique<ASTCompoundStatement>();
    auto declList = parseNode.getChildren()[1];
    bool hasReturnStatement = false;
    if(declList->getType() == NodeType::StatementList){
        parser::NonTerminalNode* decl = static_cast<parser::NonTerminalNode*>(declList);
        for(auto child: decl->getChildren()){
            if(child->getType() == NodeType::Statement){
                parser::NonTerminalNode* statementNode = static_cast<parser::NonTerminalNode*>(child);
                auto astStatement = analyzeStatement(*statementNode);
                if(!astStatement) return nullptr;
                else if(astStatement->getType() == ASTNode::ReturnStatement) hasReturnStatement = true;
                node->children.push_back(std::move(astStatement));
            }
        }
        if(!hasReturnStatement) {
            sourceCodeManagement::SourceCodeManager defaultManager = sourceCodeManagement::SourceCodeManager();
            SourceCodeReference a = SourceCodeReference (defaultManager);
            a.printContext("error: missing return statement");
            return nullptr;
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<ASTStatementNode> SemanticAnalyzer::analyzeStatement(parser::NonTerminalNode& parseNode){
    if(parseNode.getChildren().size() == 2){   //case assignment expression
        parser::NonTerminalNode* childNode = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[1]);
        auto childAST = analyzeExpression(*childNode);
        if(!childAST) return nullptr;
        return std::make_unique<ASTStatementNode>(ASTNode::ASTNodeType::ReturnStatement, std::move(childAST));
    } else {
        parser::NonTerminalNode* childNode = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[0]);
        auto childAST = analyzeExpression(*childNode);
        if(!childAST) return nullptr;
        return std::make_unique<ASTStatementNode>(ASTNode::ASTNodeType::AssignStatement, std::move(childAST));
    }
}
std::unique_ptr<ASTNode> SemanticAnalyzer::analyzeExpression(parser::NonTerminalNode& parseNode){
    NodeType parseType = parseNode.getType();
    if(parseType == NodeType::AssignmentExpression){
        parser::IdentifierNode* childIdentifier = static_cast<parser::IdentifierNode*>(parseNode.getChildren()[0]);
        parser::NonTerminalNode* childExpression = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[2]);
        if(table.contains(childIdentifier->getReference().getText()) && table.get(childIdentifier->getReference().getText())->identifierType == ASTNode::Constant){
            childIdentifier->getReference().printContext("error: cannot assign a value to a constant!");
            return nullptr;
        }
        auto identifier = analyzeIdentifier(*childIdentifier);
        auto expression = analyzeExpression(*childExpression);
        if(!identifier || !expression) return nullptr;
        return std::make_unique<ASTAssignmentExpression>(std::move(identifier),std::move(expression));
    } else if(parseType == NodeType::AdditiveExpression || parseType == NodeType::MultiplicativeExpression){
        if(parseNode.getChildren().size() == 1){    // if we only have one child we can omit this level
            parser::NonTerminalNode* childExpression = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[0]);
            return analyzeExpression(*childExpression);
        } else {
            auto operatorType = parseNode.getChildren()[1]->getType();
            parser::NonTerminalNode* firstExpr = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[0]);
            parser::NonTerminalNode* sndExpr = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[2]);
            auto astFirstExpr = analyzeExpression(*firstExpr);
            auto astSndExpr = analyzeExpression(*sndExpr);
            if(!astFirstExpr || !astSndExpr) return nullptr;
            else if(operatorType == NodeType::PlusOperator){
                return std::make_unique<ASTOperationExpressionNode>(ASTNode::ASTNodeType::PlusOperator,std::move(astFirstExpr),std::move(astSndExpr));
            }else if(operatorType == NodeType::MinusOperator) {
                return std::make_unique<ASTOperationExpressionNode>(ASTNode::ASTNodeType::MinusOperator,std::move(astFirstExpr),std::move(astSndExpr));
            }else if(operatorType == NodeType::MulOperator){
                return std::make_unique<ASTOperationExpressionNode>(ASTNode::ASTNodeType::MulOperator,std::move(astFirstExpr),std::move(astSndExpr));
            }else if(operatorType == NodeType::DivOperator){
                return std::make_unique<ASTOperationExpressionNode>(ASTNode::ASTNodeType::DivOperator,std::move(astFirstExpr),std::move(astSndExpr));
            }
        }
    } else if(parseType == NodeType::UnaryExpression){
        if(parseNode.getChildren().size() == 2){
            NodeType opType = parseNode.getChildren()[0]->getType();
            parser::NonTerminalNode* expr = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[1]);
            auto child = analyzeExpression(*expr);
            if(!child) return nullptr;
            if(opType == NodeType::MinusOperator){
                return std::make_unique<ASTUnaryExpression>(ASTNode::ASTNodeType::UnaryMinus, std::move(child));
            } else {
                return std::make_unique<ASTUnaryExpression>(ASTNode::ASTNodeType::UnaryPlus, std::move(child));
            }
        }
        parser::NonTerminalNode* expr = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[0]);
        return analyzeExpression(*expr);
    } else if(parseType == NodeType::PrimaryExpression){
        if(parseNode.getChildren().size() > 1){
            parser::NonTerminalNode* expr = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[1]);
            return analyzeExpression(*expr);
        } else {
            if(parseNode.getChildren()[0]->getType() == NodeType::Identifier){
                parser::IdentifierNode* identifier = static_cast<parser::IdentifierNode*>(parseNode.getChildren()[0]);
                return analyzeIdentifier(*identifier);
            } else {
                parser::LiteralNode* literal = static_cast<parser::LiteralNode*>(parseNode.getChildren()[0]);
                return std::make_unique<ASTLiteralNode>(literal->getValue());
            }
        }
    }
    return nullptr;
}
std::unique_ptr<ASTIdentifierNode> SemanticAnalyzer::analyzeInitIdentifier(ASTNode::ASTNodeType type, parser::IdentifierNode& parseNode){
    if(table.contains(parseNode.getReference().getText())){
        //double declaration
        parseNode.getReference().printContext("error: '"+ parseNode.getReference().getText() +"' is already declared!");
        return nullptr;
    }
    table.insert(type,parseNode.getReference());
    return std::make_unique<ASTIdentifierNode>(type,parseNode.getReference().getText());
}
std::unique_ptr<ASTIdentifierNode> SemanticAnalyzer::analyzeIdentifier(parser::IdentifierNode& parseNode){
    auto entry = table.get(parseNode.getReference().getText());
    if(!entry){
        //undeclared identifier
        parseNode.getReference().printContext("error: '"+ parseNode.getReference().getText() +"' is undeclared!");
        return nullptr;
    }
    ASTNode::ASTNodeType type = entry->identifierType;
    return std::make_unique<ASTIdentifierNode>(type,parseNode.getReference().getText());
}
} //namespace semantic