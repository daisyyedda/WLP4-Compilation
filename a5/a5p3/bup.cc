#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>

int main() {
  std::istream& in = std::cin;
  std::string s;

  std::queue<std::string> input;
  std::queue<std::string> reduction;
  std::vector<std::pair<std::string, std::vector<std::string>>> cfg;
  
  std::getline(in, s); // CFG section (skip header)
  while (std::getline(in, s)) {
    std::string left = "";
    std::vector<std::string> right;
    if (s == ".INPUT") { 
      break; 
    } else {
      std::istringstream iss(s);
      while (iss) {
        std::string substr;
        iss >> substr;
        if (left == "") {
          left = substr;
        } else {
          right.emplace_back(substr);
        }
      }
    }
    std::pair<std::string, std::vector<std::string>> rule;
    right.pop_back();
    rule.first = left;
    rule.second = right;
    cfg.emplace_back(rule);
  }

  while (in >> s) {   // Input section
    if (s == ".ACTIONS") { 
      break; 
    } else {
      input.push(s);
    }
  }

  std::getline(in, s); // Actions section (skip header)
  while (in >> s) {
    if (s == ".END") { 
      break; 
    }
    if (s == "print") {
      std::queue<std::string> inputCopy = input;
      std::queue<std::string> reductionCopy = reduction;
      while (!reductionCopy.empty()) {
        std::cout << reductionCopy.front() << " ";
		    reductionCopy.pop();
	    }
      std::cout << ".";
      while (!inputCopy.empty()) {
        std::cout << " " << inputCopy.front();
		    inputCopy.pop();
	    }     
      std::cout << std::endl;
    } else if (s == "shift") {
      reduction.push(input.front());  
      input.pop();
    } else if (s == "reduce"){
      continue;
    } else {
      int num = std::stoi(s);
      std::pair<std::string, std::vector<std::string>> rule = cfg[num];
      std::queue<std::string> tmp;
      int newLen;
      if (rule.second.size() == 1 && rule.second[0] == ".EMPTY") {
        newLen = reduction.size();
      } else {
        newLen = reduction.size() - rule.second.size();
      }
      while (newLen--) {
        tmp.push(reduction.front());
        reduction.pop();
      }
      reduction = tmp;
      reduction.push(rule.first);
    }
  }
}