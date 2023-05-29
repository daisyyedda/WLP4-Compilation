#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

typedef std::vector<std::string> PARAMTYPES;
typedef std::vector<std::pair<std::string, std::string>> PROCEDURETABLE;
typedef std::vector<std::pair<std::pair<std::string, PARAMTYPES>, PROCEDURETABLE>> SYMBOLTABLE;

struct Tree {
  Tree *parent;
  int currIdx = 1;
  std::string rule;
  std::string type = "";
  std::vector<std::string> tokens;
  std::vector<Tree *> children;

  Tree() {}
  Tree(Tree *parent): parent{parent} {}
  ~Tree() { for (auto &child : children) { delete child; } }

  void addToken(std::string token) { 
    tokens.emplace_back(token); 
  }
  void addChild(Tree *t) { children.emplace_back(t); }

  void print() {
    for (int i = 0; i < int(tokens.size()); i++) {
      if (i) { std::cout << " "; }
      std::cout << tokens[i];
    }
    if (type != "") std::cout << " : " << type;
    std::cout << std::endl;
    for (auto &child : children) { child->print(); }
  }
};

void buildParseTree(std::istream &in, Tree *&root, Tree *&curr) {
  std::string token;
  Tree *tmp = nullptr;
  while (getline(in, token)) {
    std::istringstream iss{token};
    iss >> token;
    while (curr != nullptr && curr->parent != nullptr &&
    (curr->currIdx == int(curr->tokens.size()) ||
    curr->tokens[curr->currIdx] != token)) {
      curr = curr->parent;
    }
    tmp = new Tree{curr};
    if (curr == nullptr) {
      root = curr = tmp;
    } else {
      curr->addChild(tmp);
      curr->currIdx++;
      curr = tmp;
    }
    curr->rule = token;
    curr->addToken(token);
    while (iss >> token) { 
      curr->addToken(token); 
    }
  }
}

bool duplicateProcedureName(SYMBOLTABLE &symbolTable, std::string name) {
  for (auto &procedure : symbolTable) {
    if (procedure.first.first == name) { return true; }
  }
  return false;
}

bool duplicateVariableName(PROCEDURETABLE &procedureTable, std::string name) {
  for (auto &variable : procedureTable) {
    if (variable.first == name) { return true; }
  }
  return false;
}

std::string addVariableToProcedureTable(PROCEDURETABLE &procedureTable, std::string varName, std::string varType) {
  if (duplicateVariableName(procedureTable, varName)) {
    std::cerr << "ERROR: duplicate variable name " << varName << "." << std::endl;
  }
  procedureTable.emplace_back(std::make_pair(varName, varType));
  return varType;
}

void buildProcedureTable(Tree *root, PROCEDURETABLE &procedureTable) {
  // paramslist -> dcl || params -> paramslist
  if (root->children.size() == 1) { 
    buildProcedureTable(root->children[0], procedureTable);
  // dcl -> type ID
  } else if (root->children.size() == 2) {
    std::string varType = root->children[0]->children.size() == 1 ? "int" : "int*";
    std::string varName = root->children[1]->tokens[1];
    addVariableToProcedureTable(procedureTable, varName, varType);
  // paramlist -> paramlist COMMA dcl
  } else if (root->children.size() == 3) {
    buildProcedureTable(root->children[0], procedureTable);
    buildProcedureTable(root->children[2], procedureTable);
  // dcls -> dcls dcl BECOMES NUM SEMI
  // dcls -> dcls dcl BECOMES NULL SEMI
  } else if (root->children.size() == 5) {
    buildProcedureTable(root->children[0], procedureTable);
    std::string varType = root->children[1]->children[0]->children.size() == 1 ? "int" : "int*";
    std::string varName = root->children[1]->children[1]->tokens[1]; 
    addVariableToProcedureTable(procedureTable, varName, varType);
    if (root->children[3]->rule == "NUM" && varType != "int") {
      std::cerr << "ERROR: variable " << varName << " is not of type int." << std::endl;
    } else if (root->children[3]->rule == "NULL" && varType != "int*") {
      std::cerr << "ERROR: variable " << varName << " is not of type int*." << std::endl;
    }  
  // procedure -> ... || main -> ...
  } else if (root->children.size()> 5) {
    if (root->children[3]->rule == "dcl") {
      buildProcedureTable(root->children[3], procedureTable);
      buildProcedureTable(root->children[5], procedureTable);
      buildProcedureTable(root->children[8], procedureTable);
    } else {
      buildProcedureTable(root->children[3], procedureTable);
      buildProcedureTable(root->children[6], procedureTable);
    }
  }
}

void buildParamTypes(Tree *root, PARAMTYPES &paramType, PROCEDURETABLE &procedureTable) {
  // paramslist -> dcl || params -> paramslist
  if (root->children.size() == 1) {
    buildParamTypes(root->children[0], paramType, procedureTable);
  // dcl -> type ID
  } else if (root->children.size() == 2) {
    buildParamTypes(root->children[1], paramType, procedureTable);
    paramType.emplace_back((root->children[0]->children.size() == 1) ? "int" : "int*");
  // paramlist -> paramlist COMMA dcl
  } else if (root->children.size() == 3) {
    buildParamTypes(root->children[0], paramType, procedureTable);
    buildParamTypes(root->children[2], paramType, procedureTable);
  // procedure -> ... || main -> ...
  } else if (root->children.size() > 3){
    buildParamTypes(root->children[3], paramType, procedureTable);
    if (root->children[3]->rule == "dcl") {
      buildParamTypes(root->children[5], paramType, procedureTable);
    }
  }
}

bool checkValueExists(SYMBOLTABLE &symbolTable, std::string procName, std::string varName, std::string returnType) {
  for (auto &procedure : symbolTable) {
    if (procedure.first.first == procName) {
      for (auto &variable : procedure.second) {
        if (variable.first == varName && variable.second == returnType) { 
          return true;
        } else {
          std::cerr << "ERROR: variable is not of the desired type." << std::endl;
          return false;
        }
      }
    }
  }
  std::cerr << "ERROR: variable is uninitialized." << std::endl;
  return false;
}

bool checkValueType(Tree* root, SYMBOLTABLE &symbolTable, std::string procName, std::string returnType) {
  // expr -> term || term -> factor || factor -> ID/NUM/NULL || arglist -> expr || lvalue -> ID
  if (root->children.size() == 1) {
    std::string varRule = root->children[0]->rule;
    if (varRule == "term" || varRule == "factor") {
      return checkValueType(root->children[0], symbolTable, procName, returnType);
    } else if (varRule == "ID") {
      return checkValueExists(symbolTable, procName, root->children[0]->tokens[1], returnType);
    } else if (varRule == "NUM" || varRule == "NULL") {
      return varRule == "NUM" ? returnType == "int" : returnType == "int*";
    }
  // factor -> AMP lvalue || factor -> STAR factor || lvalue -> STAR factor
  } else if (root->children.size() == 2) {
    std::string varRule = root->children[0]->rule;
    if (varRule == "STAR" || varRule == "AMP") {
      return checkValueType(root->children[1], symbolTable, procName, returnType == "int" ? "int*" : "int");
    } 
  // expr -> expr PLUS/MINUS term || term -> term STAR/SLASH/PCT factor || factor -> LPAREN expr RPAREN || factor -> ID LPAREN RPAREN
  // arglist -> expr COMMA arglist || lvalue -> LPAREN lvalue RPAREN
  } else if (root->children.size() == 3) {
    
  // factor -> ID LPAREN arglist RPAREN
  } else if (root->children.size() == 4) {

  // factor -> NEW INT LBRACK expr RBRACK
  } else if (root->children.size() == 5) {

  } 
  
}

void buildSymbolTable(Tree *root, SYMBOLTABLE &symbolTable) {
  // start -> BOF procedures EOF
  if (root->children.size() == 3) {
    buildSymbolTable(root->children[1], symbolTable);
  // procedures -> procedure procedures || procedures -> main
  } else if (root->children.size() >= 1) {
    // procedures -> procedure procedures
    if (root->children.size() == 2) {
      buildSymbolTable(root->children[1], symbolTable);
    }
    Tree* procNode = root->children[0];
    std::string procName = procNode->children[1]->tokens[1];
    if (duplicateProcedureName(symbolTable, procName)) {
      std::cerr << "ERROR: duplicate procedure name." << std::endl;
    } else {
      PARAMTYPES paramType;
      PROCEDURETABLE procedureTable;
      buildParamTypes(procNode, paramType, procedureTable);
      if (paramType.size() >= 2 && procName == "wain") {
        if (paramType[1] != "int") {
          std::cerr << "ERROR: second parameter of wain must be of type int." << std::endl;
        }
      }
      buildProcedureTable(procNode, procedureTable);
      symbolTable.emplace_back(std::make_pair(procName, paramType), procedureTable);
      Tree* exprNode;
      if (procName == "wain") {
        exprNode = root->children[0]->children[11];
      } else {
        exprNode = root->children[0]->children[9];
      }
      if (!checkValueType(exprNode, symbolTable, procName, "int")) {
        std::cerr << "ERROR: return type must be of type int." << std::endl;
      }  
    }
  }
}

void annotateParent(Tree *root, std::string varType) {
  while (root->parent->rule != "procedure" && root->parent->rule != "main") {
    root->parent->type = varType;
    root = root->parent;
  }
}

void typeAnnotate(Tree *root, SYMBOLTABLE &symbolTable) {
  // std::cout << "children size: " << root->children.size() << std::endl;
  // unhandled: factor -> ID
  if (root->children.size() == 0) {
    return;
  } else if (root->children.size() == 1) {
    // procedures -> main
    if (root->children[0]->rule == "main") {
      typeAnnotate(root->children[0], symbolTable);
    // expr -> term || term -> factor
    } else if (root->rule == "expr" || root->rule == "term") {
      // std::cout << "root type: " << root->type << std::endl;
      typeAnnotate(root->children[0], symbolTable);
    // factor -> NUM || factor -> NULL
    } else if (root->children[0]->rule == "NUM" || root->children[0]->rule == "NULL") {
      std::string varType = root->children[0]->rule == "NUM" ? "int" : "int*";
      // std::cout << "varType: " << varType << std::endl;
      root->children[0]->type = varType;
      annotateParent(root->children[0], varType);
    } 
  } else if (root->children.size() == 2) {
    // std::cout << "root type when children size is 2" << root->type << std::endl;
    // procedures -> procedure procedures
    if (root->children[0]->rule == "procedure") {
      typeAnnotate(root->children[0], symbolTable); 
      typeAnnotate(root->children[1], symbolTable); 
    } 
    // dcl -> type ID 
    if (root->rule == "dcl") {
      std::string varType = root->children[0]->children.size() == 1 ? "int" : "int*";
      // std::cout << "varType: " << varType << std::endl;
      root->children[1]->type = varType;
    }
  } else if (root->children.size() == 3) {
    // start -> BOF procedures EOF || factor -> LPAREN expr RPAREN 
    if (root->rule == "start" || (root->children[0]->rule == "LPAREN" && root->rule == "factor")) {
      typeAnnotate(root->children[1], symbolTable);
    } 
  } else if (root->children.size() > 5) {
    // main -> ...
    if (root->rule == "main") {
      // std::cout << "main detected!" << std::endl;
      typeAnnotate(root->children[3], symbolTable); // dcl
      typeAnnotate(root->children[5], symbolTable); // dcl
      typeAnnotate(root->children[8], symbolTable); // dcls
      typeAnnotate(root->children[9], symbolTable); // statements
      typeAnnotate(root->children[11], symbolTable); // expr
    // procedure -> ...
    } else {
      typeAnnotate(root->children[3], symbolTable); // params
      typeAnnotate(root->children[6], symbolTable); // dcls
      typeAnnotate(root->children[7], symbolTable); // statements
      typeAnnotate(root->children[9], symbolTable); // expr
    }
  }
}

int main(void) {
  Tree *root = nullptr;
  Tree *curr = root;
  SYMBOLTABLE symbolTable;

  buildParseTree(std::cin, root, curr);
  buildSymbolTable(root, symbolTable);
  typeAnnotate(root, symbolTable);
  // typeCheck(root, symbolTable);
  root->print();
  delete root;
}
