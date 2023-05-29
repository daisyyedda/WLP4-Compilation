#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

typedef std::map<std::string, std::pair<std::vector<std::string>, std::map<std::string, std::pair<std::string, int>>>> TABLE;

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

void buildSymbolTable(Tree *tree, TABLE &table, std::string label, int offset, bool foundNum, bool endProg) {
  std::string leftType, rightType;
  if (tree->rule == "RETURN return" || (endProg && tree->tokens[0] == "NUM")) {
    return;
  }
  if (endProg && (tree->children.size() != 0) && tree->children[0]->tokens[0] == "NUM") {
    foundNum = true;
    tree->children[0]->type = "int";
  }
  if (tree->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
    label = "wain";
    std::string firstParam = tree->children[3]->children[0]->rule;
    firstParam = (firstParam == "type INT STAR") ? "int*" : "int";
    table[label].first.emplace_back(firstParam);
    table[label].first.emplace_back("int");
  } else if (tree->rule == "dcl type ID") {
    std::string dcl = tree->children[0]->rule;
    std::string id = tree->children[1]->tokens[1];
    dcl = (dcl == "type INT STAR") ? "int*" : "int";
    tree->children[1]->type = dcl;
    offset = -4 * table[label].second.size();
    table[label].second[id].second = offset;
    table[label].second[id].first = dcl;
  } else if (tree->rule == "factor ID" || tree->rule == "lvalue ID") {
    std::string id = tree->children[0]->tokens[1];
    foundNum = true;
    tree->children[0]->type = table[label].second[id].first;
    tree->type = tree->children[0]->type;
  } else if (tree->rule == "factor ID LPAREN RPAREN" || tree->rule == "factor ID LPAREN arglist RPAREN") {
    std::string id = tree->children[0]->tokens[1];
  } else if (tree->rule == "dcls dcls dcl BECOMES NUM SEMI" || tree->rule == "dcls dcls dcl BECOMES NULL SEMI") {
    std::string str = tree->children[1]->children[0]->rule;
    tree->children[3]->type =  (tree->rule == "dcls dcls dcl BECOMES NUM SEMI") ? "int" : "int*";
  }
  for (Tree *t : tree->children) {
    if (t->rule == "RETURN return") {
      endProg = true;
    } else if (t->rule == "SEMI ;") {
      endProg = false;
    }
    buildSymbolTable(t, table, label, offset, foundNum, endProg);
    if (endProg && t->type == "" && t->rule != "arglist expr COMMA arglist" && t->rule != "arglist expr") {
      std::string s = "";
      for (int i = 0; i < t->children.size(); i++) {
        if (t->children[i]->type != "") {
          s = t->children[i]->type;
        }
      }
      t->type = s;
    }
  }
}

void codeGenerate(Tree *root, TABLE &table, std::string label, int &offset) {
  if (root->rule == "start BOF procedures EOF") {
    codeGenerate(root->children[1], table, label, offset);
  }
  if (root->rule == "procedures procedure procedures") {
    codeGenerate(root->children[1], table, label, offset);
    codeGenerate(root->children[0], table, label, offset);
  }
  if (root->rule == "procedures main") {
    codeGenerate(root->children[0], table, label, offset);
  }
  if (root->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
    label = "wain";
    std::cout << ".import print" << std::endl;
    std::cout << "lis $4" << std::endl;
    std::cout << ".word 4" << std::endl;
    std::cout << "lis $11" << std::endl;
    std::cout << ".word 1" << std::endl;
    std::cout << "sub $29, $30, $4" << std::endl;
    int size = table[label].second.size() * 4;
    std::cout << "lis $12" << std::endl;
    std::cout << ".word " << size << std::endl;
    std::cout << "sub $30, $30, $12" << std::endl;
    std::cout << "sw $1, 0($29)" << std::endl;
    std::cout << "sw $2, -4($29)" << std::endl;   
    codeGenerate(root->children[8], table, label, offset);
    codeGenerate(root->children[9], table, label, offset);
    codeGenerate(root->children[11], table, label, offset);
    std::cout << "add $30, $29, $4" << std::endl;
    std::cout << "jr $31" << std::endl;
  }
  if (root->rule == "dcls .EMPTY" || root->rule == "statement .EMPTY") {
    return;
  }
  if (root->rule == "expr term" || root->rule == "term factor") {
    codeGenerate(root->children[0], table, label, offset);
  }
  if (root->rule == "factor ID") { 
    std::string idName = root->children[0]->tokens[1];
    std::cout << "lw $3, " << table[label].second[idName].second << "($29)" << std::endl;
  }
  if (root->rule == "factor NUM") {
    std::string num = root->children[0]->tokens[1];
    std::cout << "lis $3" << std::endl;
    std::cout << ".word " << num << std::endl;
  }
  if (root->rule == "factor LPAREN expr RPAREN") {
    codeGenerate(root->children[1], table, label, offset);
  }
  if (root->rule == "expr expr PLUS term" || root->rule == "expr expr MINUS term") {
    codeGenerate(root->children[0], table, label, offset);
    std::cout << "sw $3, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "add $30, $30, $4" << std::endl;
    std::cout << "lw $5, -4($30)" << std::endl;
    if (root->rule == "expr expr PLUS term") {
      std::cout << "add $3, $5, $3" << std::endl;
    } else {
      std::cout << "sub $3, $5, $3" << std::endl;
    }
  }
  if (root->rule == "term term STAR factor" || root->rule == "term term SLASH factor" || root->rule == "term term PCT factor") {
    codeGenerate(root->children[0], table, label, offset);
    std::cout << "sw $3, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "add $30, $30, $4" << std::endl;
    std::cout << "lw $5, -4($30)" << std::endl;
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
    std::string s = root->children[3]->rule.substr(4);
    int num = stoi(s);
    std::cout << "lis $3" << std::endl;
    std::cout << ".word " << num << std::endl;
    std::string idName = root->children[1]->children[1]->tokens[1];
    std::cout << "sw $3, " << table[label].second[idName].second << "($29)" << std::endl;
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
      std::cout << "sw $3, " << table[label].second[s].second << "($29)" << std::endl;
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
    std::cout << "sw $1, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "add $1, $0, $3" << std::endl;
    std::cout << "sw $31, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    std::cout << "lis $5" << std::endl;
    std::cout << ".word print" << std::endl;
    std::cout << "jalr $5" << std::endl;
    std::cout << "add $30, $4, $30" << std::endl;
    std::cout << "lw $31, -4($30)" << std::endl;
    std::cout << "add $30, $4, $30" << std::endl;
    std::cout << "lw $1, -4($30)" << std::endl;
  }
  if (root->rule == "test expr EQ expr") {
    codeGenerate(root->children[0], table, label, offset);
    std::cout << "sw $3, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "add $30, $4, $30" << std::endl;
    std::cout << "lw $5, -4($30)" << std::endl;
    std::cout << "slt $6, $3, $5" << std::endl;
    std::cout << "slt $7, $5, $3" << std::endl;
    std::cout << "add $3, $6, $7" << std::endl;
    std::cout << "sub $3, $3, $11" << std::endl;
  }
  if (root->rule == "test expr NE expr") {
    codeGenerate(root->children[0], table, label, offset);
    std::cout << "sw $3, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "add $30, $4, $30" << std::endl;
    std::cout << "lw $5, -4($30)" << std::endl;
    std::cout << "slt $6, $3, $5" << std::endl;
    std::cout << "slt $7, $5, $3" << std::endl;
    std::cout << "add $3, $6, $7" << std::endl;
  }
  if (root->rule == "test expr LT expr") {
    codeGenerate(root->children[0], table, label, offset);
    std::cout << "sw $3, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "add $30, $30, $4" << std::endl;
    std::cout << "lw $5, -4($30)" << std::endl;
    std::cout << "slt $3, $5, $3" << std::endl;
  }
  if (root->rule == "test expr LE expr") {
    codeGenerate(root->children[0], table, label, offset);
    std::cout << "sw $3, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "add $30, $4, $30" << std::endl;
    std::cout << "lw $5, -4($30)" << std::endl;
    std::cout << "slt $3, $3, $5" << std::endl;
    std::cout << "sub $3, $11, $3" << std::endl;
  }
  if (root->rule == "test expr GE expr") {
    codeGenerate(root->children[0], table, label, offset);
    std::cout << "sw $3, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "add $30, $4, $30" << std::endl;
    std::cout << "lw $5, -4($30)" << std::endl;
    std::cout << "slt $3, $5, $3" << std::endl;
    std::cout << "sub $3, $11, $3" << std::endl;
  }
  if (root->rule == "test expr GT expr") {
    codeGenerate(root->children[0], table, label, offset);
    std::cout << "sw $3, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    codeGenerate(root->children[2], table, label, offset);
    std::cout << "add $30, $4, $30" << std::endl;
    std::cout << "lw $5, -4($30)" << std::endl;
    std::cout << "slt $3, $3, $5" << std::endl;
  }
}

int main() {
  Tree *root = nullptr;
  Tree *curr = root;
  TABLE table;
  std::string label;
  int offset = 0;
  bool endProg = false;     
  bool foundNum = false; 
  buildParseTree(std::cin, root, curr);
  buildSymbolTable(root, table, label, offset, foundNum, endProg);
  offset = 0; 
  codeGenerate(root, table, label, offset);
  delete root;
}
