#include "SemanticAnalyzer.h"
namespace semantic {
using NodeType = parser::Node::Types;

std::unique_ptr<ASTNode> SemanticAnalyzer::getChild(auto func, auto child){
    parser::NonTerminalNode* nonTerminalNode = static_cast<parser::NonTerminalNode*>(child);
    return std::move((this->*func)(*nonTerminalNode));
}
std::unique_ptr<ASTFunctionNode> SemanticAnalyzer::analyzeFunction(const std::vector<long>& arg,parser::NonTerminalNode& parseNode){
    if(parseNode.getType() == NodeType::FunctionDefinition){
        std::unique_ptr<ASTFunctionNode> node = std::make_unique<ASTFunctionNode>();
        bool hasParams = false;
        /*
         * lambda for calling analyze child function, adding the child to the newly created ASTNode and return false if there are compilation errors
         */
        auto func = [&node, this](auto func2, auto child){
            auto astVar = this->getChild(func2, child);
            if(!astVar) return false;
            node->children.push_back(std::move(astVar));
            return true;
        };
        for(auto& child: parseNode.getChildren()){
            if(child->getType() == NodeType::ParameterDeclaration){
                parser::NonTerminalNode* nonTerminalNode = static_cast<parser::NonTerminalNode*>(child.get());
                auto astParam = analyzeParameterDeclaration(arg,*nonTerminalNode);
                if(!astParam) return nullptr;
                node->children.push_back(std::move(astParam));
                hasParams = true;
            } else if((child->getType() == NodeType::VariableDeclaration && !func(&SemanticAnalyzer::analyzeVariableDeclaration, child.get()))
                       || (child->getType() == NodeType::ConstantDeclaration && !func(&SemanticAnalyzer::analyzeConstantDeclaration, child.get()))
                       || (child->getType() == NodeType::CompoundStatement && !func(&SemanticAnalyzer::analyzeCompoundStatement, child.get()))){
                return nullptr;
            }
        }
        if(!hasParams && !arg.empty()) return nullptr; //if we don't have parameters, but there are still arguments
        return node;
    }
    return nullptr;
}
bool SemanticAnalyzer::refactorDeclaration(auto& astNode, ASTNode::ASTNodeType astChildType, parser::NonTerminalNode& parseNode, size_t expectedArgSize){
    auto children = parseNode.getChildren();
    auto declList = std::move(children[1]);
    if(declList->getType() == NodeType::DeclaratorList) {
        parser::NonTerminalNode* decl = static_cast<parser::NonTerminalNode*>(declList.get());
        auto declChildren = decl->getChildren();
        if (parseNode.getType() == parser::Node::ParameterDeclaration && declChildren.size() / 2 + 1 != expectedArgSize) { //we should have arg.size()-1 commas as children => decl->getChildren().size()/2+1 when they are not included
            //if parameters size don't match
            parseNode.getReference().printContext("error: parameter size doesn't match with argument size!");
            return false;
        }
        for (auto& child : declChildren) {
            if (child->getType() == NodeType::Identifier) {
                parser::IdentifierNode* identifierNode = static_cast<parser::IdentifierNode*>(child.get());
                auto astIdentifier = analyzeInitIdentifier(astChildType, *identifierNode);
                if (!astIdentifier) return false;
                astNode.children.push_back(std::move(astIdentifier));
            }
        }
        return true;
    }
    return false;
}
std::unique_ptr<ASTParamDeclaratorListNode> SemanticAnalyzer::analyzeParameterDeclaration(const std::vector<long>& arg,parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTParamDeclaratorListNode> node = std::make_unique<ASTParamDeclaratorListNode>();
    if(!refactorDeclaration(*node,ASTNode::Parameter,parseNode,arg.size())) return nullptr;
    return node;
}
std::unique_ptr<ASTVarDeclaratorListNode> SemanticAnalyzer::analyzeVariableDeclaration(parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTVarDeclaratorListNode> node = std::make_unique<ASTVarDeclaratorListNode>();
    if(!refactorDeclaration(*node,ASTNode::Variable,parseNode, 0)) return nullptr;
    return node;
}
std::unique_ptr<ASTInitDeclaratorListNode> SemanticAnalyzer::analyzeConstantDeclaration(parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTInitDeclaratorListNode> node = std::make_unique<ASTInitDeclaratorListNode>();
    auto children = parseNode.getChildren();
    auto declList = std::move(children[1]);
    if(declList->getType() == NodeType::InitDeclaratorList){
        parser::NonTerminalNode* decl = static_cast<parser::NonTerminalNode*>(declList.get());
        for(auto& child: decl->getChildren()){
            if(child->getType() == NodeType::InitDeclarator){
                auto astIdentifier = getChild(&SemanticAnalyzer::analyzeInitDeclarator,child.get());
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
        auto children = parseNode.getChildren();
        parser::IdentifierNode* identifierNode = static_cast<parser::IdentifierNode*>(children[0].get());
        auto astIdentifier = analyzeInitIdentifier(ASTNode::ASTNodeType::Constant,*identifierNode);
        if(!astIdentifier) return nullptr;

        parser::LiteralNode* literal = static_cast<parser::LiteralNode*>(children[2].get());
        auto astLiteral = std::make_unique<ASTLiteralNode>(literal->getValue());
        std::unique_ptr<ASTInitDeclaratorNode> node = std::make_unique<ASTInitDeclaratorNode>(std::move(astIdentifier),std::move(astLiteral));
        return node;
    }
    return nullptr;
}
std::unique_ptr<ASTCompoundStatement> SemanticAnalyzer::analyzeCompoundStatement(parser::NonTerminalNode& parseNode){
    std::unique_ptr<ASTCompoundStatement> node = std::make_unique<ASTCompoundStatement>();
    auto children = parseNode.getChildren();
    auto declList = std::move(children[1]);
    bool hasReturnStatement = false;
    if(declList->getType() == NodeType::StatementList){
        parser::NonTerminalNode* decl = static_cast<parser::NonTerminalNode*>(declList.get());
        for(auto& child: decl->getChildren()){
            if(child->getType() == NodeType::Statement){
                auto astStatement = getChild(&SemanticAnalyzer::analyzeStatement,child.get());
                if(!astStatement) return nullptr;
                else if(astStatement->getType() == ASTNode::ReturnStatement) hasReturnStatement = true; //check for return statement
                node->children.push_back(std::move(astStatement));
            }
        }
        if(!hasReturnStatement) {
            parseNode.getReference().printContext("error: missing return statement");
            return nullptr;
        }
        return node;
    }
    return nullptr;
}
std::unique_ptr<ASTStatementNode> SemanticAnalyzer::analyzeStatement(parser::NonTerminalNode& parseNode){
    auto returnASTStatement = [this](ASTNode::ASTNodeType returnType, auto child) -> std::unique_ptr<ASTStatementNode>{
        parser::NonTerminalNode* childNode = static_cast<parser::NonTerminalNode*>(child);
        auto childAST = getChild(&SemanticAnalyzer::analyzeExpression,childNode);
        if(!childAST) return nullptr;
        return std::make_unique<ASTStatementNode>(returnType, std::move(childAST));
    };
    auto children = parseNode.getChildren();
    if(children.size() == 2){   //case return expression
        return returnASTStatement(ASTNode::ASTNodeType::ReturnStatement,children[1].get());
    } else {    //case assignment
        return returnASTStatement(ASTNode::ASTNodeType::AssignStatement,children[0].get());
    }
}
std::unique_ptr<ASTNode> SemanticAnalyzer::analyzeExpression(parser::NonTerminalNode& parseNode){
    NodeType parseType = parseNode.getType();
    auto children = parseNode.getChildren();
    if(parseType == NodeType::AssignmentExpression){
        parser::IdentifierNode* childIdentifier = static_cast<parser::IdentifierNode*>(children[0].get());
        auto childName = childIdentifier->getReference().getText();
        if(table.contains(childName) && table.get(childName).identifierType == ASTNode::Constant){
            parseNode.getReference().printContext("error: cannot assign a value to a constant!");
            return nullptr;
        }
        auto identifier = analyzeIdentifier(*childIdentifier);
        auto expression = getChild(&SemanticAnalyzer::analyzeExpression, children[2].get());
        if(!identifier || !expression) return nullptr;
        return std::make_unique<ASTAssignmentExpression>(std::move(identifier),std::move(expression));
    } else if(parseType == NodeType::AdditiveExpression || parseType == NodeType::MultiplicativeExpression){
        if(children.size() == 1){    // if we only have one child we can omit this level
            return getChild(&SemanticAnalyzer::analyzeExpression, children[0].get());
        } else {
            auto operatorType = children[1]->getType();
            auto astFirstExpr = getChild(&SemanticAnalyzer::analyzeExpression, children[0].get());
            auto astSndExpr = getChild(&SemanticAnalyzer::analyzeExpression, children[2].get());
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
        if(children.size() == 2){
            //in case there are +/- signs before the expression
            NodeType opType = children[0]->getType();
            //get child expression
            auto child = getChild(&SemanticAnalyzer::analyzeExpression, children[1].get());
            if(!child) return nullptr;
            if(opType == NodeType::MinusOperator){
                return std::make_unique<ASTUnaryExpression>(ASTNode::ASTNodeType::UnaryMinus, std::move(child));
            } else {
                return std::make_unique<ASTUnaryExpression>(ASTNode::ASTNodeType::UnaryPlus, std::move(child));
            }
        }
        parser::NonTerminalNode* expr = static_cast<parser::NonTerminalNode*>(children[0].get());
        return analyzeExpression(*expr);
    } else if(parseType == NodeType::PrimaryExpression){
        if(children.size() > 1){
            //case (expr)
            return getChild(&SemanticAnalyzer::analyzeExpression, children[1].get());
        } else {
            if(children[0]->getType() == NodeType::Identifier){
                //case identifier
                parser::IdentifierNode* identifier = static_cast<parser::IdentifierNode*>(children[0].get());
                return analyzeIdentifier(*identifier);
            } else {
                //case literal
                parser::LiteralNode* literal = static_cast<parser::LiteralNode*>(children[0].get());
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
    return std::make_unique<ASTIdentifierNode>(type,parseNode.getText());
}
std::unique_ptr<ASTIdentifierNode> SemanticAnalyzer::analyzeIdentifier(parser::IdentifierNode& parseNode){
    //this function is used, when analyzing statements
    if(!table.contains(parseNode.getText())){
        //undeclared identifier
        parseNode.getReference().printContext("error: '"+ std::string(parseNode.getText()) +"' is undeclared!");
        return nullptr;
    }
    auto entry = table.get(parseNode.getText());
    ASTNode::ASTNodeType type = entry.identifierType;
    return std::make_unique<ASTIdentifierNode>(type,parseNode.getText());
}
} //namespace semantic