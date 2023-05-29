#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
const std::string ALPHABET    = ".ALPHABET";
const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::string EMPTY       = ".EMPTY";

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {
  std::istream& in = std::cin;
  std::string s;

  //// store properties
  std::vector<char> alphabet;
  std::string start;
  std::vector<std::string> accept;
  std::map<std::map<char, std::string>, std::string> dfa;

  std::getline(in, s); // Alphabet section (skip header)
  // Read characters or ranges separated by whitespace
  while(in >> s) {
    if (s == STATES) { 
      break; 
    } else {
      if (isChar(s)) {
        //// Variable 's[0]' is an alphabet symbol
        alphabet.emplace_back(s[0]);
      } else if (isRange(s)) {
        for(char c = s[0]; c <= s[2]; ++c) {
          //// Variable 'c' is an alphabet symbol
          alphabet.emplace_back(c);
        }
      } 
    }
  }

  std::getline(in, s); // States section (skip header)
  // Read states separated by whitespace
  while(in >> s) {
    if (s == TRANSITIONS) { 
      break; 
    } else {
      static bool initial = true;
      bool accepting = false;
      if (s.back() == '!' && !isChar(s)) {
        accepting = true;
        s.pop_back();
      }
      //// Variable 's' contains the name of a state
      if (initial) {
        //// The state is initial
        start = s;
        initial = false;
      }
      if (accepting) {
        //// The state is accepting
        accept.emplace_back(s);
      }
    }
  }

  std::getline(in, s); // Transitions section (skip header)
  // Read transitions line-by-line
  while(std::getline(in, s)) {
    if (s == INPUT) { 
      // Note: Since we're reading line by line, once we encounter the
      // input header, we will already be on the line after the header
      break; 
    } else {
      std::string fromState, symbols, toState;
      std::istringstream line(s);
      line >> fromState;
      bool last;
      while(line >> s) {
        if(line.peek() == EOF) { // If we reached the last item on the line
          // Then it's the to-state
          toState = s;
        } else { // Otherwise, there is more stuff on the line
          // We expect a character or range
          if (isChar(s)) {
            symbols += s;
          } else if (isRange(s)) {
            for(char c = s[0]; c <= s[2]; ++c) {
              symbols += c;
            }
          }
        }
      }
      for ( char c : symbols ) {
        //// There is a transition from 'fromState' to 'toState' on 'c'
        std::map<char, std::string> transition;
        transition[c] = fromState;
        dfa[transition] = toState;
      }
    }
  }

  // Input section (already skipped header)
  while(in >> s) {
    //// Variable 's' contains an input string for the DFA
    std::string curr = start;
    bool noRoute = false;
    if (s == ".EMPTY") {
      if (std::find(accept.begin(), accept.end(), curr) != accept.end() && !noRoute) {
        std::cout << s << " true" << std::endl;
      } else {
        std::cout << s << " false" << std::endl;
      }
      continue;
    }
    for (char& c : s) {
      std::map<char, std::string> transition;
      transition[c] = curr;
      auto it = dfa.find(transition);
      if (it == dfa.end()) {
        noRoute = true;
        break;
      } else {
        curr = it->second;
      }
    }
    if ((std::find(accept.begin(), accept.end(), curr) != accept.end()) && !noRoute) {
      std::cout << s << " true" << std::endl;
    } else {
      std::cout << s << " false" << std::endl; 
    }   
  }
}
