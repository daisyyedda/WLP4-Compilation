#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <bits/stdc++.h>
#include "scanner.h"

// print the binary encoding of an instruction
void printInstr(int instr) {
  std::cout << char(instr >> 24) << char(instr >> 16) 
  << char(instr >> 8) << char(instr);
}

// store instructions and labels
void processFile(std::vector<std::vector<Token>> &lines, std::map<std::string, int> &labels) {
  int address = 0; 
  std::string line;
  while (getline(std::cin, line)) {
    std::vector<Token> tokenLine = scan(line);
    bool isInstr = false;
    if (tokenLine.size() == 0) {
      continue;
    }
    for (auto &token : tokenLine) {
      if (token.getKind() != Token::LABEL) {
        isInstr = true;
      } else if (labels.count(token.getLexeme()) == 0) {
        labels[token.getLexeme()] = address;
      } else {
        throw ScanningFailure("ERROR: Duplicate label.");
      }
    }
    lines.emplace_back(tokenLine);
    address = (isInstr ? address += 4 : address);
  } 
}

// handle .word
void dotWord(const std::vector<Token> &tokenLine, std::map<std::string, int> &labels, int idx) {
  if (tokenLine.size() == idx + 2 && tokenLine.at(idx + 1).getKind() == Token::ID) {
    std::string label = tokenLine.at(idx + 1).getLexeme();  
    if (labels.count(label + ":") == 0) {
      throw ScanningFailure("ERROR: No such label.");    
    } else {
      printInstr(labels[label + ":"]);    
    }
  } else if (tokenLine.size() == idx + 2 && (tokenLine.at(idx + 1).getKind() == Token::INT || 
                                             tokenLine.at(idx + 1).getKind() == Token::HEXINT)) {                                 
    int64_t i = tokenLine.at(idx + 1).toNumber();
    Token::Kind kind = tokenLine.at(idx + 1).getKind();
    if ((kind == Token::INT && (i > std::pow(2, 32) - 1 || i < std::pow(-2, 31))) ||
      (kind == Token::HEXINT && (i > 0xffffffff))) {
      throw ScanningFailure{"ERROR: Invalid input (integer out of range)."};
    } else {
      printInstr(i);
    }
  } else {
    throw ScanningFailure{"ERROR: Invalid input (.word)."};
  }
}

// handle add, sub, slt, and sltu
void addSubSltSltu(const std::vector<Token> &tokenLine, int idx) {
  if (tokenLine.size() == idx + 6 &&
  tokenLine.at(idx + 1).getKind() == Token::REG &&
  tokenLine.at(idx + 3).getKind() == Token::REG &&
  tokenLine.at(idx + 5).getKind() == Token::REG &&
  tokenLine.at(idx + 2).getKind() == Token::COMMA &&
  tokenLine.at(idx + 4).getKind() == Token::COMMA) {
    uint32_t d = tokenLine[idx + 1].toNumber();
    uint32_t s = tokenLine[idx + 3].toNumber();
    uint32_t t = tokenLine[idx + 5].toNumber();
    if (d >= 0 && d <= 31 && s >= 0 && s <= 31 && t >= 0 && t <= 31) {
      std::string op = tokenLine.at(idx).getLexeme();
      int opcode = ((op == "add") ? 32 : (op == "sub") ? 34 : (op == "slt") ? 42 : 43);
      uint32_t instr = (s << 21) | (t << 16) | (d << 11) | opcode;
      printInstr(instr);
    } else {
      throw ScanningFailure{"ERROR: Invalid registers (out of bound)."};
    }
  } else {
    throw ScanningFailure{"ERROR: Invalid input (add / sub / slt / sltu)."};
  }
}

// handle beq and bne
void beqBne(const std::vector<Token> &tokenLine, std::map<std::string, int> &labels, int idx, int PC) {
  if (tokenLine.size() == idx + 6 &&
  tokenLine.at(idx + 1).getKind() == Token::REG &&
  tokenLine.at(idx + 3).getKind() == Token::REG &&
  (tokenLine.at(idx + 5).getKind() == Token::INT || 
  tokenLine[idx + 5].getKind() == Token::HEXINT ||
  tokenLine.at(idx + 5).getKind() == Token::ID) &&
  tokenLine.at(idx + 2).getKind() == Token::COMMA &&
  tokenLine.at(idx + 4).getKind() == Token::COMMA) {
    uint32_t s = tokenLine[idx + 1].toNumber();
    uint32_t t = tokenLine[idx + 3].toNumber();
    if (s >= 0 && s <= 31 && t >= 0 && t <= 31) {
      Token::Kind kind = tokenLine.at(idx + 5).getKind();
      if (kind != Token::ID) {
        int64_t i = tokenLine[idx + 5].toNumber();
        if ((kind == Token::INT && (i > 32767 || i < -32768)) || 
            (kind == Token::HEXINT && i > 0xffff)) {
          throw ScanningFailure("ERROR: Invalid input (integer out of range).");
        } else {
          std::string op = tokenLine.at(idx).getLexeme();
          int opcode = ((op == "beq") ? 4 : 5);
          int32_t instr = ((i < 0) ? (opcode << 26) | (s << 21) | (t << 16) | (i & 0xFFFF) 
                                   : (opcode << 26) | (s << 21) | (t << 16) | i);
          printInstr(instr);
        }
      } else {
        std::string label = tokenLine.at(idx + 5).getLexeme();  
        if (labels.count(label + ":") == 0) {
          throw ScanningFailure("ERROR: No such label.");    
        } else {
          std::string op = tokenLine.at(idx).getLexeme();
          int opcode = ((op == "beq") ? 4 : 5);
          int16_t offset =  (labels[label + ":"] - PC) / 4;
          if (offset > 32767 || offset < -32768) {
            throw ScanningFailure("ERROR: Invalid input (out of range).");
          } else {
            int32_t instr = ((offset < 0) ? (opcode << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF) 
                                          : (opcode << 26) | (s << 21) | (t << 16) | offset);
            printInstr(instr);   
          } 
        }  
      }
    } else {
      throw ScanningFailure{"ERROR: Invalid registers (out of bound)."};
    }
  } else {
    throw ScanningFailure{"ERROR: Invalid input (beq / bne)."};
  }
}

// handle lis, mflo, and mfhi
void lisMlfoMfhi(const std::vector<Token> &tokenLine, int idx) {
  if (tokenLine.size() == idx + 2 &&
  tokenLine[idx + 1].getKind() == Token::REG) {
    uint32_t d = tokenLine[idx + 1].toNumber();
    if (d >= 0 && d <= 31) {
      std::string op = tokenLine.at(idx).getLexeme();
      int opcode = ((op == "mfhi") ? 16 : (op == "mflo") ? 18 : 20);
      uint32_t instr = (d << 11) | opcode;
      printInstr(instr);
    } else {
      throw ScanningFailure("ERROR: Invalid registers (out of bound).");
    }
  } else {
    throw ScanningFailure("ERROR: Invalid input (lis / mflo / mfhi).");
  }
}

// handle mult, multu, div, and divu
void multMultuDivDivu(const std::vector<Token> &tokenLine, int idx) {
  if (tokenLine.size() == idx + 4 &&
  tokenLine.at(idx + 1).getKind() == Token::REG &&
  tokenLine.at(idx + 2).getKind() == Token::COMMA &&
  tokenLine.at(idx + 3).getKind() == Token::REG) {
    uint32_t s = tokenLine[idx + 1].toNumber();
    uint32_t t = tokenLine[idx + 3].toNumber();
    if (s >= 0 && s <= 31 && t >= 0 && t <= 31) {
      std::string op = tokenLine.at(idx).getLexeme();
      int opcode = ((op == "mult") ? 24 : (op == "multu") ? 25 : (op == "div") ? 26 : 27);
      uint32_t instr = (s << 21) | (t << 16) | opcode;
      printInstr(instr);
    } else {
      throw ScanningFailure{"ERROR: Invalid registers (out of bound)."};
    }
  } else {
    throw ScanningFailure{"ERROR: Invalid input (mult / multu / div / divu)."};
  }
}

// handle sw and lw
void swLw(const std::vector<Token> &tokenLine, int idx) {
  if (tokenLine.size() == idx + 7 &&
  tokenLine[idx + 1].getKind() == Token::REG &&
  tokenLine[idx + 2].getKind() == Token::COMMA &&
  (tokenLine[idx + 3].getKind() == Token::INT ||
  tokenLine[idx + 3].getKind() == Token::HEXINT) &&
  tokenLine[idx + 4].getKind() == Token::LPAREN &&
  tokenLine[idx + 5].getKind() == Token::REG &&
  tokenLine[idx + 6].getKind() == Token::RPAREN) {
    uint32_t s = tokenLine[idx + 5].toNumber();
    uint32_t t = tokenLine[idx + 1].toNumber();
    int16_t i = tokenLine[idx + 3].toNumber();
    if ((tokenLine[idx + 3].getKind() == Token::INT && (i > 32767 || i < -32768)) ||
        (tokenLine[idx + 3].getKind() == Token::HEXINT && i > 0xffff)) {
      throw ScanningFailure("ERROR: Invalid input (integer out of range).");
    } else {
      if (s >= 0 && s <= 31 && t >= 0 && t <= 31) {
        std::string op = tokenLine.at(idx).getLexeme();
        int opcode = ((op == "lw") ? 35 : 43);
        int32_t instr = ((i < 0) ? (opcode << 26) | (s << 21) | (t << 16) | (i & 0xFFFF) 
                                  : (opcode << 26) | (s << 21) | (t << 16) | i);
        printInstr(instr);
      } else {
        throw ScanningFailure("ERROR: Invalid registers (out of bound).");
      }
    }
  } else {
    throw ScanningFailure("ERROR: Invalid input (sw / lw).");
  }
}

// handle jr and jalr
void jrJalr(const std::vector<Token> &tokenLine, int idx) {
  if (tokenLine.size() == idx + 2 &&
  tokenLine[idx + 1].getKind() == Token::REG) {
    uint32_t s = tokenLine[idx + 1].toNumber();
    if (s >= 0 && s <= 31) {
      std::string op = tokenLine.at(idx).getLexeme();
      int opcode = ((op == "jr") ? 8 : 9);
      uint32_t instr = (s << 21) | opcode;
      printInstr(instr);
    } else {
      throw ScanningFailure("ERROR: Invalid registers (out of bound).");
    }
  } else {
    throw ScanningFailure("ERROR: Invalid input (jr / jalr).");
  }
}

// read assembly file and translate into machine code
int main() {
  std::string line;
  std::vector<std::vector<Token>> lines;
  std::map<std::string, int> labels;
  try { 
    processFile(lines, labels);
    int PC = -4;
    for (const auto &tokenLine : lines) {
      int idx = 0;
      // count the number of tokens
      while ((idx < tokenLine.size()) && (tokenLine.at(idx).getKind() == Token::LABEL)) {
        idx++;
      }
      if (idx < tokenLine.size()) {
        Token::Kind kind = tokenLine.at(idx).getKind();
        std::string lex = tokenLine.at(idx).getLexeme(); 
        if (kind != Token::WORD && kind != Token::ID) {
          throw ScanningFailure{"ERROR: Invalid input (first token must be WORD or ID)."};
        } else {
          PC += 4;
          if (kind == Token::WORD && lex == ".word") {
            dotWord(tokenLine,labels, idx);
          } else if (kind == Token::ID) {
            if (lex == "jr" || lex == "jalr") {
              jrJalr(tokenLine, idx);
            } else if (lex == "add" || lex == "sub" || lex == "slt" || lex == "sltu") {
              addSubSltSltu(tokenLine, idx);
            } else if (lex == "mult" || lex == "multu" || lex == "div" || lex == "divu") {
              multMultuDivDivu(tokenLine, idx);
            } else if (lex == "lw" || lex == "sw") { 
              swLw(tokenLine, idx);
            } else if (lex == "lis" || lex == "mflo" || lex == "mfhi") {
              lisMlfoMfhi(tokenLine, idx);
            } else if (lex == "beq" || lex == "bne") { 
              beqBne(tokenLine,labels, idx, PC + 4);
            } else {
              throw ScanningFailure{"ERROR: Invalid input (undefined lexeme)."};
            }
          } else {
            throw ScanningFailure{"ERROR: Invalid input (invalid syntax)."};
          }
        }
      } 
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  return 0;
}