#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>

int checkReduce(std::vector<std::pair<std::string, std::pair<std::string, std::string>>> reductions, 
int stateNum, std::string lookAhead) {
  for (auto& t : reductions) {
    if (t.first == std::to_string(stateNum) && t.second.second == lookAhead) {
      return std::stoi(t.second.first);
    }
  }
  return -1; 
}

int checkNextState(std::vector<std::pair<std::string, std::pair<std::string, std::string>>> transitions, 
int stateNum, std::string lookAhead) {
  for (auto& t : transitions) {
    if (t.second.first == std::to_string(stateNum) && t.first == lookAhead) {
      return std::stoi(t.second.second);
    }
  }
  return -1; 
}

void printInput(std::queue<std::string> input, std::stack<std::string> reduction) {
  std::stack<std::string> tmp;
  while (!reduction.empty()) {
    tmp.push(reduction.top());
    reduction.pop();
  }  
  while (!tmp.empty()) {
    std::string t = tmp.top();
    std::cout << t << " ";
    tmp.pop();
    reduction.push(t); 
  }
  std::cout << ".";
  if (!input.empty()) std::cout << " ";
  while (!input.empty()) {
    std::cout << input.front();
		input.pop();
    if (!input.empty()) {
      std::cout << " ";
    }
	}     
  std::cout << std::endl;
}

void shiftInput(std::queue<std::string>& input, std::stack<std::string>& reduction, std::stack<int>& stateStack, 
std::vector<std::pair<std::string, std::pair<std::string, std::string>>> transitions) {
  reduction.push(input.front());  
  stateStack.push(checkNextState(transitions, stateStack.top(), input.front()));
  input.pop();
  printInput(input, reduction);
}

void reduceInput(std::vector<std::pair<std::string, std::vector<std::string>>> cfg, 
std::queue<std::string>& input, std::stack<std::string>& reduction, std::string ruleNum, 
std::stack<int>& stateStack) {
  int num = std::stoi(ruleNum);
  std::pair<std::string, std::vector<std::string>> rule = cfg[num];
  int afterSize = rule.second.size();
  if (rule.second.size() == 1 && rule.second[0] == ".EMPTY") {
    afterSize = 0;
  }
  while (afterSize--) {
    stateStack.pop();
    reduction.pop();
  }
  reduction.push(rule.first);
  printInput(input, reduction);
}

int main() {
  std::istream& in = std::cin;
  std::string s;
  
  std::queue<std::string> input;  
  std::stack<std::string> reduction;
  std::vector<std::pair<std::string, std::vector<std::string>>> cfg;
  std::vector<std::pair<std::string, std::pair<std::string, std::string>>> transitions;
  std::vector<std::pair<std::string, std::pair<std::string, std::string>>> reductions;
  int initialState;
  std::string startSymbol;
  
  // .CFG section (skip header)
  std::getline(in, s); 
  while (std::getline(in, s)) {
    std::string left = "";
    std::vector<std::string> right;
    if (s == ".INPUT") break; 
    std::istringstream iss(s);
    while (iss) {
      std::string substr;
      iss >> substr;
      if (left == "") {
        left = substr;
      } else {
        right.emplace_back(substr);
      }
      if (cfg.size() == 0) startSymbol = substr;
    }
    std::pair<std::string, std::vector<std::string>> rule;
    right.pop_back();
    rule.first = left;
    rule.second = right;
    cfg.emplace_back(rule);
  }

  // .INPUT section
  while (in >> s) {   
    if (s == ".TRANSITIONS") break; 
    input.push(s);
  }

  // .TRANSITIONS section
  while (in >> s) {
    if (s == ".REDUCTIONS") break; 
    std::string fromState, symbol, toState, newLine;
    std::pair<std::string, std::pair<std::string, std::string>> transition;
    fromState = s;
    in >> symbol;
    in >> toState;
    transition.second.first = fromState;
    transition.first = symbol;
    if (symbol == "BOF") initialState = std::stoi(fromState);
    transition.second.second = toState;
    transitions.emplace_back(transition);
  }

  // .REDUCTIONS section
  while (in >> s) {
    if (s == ".END") break; 
    std::string stateNumber, ruleNumber, tag, newLine;
    std::pair<std::string, std::pair<std::string, std::string>> reduction;
    stateNumber = s;
    in >> ruleNumber;
    in >> tag;
    reduction.first = stateNumber;
    reduction.second.first = ruleNumber;
    reduction.second.second = tag;
    reductions.emplace_back(reduction);
  }
  
  // SLR algorithm
  std::stack<int> stateStack;
  int numSymbols = 1;
  stateStack.push(initialState);
  if (!input.empty()) {
    printInput(input, reduction);
    shiftInput(input, reduction, stateStack, transitions);
  }
  while (reduction.top() != startSymbol) {
    if (reduction.top() == "EOF") {
      int ruleNum = checkReduce(reductions, stateStack.top(), ".ACCEPT");
      reduceInput(cfg, input, reduction, std::to_string(ruleNum), stateStack);
      stateStack.push(checkNextState(transitions, stateStack.top(), reduction.top()));
      break;
    }
    while (checkReduce(reductions, stateStack.top(), input.front()) != -1) {
      int ruleNum = checkReduce(reductions, stateStack.top(), input.front());
      reduceInput(cfg, input, reduction, std::to_string(ruleNum), stateStack);
      stateStack.push(checkNextState(transitions, stateStack.top(), reduction.top()));
    }
    shiftInput(input, reduction, stateStack, transitions);
    numSymbols++;
    if (stateStack.top() == -1) {
      std::cerr << "ERROR at " << numSymbols << std::endl;
      return;
    }
  }
}