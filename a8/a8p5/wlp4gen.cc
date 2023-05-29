#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

typedef std::map<std::string, std::map<std::string, std::pair<std::string, int>>> TABLE;

struct Tree {
  Tree *parent;
  int currIdx = 1;
  std::string rule;
  std::string type = "";
  std::vector<std::string> tokens;
  std::vector<Tree *> children;
  Tree() {}
  Tree(Tree *parent) : parent{parent} {}
  ~Tree() { for (auto &child : children) { delete child; } }

  void addToken(std::string token) { tokens.emplace_back(token); }
  void addChild(Tree *t) { children.emplace_back(t); }
};

void pushReg(std::string regNum) {
  std::cout << "sw $" << regNum << ", -4($30)" << std::endl;
  std::cout << "sub $30, $30, $4" << std::endl;
}

void popReg(std::string regNum) {
  std::cout << "add $30, $30, $4" << std::endl;
  std::cout << "lw $" << regNum << ", -4($30)" << std::endl;
}

void buildParseTree(std::istream &in, Tree *&root, Tree *&curr, std::vector<std::string> &usedFunc) {
  std::string token;
  Tree *tmp = nullptr;
  bool recordFunc = false;
  while (getline(in, token)) {
    bool encounteredType = false;
    bool hasTrimmed = false;
    std::istringstream iss{token};
    iss >> token;
    while (curr != nullptr && curr->parent != nullptr &&
    (curr->currIdx == int(curr->tokens.size()) || curr->tokens[curr->currIdx] != token)) {
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
    curr->addToken(token);
    curr->rule = token + " ";
    while (iss >> token) {
      if (encounteredType) {
        curr->type = token;
        curr->rule.pop_back();
        hasTrimmed = true;
        break;
      } else if (token == ":") {
        encounteredType = true;
        continue;
      } else {
        curr->addToken(token);
        curr->rule.append(token + " ");
      }
    }
    if (!hasTrimmed) { 
      curr->rule.pop_back(); 
    }
    if (recordFunc && curr->tokens[0] == "ID") {
      usedFunc.emplace_back("F" + curr->tokens[1]);
      recordFunc = false;
    }
    if (curr->rule == "factor ID LPAREN RPAREN" || curr->rule == "factor ID LPAREN arglist RPAREN") {
      recordFunc = true;
    }
  }
}

void buildProcTable(Tree *tree, std::map<std::string, std::pair<std::string, int>> &procTable) {
  if (tree->tokens[0] == "dcl") {
    int offset = procTable.size() * -4;
    procTable[tree->children[1]->tokens[1]] = std::pair<std::string, int>(tree->children[1]->type, offset);
  } else {
    for (int i = 0; i < (int)tree->children.size(); i++) {
      buildProcTable(tree->children[i], procTable);
    }
  }
}

int paraNum(Tree *tree) {
  int num = 0;
  if (tree->rule == "paramlist dcl COMMA paramlist") {
    num = 1 + paraNum(tree->children[2]);
  } else if (tree->rule == "paramlist dcl") {
    num++;
  } else if (tree->rule == "params paramlist") {
    num = paraNum(tree->children[0]);
  }
  return num;
}

void buildSymbolTable(Tree *tree, TABLE &table, std::string label, int offset) {
  if (tree->tokens[0] == "procedure" || tree->tokens[0] == "main") {
    label = tree->children[1]->tokens[1];
    if (tree->tokens[0] != "main") {
      label = "F" + label;
    }
    buildProcTable(tree, table[label]);
    if (label != "wain") {
      int offset = 4 * paraNum(tree->children[3]);
      for (auto &p : table[label]) {
        p.second.second += offset;
      }
    }
  }
  for (Tree *t : tree->children) {
    buildSymbolTable(t, table, label, offset);
  }
}

bool isConstNum(Tree *root, int& constNum) {
  if (root->rule == "factor NUM") {
    constNum = stoi(root->children[0]->tokens[1]);
    return true;
  } else if (root->tokens[0] == "factor") {
    return false;
  } else if (root->rule == "term factor" || root->rule == "expr term") {
    return isConstNum(root->children[0], constNum);
  } else {
    int constNumLeft, constNumRight;
    bool LHS = isConstNum(root->children[0], constNumLeft);
    bool RHS = isConstNum(root->children[2], constNumRight);
    if (LHS && RHS) {
      std::string sign = root->children[1]->tokens[0];
      constNum = (sign == "PLUS") ? constNumLeft + constNumRight : 
      (sign == "MINUS")  ? constNumLeft - constNumRight :
      (sign == "STAR")  ? constNumLeft * constNumRight :
      (sign == "SLASH")  ? constNumLeft / constNumRight : constNumLeft % constNumRight;
      return true;
    } else {
      return false;
    }
  }
}

void codeGenerate(Tree *root, TABLE &table, std::string label, int &offset, int &time, std::vector<std::string> &usedFunc) {
  if (root->rule == "start BOF procedures EOF") {
    codeGenerate(root->children[1], table, label, offset, time, usedFunc);
  }
  if (root->rule == "procedures procedure procedures") {
    codeGenerate(root->children[1], table, label, offset, time, usedFunc);
    codeGenerate(root->children[0], table, label, offset, time, usedFunc);
  }
  if (root->rule == "procedures main") {
    codeGenerate(root->children[0], table, label, offset, time, usedFunc);
  }
  if (root->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
    label = root->children[1]->tokens[1];
    std::string procName = label;
    std::cout << procName << ": " << std::endl;
    std::cout << "sub $29, $30, $4" << std::endl;
    std::cout << "sw $1, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    std::cout << "sw $2, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4 " << std::endl;
    if (root->children[3]->type == "int" && root->children[5]->type == "int") {
      std::cout << "add $2, $0, $0" << std::endl;
    }
    pushReg("31");
    std::cout << "lis $5" << std::endl;
    std::cout << ".word init" << std::endl;
    std::cout << "jalr $5" << std::endl;
    popReg("31");
    int sizeSub = 4 * (table[label].size() - 2);
    std::cout << "lis $17" << std::endl;
    std::cout << ".word " << sizeSub << std::endl;
    std::cout << "sub $30, $30, $17" << std::endl;
    codeGenerate(root->children[8], table, label, offset, time, usedFunc);
    codeGenerate(root->children[9], table, label, offset, time, usedFunc);
    codeGenerate(root->children[11], table, label, offset, time, usedFunc);
    std::cout << "add $30, $30, $17" << std::endl;
    std::cout << "jr $31" << std::endl;
  }
  if (root->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
    label = "F" + root->children[1]->tokens[1];
    if (std::find(usedFunc.begin(), usedFunc.end(), label) != usedFunc.end()) {
      std::cout << label << ": " << std::endl;
      std::cout << "sub $29, $30, $4" << std::endl;
      codeGenerate(root->children[6], table, label, offset, time, usedFunc);
      codeGenerate(root->children[7], table, label, offset, time, usedFunc);
      codeGenerate(root->children[9], table, label, offset, time, usedFunc);
      std::cout << "add $30, $29, $4" << std::endl;
      std::cout << "jr $31" << std::endl; 
    }    
  }
  if (root->rule == "params paramlist" || root->rule == "paramlist dcl" || root->rule == "arglist expr") {
    codeGenerate(root->children[0], table, label, offset, time, usedFunc);
  }
  if (root->rule == "paramlist dcl COMMA paramlist" || root->rule == "arglist expr COMMA arglist") {
    codeGenerate(root->children[0], table, label, offset, time, usedFunc);
    codeGenerate(root->children[2], table, label, offset, time, usedFunc);
  }
  if (root->rule == "dcls .EMPTY" || root->rule == "statement .EMPTY" || root->rule == "params .EMPTY") {
    return;
  }
  if (root->rule == "expr term" || root->rule == "term factor") {
    codeGenerate(root->children[0], table, label, offset, time, usedFunc);
  }
  if (root->rule == "factor ID") { 
    std::string idName = root->children[0]->tokens[1];
    std::cout << "lw $3, " << table[label][idName].second << "($29)" << std::endl;
  }
  if (root->rule == "factor NUM") {
    std::string num = root->children[0]->tokens[1];
    std::cout << "lis $3" << std::endl;
    std::cout << ".word " << num << std::endl;
  }
  if (root->rule == "factor NULL") {
    std::cout << "add $3, $0, $11" << std::endl;
  }
  if (root->rule == "factor AMP lvalue") {
    Tree *tmp = root->children[1];
    while (tmp->rule == "lvalue LPAREN lvalue RPAREN") {
      tmp = tmp->children[1];
    }
    if (tmp->rule == "lvalue ID") {
      std::cout << "lis $3" << std::endl;
      std::cout << ".word " << table[label][tmp->children[0]->tokens[1]].second << std::endl;
      std::cout << "add $3, $3, $29" << std::endl;
    } else { 
      codeGenerate(tmp->children[1], table, label, offset, time, usedFunc);
    }    
  }
  if (root->rule == "factor STAR factor") {
    codeGenerate(root->children[1], table, label, offset, time, usedFunc);
    std::cout << "lw $3, 0($3)" << std::endl;
  }
  if (root->rule == "factor LPAREN expr RPAREN") {
    codeGenerate(root->children[1], table, label, offset, time, usedFunc);
  }
  if (root->rule == "factor ID LPAREN RPAREN") {
    pushReg("29");
    pushReg("31");
    std::cout << "lis $5" << std::endl;
    std::cout << ".word " << "F" + root->children[0]->tokens[1] << std::endl;
    std::cout << "jalr $5" << std::endl;
    popReg("31");
    popReg("29");
  }
  if (root->rule == "factor ID LPAREN arglist RPAREN") {
    pushReg("29");
    pushReg("31");
    int time = 1;
    Tree *argList = root->children[2];
    while (argList->tokens.size() > 2) {
      codeGenerate(argList->children[0], table, label, offset, time, usedFunc);
      pushReg("3");
      time++;
      argList = argList->children[2];
    }
    codeGenerate(argList->children[0], table, label, offset, time, usedFunc);
    pushReg("3");        
    std::cout << "lis $5" << std::endl;
    std::cout << ".word " << "F" + root->children[0]->tokens[1] << std::endl;
    std::cout << "jalr $5" << std::endl;
    for (int i = 0; i < time + 1; i++) { popReg("31"); }
    popReg("29");
  }
  if (root->rule == "factor NEW INT LBRACK expr RBRACK") {
    codeGenerate(root->children[3], table, label, offset, time, usedFunc);
    std::cout << "add $1, $3, $0" << std::endl;
    pushReg("31");
    std::cout << "lis $5" << std::endl;
    std::cout << ".word new" << std::endl;
    std::cout << "jalr $5" << std::endl;
    popReg("31");
    std::cout << "bne $3, $0, 1" << std::endl;
    std::cout << "add $3, $11, $0" << std::endl;
  }
  if (root->rule == "expr expr PLUS term" || root->rule == "expr expr MINUS term") {
    int numLeft, numRight;
    bool isConstLeft = isConstNum(root->children[0], numLeft);
    bool isConstRight = isConstNum(root->children[2], numRight);
    if (isConstLeft && isConstRight) {
      if (root->rule == "expr expr PLUS term") {
        std::cout << "lis $3" << std::endl;
        std::cout << ".word " << numLeft + numRight << std::endl;
      } else {
        std::cout << "lis $3" << std::endl;
        std::cout << ".word " << numLeft - numRight << std::endl;
      }
    } else {
      if (root->children[0]->type == "int" && root->children[2]->type == "int") { 
        codeGenerate(root->children[0], table, label, offset, time, usedFunc);
        pushReg("3");
        codeGenerate(root->children[2], table, label, offset, time, usedFunc);
        popReg("5");
        std::cout << (root->tokens[2] == "PLUS" ? "add" : "sub") << " $3, $5, $3" << std::endl;
      } else if (root->tokens[2] == "PLUS" && root->children[0]->type == "int*" && root->children[2]->type == "int") {
        codeGenerate(root->children[0], table, label, offset, time, usedFunc);
        pushReg("3");
        codeGenerate(root->children[2], table, label, offset, time, usedFunc);
        std::cout << "mult $3, $4" << std::endl;
        std::cout << "mflo $3" << std::endl;
        popReg("5");
        std::cout << "add $3, $5, $3" << std::endl;
      } else if (root->tokens[2] == "PLUS" && root->children[0]->type == "int" && root->children[2]->type == "int*") {
        codeGenerate(root->children[0], table, label, offset, time, usedFunc);
        std::cout << "mult $3, $4" << std::endl;
        std::cout << "mflo $3" << std::endl;
        pushReg("3");
        codeGenerate(root->children[2], table, label, offset, time, usedFunc);
        popReg("5");
        std::cout << "add $3, $5, $3" << std::endl;
      } else if (root->tokens[2] == "MINUS" && root->children[0]->type == "int*" && root->children[2]->type == "int*") {
        codeGenerate(root->children[0], table, label, offset, time, usedFunc);
        pushReg("3");
        codeGenerate(root->children[2], table, label, offset, time, usedFunc);
        popReg("5");
        std::cout << "sub $3, $5, $3" << std::endl;
        std::cout << "div $3, $4" << std::endl;
        std::cout << "mflo $3" << std::endl;
      } else if (root->tokens[2] == "MINUS" && root->children[0]->type == "int*" && root->children[2]->type == "int") {
        codeGenerate(root->children[0], table, label, offset, time, usedFunc);
        pushReg("3");
        codeGenerate(root->children[2], table, label, offset, time, usedFunc);
        std::cout << "mult $3, $4" << std::endl;
        std::cout << "mflo $3" << std::endl;
        popReg("5");
        std::cout << "sub $3, $5, $3" << std::endl;
      }
    }
  }
  if (root->rule == "term term STAR factor" || root->rule == "term term SLASH factor" || root->rule == "term term PCT factor") {
    int numLeft, numRight;
    bool isConstLeft = isConstNum(root->children[0], numLeft);
    bool isConstRight = isConstNum(root->children[2], numRight);
    if (isConstLeft && isConstRight) {
      if (root->rule == "term term STAR factor") {
        std::cout << "lis $3" << std::endl;
        std::cout << ".word " << numLeft * numRight << std::endl;
      } else if (root->rule == "term term SLASH factor") {
        std::cout << "lis $3" << std::endl;
        std::cout << ".word " << numLeft / numRight << std::endl;
      } else {
        std::cout << "lis $3" << std::endl;
        std::cout << ".word " << numLeft % numRight << std::endl;
      }
    } else {
      codeGenerate(root->children[0], table, label, offset, time, usedFunc);
      pushReg("3");
      codeGenerate(root->children[2], table, label, offset, time, usedFunc);
      popReg("5");
      if (root->rule == "term term STAR factor") {
        std::cout << "mult $5, $3" << std::endl;
      } else {
        std::cout << "div $5, $3" << std::endl;
      }
      if (root->rule == "term term PCT factor") {
        std::cout << "mfhi $3" << std::endl;
      } else {
        std::cout << "mflo $3" << std::endl;
      }
    }
  }
  if (root->rule == "statements statements statement") {
    codeGenerate(root->children[0], table, label, offset, time, usedFunc);
    codeGenerate(root->children[1], table, label, offset, time, usedFunc);
  }
  if (root->rule == "dcls dcls dcl BECOMES NUM SEMI") {
    codeGenerate(root->children[0], table, label, offset, time, usedFunc);
    std::string s = root->children[3]->tokens[1];
    int num = stoi(s);
    std::cout << "lis $3" << std::endl;
    std::cout << ".word " << num << std::endl;
    if (label == "wain") {
      std::string idName = root->children[1]->children[1]->tokens[1];
      std::cout << "sw $3, " << table[label][idName].second << "($29)" << std::endl;
    } else {
      std::cout << "sw $3, -4($30)" << std::endl;
      std::cout << "sub $30, $30, $4" << std::endl;
    }
  }
  if (root->rule == "dcls dcls dcl BECOMES NULL SEMI") {
    codeGenerate(root->children[0], table, label, offset, time, usedFunc);
    std::string s = root->children[3]->tokens[1];
    std::cout << "add $3, $0, $11" << std::endl;
    std::string idName = root->children[1]->children[1]->tokens[1];
    std::cout << "sw $3, " << table[label][idName].second << "($29)" << std::endl;
  }
  if (root->rule == "statement lvalue BECOMES expr SEMI") {
    codeGenerate(root->children[2], table, label, offset, time, usedFunc);
    Tree *c = root->children[0];
    std::string s = c->rule;
    while (s == "lvalue LPAREN lvalue RPAREN") {
      c = c->children[1];
      s = c->rule;
    }
    if (s == "lvalue ID") {
      c = c->children[0];
      s = c->tokens[1];
      std::cout << "sw $3, " << table[label][s].second << "($29)" << std::endl;
    } else {
      std::cout << "add $16, $0, $3" << std::endl;
      codeGenerate(c->children[1], table, label, offset, time, usedFunc);
      std::cout << "sw $16, 0($3)" << std::endl;
    }
  }
  if (root->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
    offset++;
    int newOffset =  offset - 1;
    codeGenerate(root->children[2], table, label, offset, time, usedFunc);
    std::cout << "beq $3, $0, sK" << newOffset << std::endl;
    codeGenerate(root->children[5], table, label, offset, time, usedFunc);
    std::cout << "beq $0, $0, eI" << newOffset << std::endl;
    std::cout << "sK" << newOffset << ":" << std::endl;
    codeGenerate(root->children[9], table, label, offset, time, usedFunc);
    std::cout << "eI" << newOffset << ":" << std::endl;
  }
  if (root->rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
    offset++;
    int newOffset =  offset - 1;
    std::cout << "lo" << newOffset << ":" << std::endl;
    codeGenerate(root->children[2], table, label, offset, time, usedFunc);
    std::cout << "beq $3, $0, eW" << newOffset  << std::endl;
    codeGenerate(root->children[5], table, label, offset, time, usedFunc);
    std::cout << "beq $0, $0, lo" << newOffset  << std::endl;
    std::cout << "eW" << newOffset  << ":" << std::endl;
  }
  if (root->rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
    pushReg("1");
    codeGenerate(root->children[2], table, label, offset, time, usedFunc);
    std::cout << "add $1, $0, $3" << std::endl;
    pushReg("31");
    std::cout << "jalr $15" << std::endl;
    popReg("31");
    popReg("1");
  }
  if (root->rule == "statement DELETE LBRACK RBRACK expr SEMI") {
    codeGenerate(root->children[3], table, label, offset, time, usedFunc);
    std::cout << "add $1, $3, $0" << std::endl;
    pushReg("31");
    std::cout << "lis $5" << std::endl;
    std::cout << ".word delete" << std::endl;
    std::cout << "jalr $5" << std::endl;
    popReg("31");
    time++;
  }
  if (root->tokens[0] == "test" && root->tokens[1] == "expr" && root->tokens[3] == "expr") {
    bool ptrComp = root->children[0]->type == "int*" && root->children[2]->type == "int*";
    codeGenerate(root->children[0], table, label, offset, time, usedFunc);
    std::cout << "add $14, $3, $0" << std::endl;
    codeGenerate(root->children[2], table, label, offset, time, usedFunc);
    if (root->rule == "test expr EQ expr") { 
      if (ptrComp) {
        std::cout << "sltu $6, $3, $14" << std::endl;
        std::cout << "sltu $7, $14, $3" << std::endl;
      } else {
        std::cout << "slt $6, $3, $14" << std::endl;
        std::cout << "slt $7, $14, $3" << std::endl;
      }
      std::cout << "add $3, $6, $7" << std::endl;
      std::cout << "sub $3, $11, $3" << std::endl;
    } else if (root->rule == "test expr NE expr") { 
      if (ptrComp) {
        std::cout << "sltu $6, $3, $14" << std::endl;
        std::cout << "sltu $7, $14, $3" << std::endl;
      } else {
        std::cout << "slt $6, $3, $14" << std::endl;
        std::cout << "slt $7, $14, $3" << std::endl;
      }
      std::cout << "add $3, $6, $7" << std::endl;
    } else if (root->rule == "test expr LT expr") { 
      if (ptrComp) {
        std::cout << "sltu $3, $14, $3" << std::endl;
      } else {
        std::cout << "slt $3, $14, $3" << std::endl;
      }
    } else if (root->rule == "test expr LE expr") { 
      if (ptrComp) {
        std::cout << "sltu $3, $3, $14" << std::endl;
      } else {
        std::cout << "slt $3, $3, $14" << std::endl;
      }
      std::cout << "sub $3, $11, $3" << std::endl;
    } else if (root->rule == "test expr GE expr") { 
      if (ptrComp) {
        std::cout << "sltu $3, $14, $3" << std::endl;
      } else {
        std::cout << "slt $3, $14, $3" << std::endl;
      }
      std::cout << "sub $3, $11, $3" << std::endl;
    } else if (root->rule == "test expr GT expr") { 
      if (ptrComp) {
        std::cout << "sltu $3, $3, $14" << std::endl;
      } else {
        std::cout << "slt $3, $3, $14" << std::endl;
      }
    }
  }
}

int main() {
  Tree *root = nullptr;
  Tree *curr = root;
  TABLE table;
  std::string label;
  int offset = 0;
  std::vector<std::string> usedFunc;
  buildParseTree(std::cin, root, curr, usedFunc);
  buildSymbolTable(root, table, label, offset);
  offset = 0; 
  int time = 0;
  std::cout << ".import init" << std::endl;
  std::cout << ".import new" << std::endl;
  std::cout << ".import delete" << std::endl;
  std::cout << ".import print" << std::endl;
  std::cout << "lis $4" << std::endl;
  std::cout << ".word 4" << std::endl;
  std::cout << "lis $11" << std::endl;
  std::cout << ".word 1" << std::endl;
  std::cout << "lis $15" << std::endl;
  std::cout << ".word print" << std::endl;
  codeGenerate(root, table, label, offset, time, usedFunc);
  delete root;
}