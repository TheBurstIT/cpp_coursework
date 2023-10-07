#pragma once

#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "BracketToken.hpp"
#include "OperandToken.hpp"
#include "OperatorToken.hpp"
#include "iostream"

template <typename T>
class ExprInPolishNotation {
 public:
  ExprInPolishNotation(const std::string& expr) {
    std::stack<char> stack;
    std::stringstream string_stream(expr);
    // auto leak_check = new OperandToken<T>("5");
    T t_token;
    char char_token;
    bool was_operand = false;
    while (!string_stream.eof()) {
      auto offset = string_stream.tellg();
      if (string_stream >> t_token) {
        if (was_operand) {
          SafetyThrowExpr();
        }
        tokens_.push_back(new OperandToken<T>(t_token));
        was_operand = true;
      } else {
        string_stream.clear();
        string_stream.seekg(offset);
      }
      if (string_stream >> char_token) {
        if ('0' <= char_token && char_token <= '9') {
          SafetyThrowExpr();
        }
        if (char_token == '+' || char_token == '-' || char_token == '*' ||
            char_token == '/') {
          was_operand = false;
          if ((char_token == '+' || char_token == '-') &&
              (string_stream.tellg() == 1 ||
               (string_stream.tellg() != 1 &&
                !PrevIsOperand(string_stream.tellg(), expr)))) {
            char_token == '+' ? char_token = '#' : char_token = '~';
          }
          while (!stack.empty() && kOperationPriority.at(stack.top()) >=
                                       kOperationPriority.at(char_token)) {
            tokens_.push_back(CreateOperatorToken(stack.top()));
            stack.pop();
          }
          stack.push(char_token);
        } else if (char_token == '(') {
          stack.push(char_token);

        } else if (char_token == ')') {
          while (!stack.empty() && stack.top() != '(') {
            tokens_.push_back(CreateOperatorToken(stack.top()));
            stack.pop();
          }
          if (!stack.empty() && stack.top() == '(') {
            stack.pop();
          } else {
            SafetyThrowExpr();
          }
        }
      }
    }
    while (!stack.empty()) {
      tokens_.push_back(CreateOperatorToken(stack.top()));
      stack.pop();
    }
  }
  ~ExprInPolishNotation() {
    for (auto& token : tokens_) {
      delete token;
    }
  }
  const std::vector<AbstractToken*>& GetTokens() { return tokens_; }

 private:
  void SafetyThrowExpr() {
    for (size_t i = 0; i < tokens_.size(); ++i) {
      delete tokens_[i];
    }
    throw std::runtime_error("Polish!");
  }
  struct InvalidConfig : std::exception {
    [[nodiscard]] const char* what() const noexcept override {
      return "Invalid bracket config!";
    }
  };
  bool PrevIsOperand(size_t index, const std::string& expr) {
    index = index - 2;
    for (; expr[index] == ' '; --index) {
    }
    return kOperationPriority.find(expr[index]) == kOperationPriority.end();
  }

  const std::unordered_map<char, int> kOperationPriority = {
      {'(', 0}, {'+', 1}, {'-', 1}, {'*', 2}, {'/', 2}, {'#', 3}, {'~', 3}};

  bool IsBinaryFunc(char token) const { return token != '#' && token != '~'; }

  char ToBinary(char token) {
    if (token == '#') {
      return '+';
    }
    if (token == '~') {
      return '-';
    }
    return token;
  }

  AbstractToken* CreateOperatorToken(char token) {
    std::string token_str;
    token_str += ToBinary(token);
    if (IsBinaryFunc(token)) {
      return new OperatorToken<T, true>(token_str);
    }
    return new OperatorToken<T, false>(token_str);
  }
  std::vector<AbstractToken*> tokens_;
};