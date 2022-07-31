#include "SemanticAnalyzer.h"
namespace semantic {
using NodeType = parser::Node::Types;

std::unique_ptr<ASTFunctionNode> SemanticAnalyzer::analyzeFunction(const std::vector<long>& arg,parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTFunctionNode> node = std::make_unique<ASTFunctionNode>();
    if(parseNode.getType() == NodeType::FunctionDefinition){
        for(auto child: parseNode.getChildren()){
            if(child->getType() == NodeType::ParameterDeclaration){
                parser::NonTerminalNode* nonTerminalNode = static_cast<parser::NonTerminalNode*>(child);
                auto astParam = analyzeParameterDeclaration(arg,*nonTerminalNode);
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
std::unique_ptr<ASTDeclaratorListNode> SemanticAnalyzer::analyzeParameterDeclaration(const std::vector<long>& arg,parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTDeclaratorListNode> node = std::make_unique<ASTDeclaratorListNode>();
    auto declList = parseNode.getChildren()[1];
    if(declList->getType() == NodeType::DeclaratorList){
        parser::NonTerminalNode* decl = static_cast<parser::NonTerminalNode*>(declList);
        if(decl->getChildren().size()/2+1 != arg.size()){   //we should have arg.size()-1 commas as children => decl->getChildren().size()/2+1 when they are not included
            //if parameters size don't match
            sourceCodeManagement::SourceCodeManager defaultManager = sourceCodeManagement::SourceCodeManager();
            SourceCodeReference a = SourceCodeReference (defaultManager);
            a.printContext("error: parameter size doesn't match with argument size!");
            return nullptr;
        }
        for(auto child: decl->getChildren()){
            if(child->getType() == NodeType::Identifier){
                parser::IdentifierNode* identifierNode = static_cast<parser::IdentifierNode*>(child);
                auto astIdentifier = analyzeInitIdentifier(ASTNode::Parameter,*identifierNode);
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
                else if(astStatement->getType() == ASTNode::ReturnStatement) hasReturnStatement = true; //check for return statement
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
    if(parseNode.getChildren().size() == 2){   //case return expression
        parser::NonTerminalNode* childNode = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[1]);
        auto childAST = analyzeExpression(*childNode);
        if(!childAST) return nullptr;
        return std::make_unique<ASTStatementNode>(ASTNode::ASTNodeType::ReturnStatement, std::move(childAST));
    } else {    //case assignment
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
            //in case there are +/- signs before the expression
            NodeType opType = parseNode.getChildren()[0]->getType();
            //get child expression
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
            //case (expr)
            parser::NonTerminalNode* expr = static_cast<parser::NonTerminalNode*>(parseNode.getChildren()[1]);
            return analyzeExpression(*expr);
        } else {
            if(parseNode.getChildren()[0]->getType() == NodeType::Identifier){
                //case identifier
                parser::IdentifierNode* identifier = static_cast<parser::IdentifierNode*>(parseNode.getChildren()[0]);
                return analyzeIdentifier(*identifier);
            } else {
                //case literal
                parser::LiteralNode* literal = static_cast<parser::LiteralNode*>(parseNode.getChildren()[0]);
                return std::make_unique<ASTLiteralNode>(literal->getValue());
            }
        }
    }
    return nullptr;
}
std::unique_ptr<ASTNode> SemanticAnalyzer::analyzeInitIdentifier(ASTNode::ASTNodeType type, parser::IdentifierNode& parseNode){
    //this function is used for identifier declarations
    if(table.contains(parseNode.getText())){
        //double declaration
        parseNode.getReference().printContext("error: '"+ std::string(parseNode.getText()) +"' is already declared!");
        return nullptr;
    }
    table.insert(type,parseNode.getReference());
    if(type == ASTNode::Parameter){
        //treat parameters differently
        return std::make_unique<ASTParamIdentifierNode>(parseNode.getText());//0 as default value for the uninitialized variable
    }
    return std::make_unique<ASTIdentifierNode>(type,parseNode.getText());
}
std::unique_ptr<ASTIdentifierNode> SemanticAnalyzer::analyzeIdentifier(parser::IdentifierNode& parseNode){
    //this function is used, when analyzing statements
    auto entry = table.get(parseNode.getText());
    if(!entry){
        //undeclared identifier
        parseNode.getReference().printContext("error: '"+ std::string(parseNode.getText()) +"' is undeclared!");
        return nullptr;
    }
    ASTNode::ASTNodeType type = entry->identifierType;
    return std::make_unique<ASTIdentifierNode>(type,parseNode.getText());
}
} //namespace semantic