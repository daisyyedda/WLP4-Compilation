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

void buildParseTree(std::istream &in, Tree *&root, Tree *&curr) {
  std::string token;
  Tree *tmp = nullptr;
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

void codeGenerate(Tree *root, TABLE &table, std::string label, int &offset) {
  if (root->rule == "start BOF procedures EOF") {
    codeGenerate(root->children[1], table, label, offset);
  }
  if (root->rule == "procedures procedure procedures") {
    codeGenerate(root->children[0], table, label, offset);
    codeGenerate(root->children[1], table, label, offset);
  }
  if (root->rule == "procedures main") {
    codeGenerate(root->children[0], table, label, offset);
  }
  if (root->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
    label = root->children[1]->tokens[1];
    std::string procName = label;
    std::cout << procName << ": " << std::endl;
    std::cout << "lis $4" << std::endl;
    std::cout << ".word 4" << std::endl;
    std::cout << "lis $11" << std::endl;
    std::cout << ".word 1" << std::endl;
    std::cout << "sub $29, $30, $4" << std::endl;
    std::cout << "sw $1, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    std::cout << "sw $2, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4 " << std::endl;
    for (int i = 2; i < (int)table[label].size(); i++) {
      std::cout << "sub $30, $30, $4" << std::endl;
    }  
    codeGenerate(root->children[8], table, label, offset);
    codeGenerate(root->children[9], table, label, offset);
    codeGenerate(root->children[11], table, label, offset);
    for (int i = 2; i < (int)table[label].size(); i++) {
      std::cout << "add $30, $30, $4" << std::endl;
    } 
    std::cout << "jr $31" << std::endl;
  }
  if (root->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
    label = "F" + root->children[1]->tokens[1];
    std::cout << label << ": " << std::endl;
    std::cout << "sub $29, $30, $4" << std::endl;
    pushReg("5");
    pushReg("6");
    pushReg("7");
    codeGenerate(root->children[6], table, label, offset);
    codeGenerate(root->children[7], table, label, offset);
    codeGenerate(root->children[9], table, label, offset);
    popReg("7");
    popReg("6");
    popReg("5");
    std::cout << "add $30, $29, $4" << std::endl;
    std::cout << "jr $31" << std::endl;  
  }
  if (root->rule == "params paramlist" || root->rule == "paramlist dcl" || root->rule == "arglist expr") {
    codeGenerate(root->children[0], table, label, offset);
  }
  if (root->rule == "paramlist dcl COMMA paramlist" || root->rule == "arglist expr COMMA arglist") {
    codeGenerate(root->children[0], table, label, offset);
    codeGenerate(root->children[2], table, label, offset);
  }
  if (root->rule == "dcls .EMPTY" || root->rule == "statement .EMPTY" || root->rule == "params .EMPTY") {
    return;
  }
  if (root->rule == "expr term" || root->rule == "term factor") {
    codeGenerate(root->children[0], table, label, offset);
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
      codeGenerate(tmp->children[1], table, label, offset);
    }    
  }
  if (root->rule == "factor STAR factor") {
    codeGenerate(root->children[1], table, label, offset);
    std::cout << "lw $3, 0($3)" << std::endl;
  }
  if (root->rule == "factor LPAREN expr RPAREN") {
    codeGenerate(root->children[1], table, label, offset);
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
      codeGenerate(argList->children[0], table, label, offset);
      pushReg("3");
      time++;
      argList = argList->children[2];
    }
    codeGenerate(argList->children[0], table, label, offset);
    pushReg("3");        
    std::cout << "lis $5" << std::endl;
    std::cout << ".word " << "F" + root->children[0]->tokens[1] << std::endl;
    std::cout << "jalr $5" << std::endl;
    for (int i = 0; i < time; i++) { popReg("31"); }
    popReg("31");
    popReg("29");
  }

  if (root->rule == "expr expr PLUS term" || root->rule == "expr expr MINUS term") {
    codeGenerate(root->children[0], table, label, offset);
    pushReg("3");
    codeGenerate(root->children[2], table, label, offset);
    popReg("5");
    if (root->rule == "expr expr PLUS term") {
      std::cout << "add $3, $5, $3" << std::endl;
    } else {
      std::cout << "sub $3, $5, $3" << std::endl;
    }
  }
  if (root->rule == "term term STAR factor" || root->rule == "term term SLASH factor" || root->rule == "term term PCT factor") {
    codeGenerate(root->children[0], table, label, offset);
    pushReg("3");
    codeGenerate(root->children[2], table, label, offset);
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
  if (root->rule == "statements statements statement") {
    codeGenerate(root->children[0], table, label, offset);
    codeGenerate(root->children[1], table, label, offset);
  }
  if (root->rule == "dcls dcls dcl BECOMES NUM SEMI") {
    codeGenerate(root->children[0], table, label, offset);
    std::string s = root->children[3]->tokens[1];
    int num = stoi(s);
    std::cout << "lis $3" << std::endl;
    std::cout << ".word " << num << std::endl;
    std::string idName = root->children[1]->children[1]->tokens[1];
    std::cout << "sw $3, " << table[label][idName].second << "($29)" << std::endl;
  }
  if (root->rule == "dcls dcls dcl BECOMES NULL SEMI") {
    codeGenerate(root->children[0], table, label, offset);
    std::string s = root->children[3]->tokens[1];
    std::cout << "add $3, $0, $11" << std::endl;
    std::string idName = root->children[1]->children[1]->tokens[1];
    std::cout << "sw $3, " << table[label][idName].second << "($29)" << std::endl;
  }
  if (root->rule == "statement lvalue BECOMES expr SEMI") {
    codeGenerate(root->children[2], table, label, offset);
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
      pushReg("3");
      codeGenerate(c->children[1], table, label, offset);
      popReg("5");
      std::cout << "sw $5, 0($3)" << std::endl;
    }
  }
  if (root->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
    offset++;
    int newOffset =  offset - 1;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "beq $3, $0, else" << newOffset << std::endl;
    codeGenerate(root->children[5], table, label, offset);
    std::cout << "beq $0, $0, endif" << newOffset << std::endl;
    std::cout << "else" << newOffset << ":" << std::endl;
    codeGenerate(root->children[9], table, label, offset);
    std::cout << "endif" << newOffset << ":" << std::endl;
  }
  if (root->rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
    offset++;
    int newOffset =  offset - 1;
    std::cout << "loop" << newOffset << ":" << std::endl;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "beq $3, $0, endWhile" << newOffset  << std::endl;
    codeGenerate(root->children[5], table, label, offset);
    std::cout << "beq $0, $0, loop" << newOffset  << std::endl;
    std::cout << "endWhile" << newOffset  << ":" << std::endl;
  }
  if (root->rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
    pushReg("1");
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "add $1, $0, $3" << std::endl;
    pushReg("31");
    std::cout << "lis $5" << std::endl;
    std::cout << ".word print" << std::endl;
    std::cout << "jalr $5" << std::endl;
    popReg("31");
    popReg("1");
  }
  if (root->rule == "test expr EQ expr") {
    codeGenerate(root->children[0], table, label, offset);
    pushReg("3");
    codeGenerate(root->children[2], table, label, offset);
    popReg("5");
    std::cout << "slt $6, $3, $5" << std::endl;
    std::cout << "slt $7, $5, $3" << std::endl;
    std::cout << "add $3, $6, $7" << std::endl;
    std::cout << "sub $3, $3, $11" << std::endl;
  }
  if (root->rule == "test expr NE expr") {
    codeGenerate(root->children[0], table, label, offset);
    pushReg("3");
    codeGenerate(root->children[2], table, label, offset);
    popReg("5");
    std::cout << "slt $6, $3, $5" << std::endl;
    std::cout << "slt $7, $5, $3" << std::endl;
    std::cout << "add $3, $6, $7" << std::endl;
  }
  if (root->rule == "test expr LT expr") {
    codeGenerate(root->children[0], table, label, offset);
    pushReg("3");
    codeGenerate(root->children[2], table, label, offset);
    popReg("5");
    std::cout << "slt $3, $5, $3" << std::endl;
  }
  if (root->rule == "test expr LE expr") {
    codeGenerate(root->children[0], table, label, offset);
    pushReg("3");
    codeGenerate(root->children[2], table, label, offset);
    popReg("5");
    std::cout << "slt $3, $3, $5" << std::endl;
    std::cout << "sub $3, $11, $3" << std::endl;
  }
  if (root->rule == "test expr GE expr") {
    codeGenerate(root->children[0], table, label, offset);
    pushReg("3");
    codeGenerate(root->children[2], table, label, offset);
    popReg("5");
    std::cout << "slt $3, $5, $3" << std::endl;
    std::cout << "sub $3, $11, $3" << std::endl;
  }
  if (root->rule == "test expr GT expr") {
    codeGenerate(root->children[0], table, label, offset);
    pushReg("3");
    codeGenerate(root->children[2], table, label, offset);
    popReg("5");
    std::cout << "slt $3, $3, $5" << std::endl;
  }
}

int main() {
  Tree *root = nullptr;
  Tree *curr = root;
  TABLE table;
  std::string label;
  int offset = 0;
  buildParseTree(std::cin, root, curr);
  buildSymbolTable(root, table, label, offset);
  offset = 0; 
  std::cout << "beq $0, $0, wain" << std::endl;
  codeGenerate(root, table, label, offset);
  delete root;
}
