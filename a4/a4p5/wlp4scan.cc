#include <cmath>
#include <vector>
#include <string>
#include <iostream>

bool isDigit(char c) {
  return (c >= 48 && c <= 57) ? true : false;
}

bool isLetter(char c) {
  return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) ? true : false;
}

void printNum(long long num) {
  if (num > std::pow(2, 31) - 1) {
    std::cerr << "ERROR: Number is out of range." << std::endl;
  } else {
    std::cout << "NUM " << num << std::endl;
  }
}

void printID(std::string ID) {
  if (ID == "wain" || ID == "int" || ID == "if" || ID == "else" || 
      ID == "while" || ID == "println" || ID == "return" || ID == "NULL" || 
      ID == "new" || ID == "delete") {
    std::cerr << "ERROR: ID is a keyword." << std::endl;
  } else {
    std::cout << "ID " << ID << std::endl;
  }
}

void printChar(char c, bool &valid) {
  valid = true;
  if (c == '(') {
    std::cout << "LPAREN " << c << std::endl;
  } else if (c == ')') {
    std::cout << "RPAREN " << c << std::endl;
  } else if (c == '{') {
    std::cout << "LBRACE " << c << std::endl;
  } else if (c == '}') {
    std::cout << "RBRACE " << c << std::endl;
  } else if (c == '[') {
    std::cout << "LBRACK " << c << std::endl;
  } else if (c == ']') {
    std::cout << "RBRACK " << c << std::endl;
  }  else if (c == '=') {
    std::cout << "BECOMES " << c << std::endl;
  } else if (c == '<') {
    std::cout << "LT " << c << std::endl;
  } else if (c == '>') {
    std::cout << "GT " << c << std::endl;
  } else if (c == '+') {
    std::cout << "PLUS " << c << std::endl;
  } else if (c == '-') {
    std::cout << "MINUS " << c << std::endl;
  } else if (c == '*') {
    std::cout << "STAR " << c << std::endl;
  } else if (c == '/') {
    std::cout << "SLASH " << c << std::endl;
  } else if (c == '%') {
    std::cout << "PCT " << c << std::endl;
  } else if (c == ',') {
    std::cout << "COMMA " << c << std::endl;
  } else if (c == ';') {
    std::cout << "SEMI " << c << std::endl;
  } else if (c == '&') {
    std::cout << "AMP " << c << std::endl;
  } else if (isLetter(c)) {
    std::cout << "ID " << c << std::endl;
  } else if (isDigit(c)) {
    std::cout << "NUM " << c << std::endl;
  } else if (c == 32 || c == 9 || c == 10 || c == 0) {
    return;
  } else {
    valid = false;
    std::cerr << "ERROR: Char is not a valid token." << std::endl;
  } 
}

void printKeyword(std::string s, bool &valid) {
  valid = true;
  if (s == "if") {
    std::cout << "IF " << s << std::endl;
  } else if (s == "else") {
    std::cout << "ELSE " << s << std::endl;
  } else if (s == "while") {
    std::cout << "WHILE " << s << std::endl;
  } else if (s == "println") {
    std::cout << "PRINTLN " << s << std::endl;
  } else if (s == "wain") {
    std::cout << "WAIN " << s << std::endl;
  } else if (s == "int") {
    std::cout << "INT " << s << std::endl;
  } else if (s == "==") {
    std::cout << "EQ " << s << std::endl;
  } else if (s == "!=") {
    std::cout << "NE " << s << std::endl;
  } else if (s == "<=") {
    std::cout << "LE " << s << std::endl;
  } else if (s == ">=") {
    std::cout << "GE " << s << std::endl;
  } else if (s == "new") {
    std::cout << "NEW " << s << std::endl;
  } else if (s == "delete") {
    std::cout << "DELETE " << s << std::endl;
  } else if (s == "NULL") {
    std::cout << "NULL " << s << std::endl;
  } else if (s == "return") {
    std::cout << "RETURN " << s << std::endl;
  } else {
    valid = false;
  }
}

int main() {
  std::istream& in = std::cin;
  std::string s;
  std::vector<char> tempID;
  std::vector<char> tempNum;
  bool isValidChar = false;
  bool isValidKeyword = false;
  bool maybeNum = false;
  bool maybeID = false;
  bool maybeDivide = false;
  
  while (in >> s) {
    if (s == "//") {
      std::getline(in, s);   
    } else if (s.length() == 1 && s[0] == 10) {
      std::getline(in, s);   
    }  else if (s.length() == 1) {
      printChar(s[0], isValidChar);
      isValidChar = false;
    } else {
      printKeyword(s, isValidKeyword);
      if (isValidKeyword) {
        isValidKeyword = false;
        continue;
      } else {
        std::vector<char> item;
        for (int i = 0; i <= s.length(); ++i) {
          if (maybeDivide && s[i] == '/') {
            maybeDivide = false;
            std::getline(in, s);
            break;
          }
          if (maybeDivide) {
            std::cout << "SLASH /" << std::endl;
            maybeDivide = false;
          }
          if (tempNum.size() != 0 && !isDigit(s[i])) {
            std::string Num(tempNum.begin(), tempNum.end());
            long long num = std::stoll(Num);
            printNum(num);
            item.clear();
            tempNum.clear();
            maybeNum = false;
          }
          if (tempID.size() != 0 && !isLetter(s[i]) && !isDigit(s[i])) {
            std::string ID(tempID.begin(), tempID.end());
            printID(ID);
            item.clear();
            tempID.clear();
            maybeID = false;
          }
          item.emplace_back(s[i]);
          if (maybeNum && isDigit(s[i])) {
            tempNum.emplace_back(s[i]);
          }
          if (maybeID && (isLetter(s[i]) || isDigit(s[i]))) {
            tempID.emplace_back(s[i]);
          }
          if (tempID.size() == 0) {
            maybeID = isLetter(s[i]) ? true : false;  
            if (maybeID) {
              tempID.emplace_back(s[i]);
            }  
          } else {
            maybeID = (isLetter(s[i]) || isDigit(s[i])) ? true : false; 
          } 
          if (tempNum.size() == 0) {
            maybeNum = isDigit(s[i]) ? true : false;
            if (maybeNum && !maybeID) {
              tempNum.emplace_back(s[i]);
            }
          } 
          
          // non-letter/digit/slash character (unless at the end of str)
          if (item.size() == 1) {
            if ((item[0] != '/' && !isLetter(item[0]) && !isDigit(item[0])) || 
               (i == s.length() - 1 && !isLetter(item[0]) && !isDigit(item[0]))) {
              printChar(item[0], isValidChar);
            }
            maybeDivide = item[0] == '/' ? true : false;
            if (isValidChar) {
              item.clear();
              isValidChar = false;
            }
          } else {
            std::string str(item.begin(), item.end());
            if (str == "//") {
              std::getline(in, s);  
              break;
            } else {
              printKeyword(str, isValidKeyword);
              if (isValidKeyword) {
                item.clear();
                tempID.clear();
                tempNum.clear();
                isValidKeyword = false;
              }
            } 
          }
        }
      }
    } 
  }
}
