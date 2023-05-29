#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

class ExceptionFailure;
typedef ExceptionFailure ExFail;
typedef std::vector<std::string> PARAMTYPES;
typedef std::vector<std::pair<std::string, std::string>> PROCEDURETABLE;
typedef std::vector<std::pair<std::pair<std::string, PARAMTYPES>, PROCEDURETABLE>> SYMBOLTABLE;

class ExceptionFailure {
  std::string message;
public:
  ExceptionFailure(const std::string &message): message{message} {}
  const std::string &what() { return message; }
};

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
    throw ExFail{"ERROR: variable " + varName + " is already declared."};
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
  // paramlist -> dcl COMMA paramlist
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
      throw ExFail{"ERROR: variable " + varName + " is not of type int."};
    } else if (root->children[3]->rule == "NULL" && varType != "int*") {
      throw ExFail{"ERROR: variable " + varName + " is not of type int*."};
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

std::string returnValueType(SYMBOLTABLE &symbolTable, std::string procName, std::string varName) {
  for (auto &procedure : symbolTable) {
    if (procedure.first.first == procName) {
      for (auto &variable : procedure.second) {
        if (variable.first == varName) { 
          return variable.second;
        }
      }
    }
  }
  throw ExFail{"ERROR: variable " + varName + " is not declared."};
}

std::string exprArithmetic(std::string leftType, std::string rightType, std::string op, Tree *root) {
  if (op == "PLUS") {
    if (leftType == "int*" && rightType == "int*") {
      throw ExFail{"ERROR: cannot add two int*."};
    } else {
      return (leftType == "int*" || rightType == "int*") ? "int*" : "int";
    }  
  } else {
    if (leftType == rightType) {
      return "int";
    } else if (leftType == "int*" && rightType == "int") {
      return "int*";
    } else if (leftType == "int" && rightType == "int*") {
      throw ExFail{"ERROR: cannot subtract an int* from an int."};
    }
  }
  return "";
}

std::string termArithmetic(std::string leftType, std::string rightType, std::string op) {
  if (!(leftType == "int" && rightType == "int")) {
    throw ExFail{"ERROR: cannot multiply or divide non-integers."};
  } else {
    return "int";
  }
}

bool isProcInScope(std::string newProc, std::string currProc, SYMBOLTABLE &symbolTable) {
  int newProcIdx = -1;
  int currProcIdx = -1;
  int count = 0;
  for (auto &procedure : symbolTable) {
    if (procedure.first.first == newProc) {
      newProcIdx = count;
    } else if (procedure.first.first == currProc) {
      currProcIdx = count;
    }
    count++;
  }
  if (newProcIdx == -1) {
    throw ExFail{"ERROR: procedure " + newProc + " is not declared."};
    return false;
  } else if (newProcIdx > currProcIdx && !(currProcIdx == -1 && newProcIdx != -1)) {
    throw ExFail{"ERROR: procedure " + newProc + " is not in scope."};
    return false;
  } else {
    return true;
  }
}

std::string findValueType(Tree* root, SYMBOLTABLE &symbolTable, std::string procName);

PARAMTYPES getArgTypes(Tree *root, PARAMTYPES &argTypes, SYMBOLTABLE &symbolTable, std::string procName) {
  // arglist -> expr 
  if (root->children.size() == 1) {
    argTypes.emplace_back(findValueType(root->children[0], symbolTable, procName));
  // arglist -> expr COMMA arglist
  } else if (root->children.size() == 3) {
    argTypes.emplace_back(findValueType(root->children[0], symbolTable, procName));
    getArgTypes(root->children[2], argTypes, symbolTable, procName);
  }
  return argTypes;
}

bool areParamsEqual(PARAMTYPES &paramTypes, PARAMTYPES &argTypes) {
  if (paramTypes.size() != argTypes.size()) {
    throw ExFail{"ERROR: parameter sizes do not match."};
    return false;
  } else {
    for (int i = 0; i < paramTypes.size(); i++) {
      if (paramTypes[i] != argTypes[i]) {
        throw ExFail{"ERROR: parameter types do not match."};
        return false;
      }
    }
  }
  return true;
}

std::string findValueType(Tree* root, SYMBOLTABLE &symbolTable, std::string procName) {
  if (root->children.size() == 1) {
    std::string varRule = root->children[0]->rule;
    // expr -> term || term -> factor || arglist -> expr || params -> paramlist || paramlist -> dcl
    if (varRule == "term" || varRule == "factor" || varRule == "paramlist" || varRule == "dcl" || varRule == "expr") {
      return findValueType(root->children[0], symbolTable, procName);
    // factor -> ID || lvalue -> ID
    } else if (varRule == "ID") {
      return returnValueType(symbolTable, procName, root->children[0]->tokens[1]);
    // factor -> NUM/NULL
    } else if (varRule == "NUM" || varRule == "NULL") {
      return varRule == "NUM" ? "int" : "int*";
    }
  } else if (root->children.size() == 2) {
    std::string varRule = root->children[0]->rule;
    // factor -> AMP lvalue || factor -> STAR factor || lvalue -> STAR factor
    if (varRule == "STAR" || varRule == "AMP") {
      return findValueType(root->children[1], symbolTable, procName) == "int" ? "int*" : "int";
    // dcl -> type ID
    } else if (root->rule == "dcl") {
      return root->children[0]->children.size() == 1 ? "int" : "int*";
    }
  } else if (root->children.size() == 3) {
    // expr -> expr PLUS/MINUS term || term -> term STAR/SLASH/PCT factor 
    if (root->rule == "expr" || root->rule == "term" || root->rule == "arglist") {
      std::string leftType = findValueType(root->children[0], symbolTable, procName);
      std::string rightType = findValueType(root->children[2], symbolTable, procName);
      std::string op = root->children[1]->rule;
      return root->rule == "expr" ? exprArithmetic(leftType, rightType, op, root) : termArithmetic(leftType, rightType, op);
    } else if (root->rule == "factor") {
      // factor -> LPAREN expr RPAREN
      if (root->children[0]->rule == "LPAREN") {
        return findValueType(root->children[1], symbolTable, procName);
      // factor -> ID LPAREN RPAREN
      } else {
        std::string procNameUsed = root->children[0]->tokens[1];
        PARAMTYPES paramType;
        if (isProcInScope(procNameUsed, procName, symbolTable)) {
          for (auto &procedure : symbolTable) {
            if (procedure.first.first == procNameUsed) {
              paramType = procedure.first.second;
            }
          }
        } 
        if (paramType.size() != 0) {
          throw ExFail{"ERROR: parameter types do not match."};
        } else {
          return "int";
        }
      }
    // lvalue -> LPAREN lvalue RPAREN
    } else if (root->rule == "lvalue") {
      return findValueType(root->children[1], symbolTable, procName);
    // arglist -> expr COMMA arglist || paramlist -> dcl COMMA paramlist
    } else if (root->rule == "arglist" || root->rule == "paramlist") {
      findValueType(root->children[0], symbolTable, procName);
      findValueType(root->children[2], symbolTable, procName);
    }
  } else if (root->children.size() == 4) {
    // factor -> ID LPAREN arglist RPAREN
    if (root->rule == "factor") {
      Tree* argNode = root->children[2];
      std::string procNameUsed = root->children[0]->tokens[1];
      PARAMTYPES paramType2;
      paramType2 = getArgTypes(argNode, paramType2, symbolTable, procName);
      PARAMTYPES paramType;
      if (isProcInScope(procNameUsed, procName, symbolTable)) {
        for (auto &procedure : symbolTable) {
          if (procedure.first.first == procNameUsed) {
            paramType = procedure.first.second;
          }     
        }
      }
      if (areParamsEqual(paramType, paramType2)) {
        return "int";
      }
    }
  // factor -> NEW INT LBRACK expr RBRACK
  } else if (root->children.size() == 5) {
    if (root->rule == "factor") {
      std::string exprType = findValueType(root->children[3], symbolTable, procName);
      if (exprType == "int*") {
        throw ExFail{"ERROR: cannot allocate an array of int*."};
      }
      return "int*";
    }
  } 
  return "";
}

void buildSymbolTable(Tree *root, SYMBOLTABLE &symbolTable) {
  // start -> BOF procedures EOF
  if (root->children.size() == 3) {
    buildSymbolTable(root->children[1], symbolTable);
  // procedures -> procedure procedures || procedures -> main
  } else if (root->children.size() >= 1) {
    // procedures -> procedure procedures
    Tree* procNode = root->children[0];
    std::string procName = procNode->children[1]->tokens[1];
    if (duplicateProcedureName(symbolTable, procName)) {
      throw ExFail{"ERROR: procedure " + procName + " is already declared."};
    } else {
      PARAMTYPES paramType;
      PROCEDURETABLE procedureTable;
      buildParamTypes(procNode, paramType, procedureTable);
      if (paramType.size() >= 2 && procName == "wain") {
        if (paramType[1] != "int") {
          throw ExFail{"ERROR: second parameter of wain must be of type int."};
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
      std::string returnType = findValueType(exprNode, symbolTable, procName);
      if (returnType != "int") {
        throw ExFail{"ERROR: return type of " + procName + " must be int."};
      }  
    }
    if (root->children.size() == 2) {
      buildSymbolTable(root->children[1], symbolTable);
    }
  }
}

// need to modify this function to handle all of the corner cases
void annotateParent(Tree *root, std::string varType) {
  while ((root->parent->children.size() == 1 || 
        (root->parent->children.size() == 3 && root->rule == "term")) &&
        root->parent->rule != "arglist") { 
    root->parent->type = varType;
    root = root->parent;
  }
}

void typeAnnotate(Tree *root, SYMBOLTABLE &symbolTable, std::string procName) {
  if (root->children.size() == 0) {
    return;
  } else if (root->children.size() == 1) {
    // procedures -> main
    if (root->children[0]->rule == "main") {
      typeAnnotate(root->children[0], symbolTable, procName);
    // expr -> term || term -> factor || arglist -> expr || params -> paramlist || paramlist -> dcl
    } else if (root->rule == "expr" || root->rule == "term" || root->rule == "arglist" || root->rule == "params" || root->rule == "paramlist") {
      typeAnnotate(root->children[0], symbolTable, procName);
    // factor -> NUM || factor -> NULL
    } else if (root->children[0]->rule == "NUM" || root->children[0]->rule == "NULL") {
      std::string varType = root->children[0]->rule == "NUM" ? "int" : "int*";
      root->children[0]->type = varType;
      annotateParent(root->children[0], varType);
    // factor -> ID || lvalue -> ID
    } else if (root->children[0]->rule == "ID") {
      std::string varType = returnValueType(symbolTable, procName, root->children[0]->tokens[1]);
      root->children[0]->type = varType;
      annotateParent(root->children[0], varType);
    }
  } else if (root->children.size() == 2) {
    // procedures -> procedure procedures || statements -> statements statement
    if (root->children[0]->rule == "procedure" || root->children[0]->rule == "statements") {
      std::string procNameNew;
      if (root->children[0]->rule == "procedure") {
        procNameNew = root->children[0]->children[1]->tokens[1];
        typeAnnotate(root->children[0], symbolTable, procNameNew);
      } else {
        typeAnnotate(root->children[0], symbolTable, procName);
      }
      typeAnnotate(root->children[1], symbolTable, procName); 
    } 
    // dcl -> type ID 
    if (root->rule == "dcl") {
      std::string varType = root->children[0]->children.size() == 1 ? "int" : "int*";
      root->children[1]->type = varType;
    }
    // factor -> STAR factor || factor -> AMP lvalue || lvalue -> STAR factor
    if (root->rule == "factor" || root->rule == "lvalue") {
      std::string varType = findValueType(root->children[1], symbolTable, procName);
      root->type = (varType == "int*") ? "int" : "int*";
      annotateParent(root, root->type);
      typeAnnotate(root->children[1], symbolTable, procName);
    }
  } else if (root->children.size() == 3) {
    // start -> BOF procedures EOF
    if (root->rule == "start") {
      typeAnnotate(root->children[1], symbolTable, procName);
    // expr -> expr PLUS/MINUS term || term -> term STAR/SLASH/PCT factor 
    } else if (root->rule == "expr" || root->rule == "term") {
      std::string op = root->children[1]->rule;
      std::string leftType = findValueType(root->children[0], symbolTable, procName);
      std::string rightType = findValueType(root->children[2], symbolTable, procName);
      typeAnnotate(root->children[0], symbolTable, procName);
      typeAnnotate(root->children[2], symbolTable, procName);
      root->type = (root->rule == "expr") ? exprArithmetic(leftType, rightType, op, root) : termArithmetic(leftType, rightType, op);
    } else if (root->rule == "factor" || root->rule == "lvalue") {
      // factor -> ID LPAREN RPAREN
      if (root->children[0]->rule == "ID") {
        root->type = "int";
        typeAnnotate(root->children[0], symbolTable, procName);
      // factor -> LPAREN expr RPAREN || lvalue -> LPAREN lvalue RPAREN
      } else {
        root->type = findValueType(root->children[1], symbolTable, procName);
        typeAnnotate(root->children[1], symbolTable, procName);
      }
      annotateParent(root, root->type);
    } else if (root->rule == "arglist") {
      // arglist -> expr COMMA arglist
      typeAnnotate(root->children[0], symbolTable, procName);
      typeAnnotate(root->children[2], symbolTable, procName);
    } else if (root->rule == "paramlist") {
      // paramlist -> dcl COMMA paramlist
      typeAnnotate(root->children[0], symbolTable, procName);
      typeAnnotate(root->children[2], symbolTable, procName);
    } else if (root->rule == "test") {
      // test -> expr EQ/NE/LE/GE/LT/GT expr
      std::string leftType = findValueType(root->children[0], symbolTable, procName);
      std::string rightType = findValueType(root->children[2], symbolTable, procName);
      if (leftType != rightType) {
        throw ExFail{"ERROR: Type mismatch in test."};
      }
      typeAnnotate(root->children[0], symbolTable, procName);
      typeAnnotate(root->children[2], symbolTable, procName);
    }
  } else if (root->children.size() == 4) {
    // factor -> ID LPAREN arglist RPAREN
    if (root->rule == "factor") {
      root->type = "int";
      typeAnnotate(root->children[0], symbolTable, procName);
      typeAnnotate(root->children[2], symbolTable, procName);
      annotateParent(root, root->type);
    // statements -> lvalue BECOMES expr SEMI
    } else if (root->rule == "statement") {
      std::string leftType = findValueType(root->children[0], symbolTable, procName);
      std::string rightType = findValueType(root->children[2], symbolTable, procName);
      typeAnnotate(root->children[0], symbolTable, procName);
      typeAnnotate(root->children[2], symbolTable, procName);
      if (leftType != rightType) {
        throw ExFail{"ERROR: cannot assign " + rightType + " to " + leftType + "."};
      }
    }
  } else if (root->children.size() == 5) {
    // dcls -> dcls dcl BECOMES NUM/NULL SEMI
    if (root->rule == "dcls") {
      typeAnnotate(root->children[0], symbolTable, procName);
      std::string varType = root->children[3]->rule == "NUM" ? "int" : "int*";
      root->children[1]->children[1]->type = varType;
      root->children[3]->type = varType;
    // factor -> NEW INT LBRACK expr RBRACK
    } else if (root->rule == "factor") {
      std::string exprType = findValueType(root->children[3], symbolTable, procName);
      if (exprType == "int*") {
        throw ExFail{"ERROR: cannot allocate an array of pointers."};
      }
      root->type = "int*";
      annotateParent(root, root->type);
      typeAnnotate(root->children[3], symbolTable, procName);
     
    } else if (root->rule == "statement") {
      // statement -> PRINTLN LPAREN expr RPAREN SEMI
      if (root->children[0]->rule == "PRINTLN") {
        std::string exprType = findValueType(root->children[2], symbolTable, procName);
        if (exprType == "int*") {
          throw ExFail{"ERROR: cannot print a pointer."};
        }
        typeAnnotate(root->children[2], symbolTable, procName);
      // statement -> DELETE LBRACK RBRACK expr SEMI
      } else {
        std::string exprType = findValueType(root->children[3], symbolTable, procName);
        if (exprType != "int*") {
          throw ExFail{"ERROR: cannot delete a non-pointer."};
        }
        typeAnnotate(root->children[3], symbolTable, procName);
      }
    }
  } else if (root->children.size() > 5) {
    // main -> ...
    if (root->rule == "main") {
      procName = "wain";
      typeAnnotate(root->children[3], symbolTable, procName); // dcl
      typeAnnotate(root->children[5], symbolTable, procName); // dcl
      typeAnnotate(root->children[8], symbolTable, procName); // dcls
      typeAnnotate(root->children[9], symbolTable, procName); // statements
      typeAnnotate(root->children[11], symbolTable, procName); // expr
    // procedure -> ...
    } else if (root->rule == "procedure"){
      procName = root->children[1]->tokens[1];
      typeAnnotate(root->children[3], symbolTable, procName); // params
      typeAnnotate(root->children[6], symbolTable, procName); // dcls
      typeAnnotate(root->children[7], symbolTable, procName); // statements
      typeAnnotate(root->children[9], symbolTable, procName); // expr
    } else if (root->rule == "statement") {
      // statement -> WHILE LPAREN test RPAREN LBRACE statements RBRACE
      if (root->children.size() == 7) {
        typeAnnotate(root->children[2], symbolTable, procName); // test
        typeAnnotate(root->children[5], symbolTable, procName); // statements
      // statement -> IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE
      } else {
        typeAnnotate(root->children[2], symbolTable, procName); // test
        typeAnnotate(root->children[5], symbolTable, procName); // statements
        typeAnnotate(root->children[9], symbolTable, procName); // statements
      }
    }
  }
}

int main() {
  Tree *root = nullptr;
  Tree *curr = root;
  SYMBOLTABLE symbolTable;
  std::string procName;
  try {
    buildParseTree(std::cin, root, curr);
    buildSymbolTable(root, symbolTable);
    typeAnnotate(root, symbolTable, procName);
    root->print();
  } catch (ExFail &e) {
    std::cerr << e.what() << std::endl;
  }
  delete root;
}

