// solution here
#pragma once

#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "PolishExpr.hpp"

template <typename T>
class Calculator {
 public:
  static T CalculateExpr(const std::string& expr) {
    std::stack<OperandToken<T>*> stack;
    try {
      ExprInPolishNotation<T> expr_in_polish_notation(expr);
      const std::vector<AbstractToken*> kTokens =
          expr_in_polish_notation.GetTokens();
      for (size_t i = 0; i < kTokens.size(); ++i) {
        if (OperandToken<T>* operand =
                dynamic_cast<OperandToken<T>*>(kTokens[i]);
            operand != nullptr) {
          stack.push(new OperandToken<T>(operand->GetValue()));
        } else if (OperatorToken<T, true>* binary =
                       dynamic_cast<OperatorToken<T, true>*>(kTokens[i]);
                   binary != nullptr) {
          OperandToken<T>* first_operand;
          OperandToken<T>* second_operand;
          if (stack.size() >= 2) {
            second_operand = stack.top();
            stack.pop();
            first_operand = stack.top();
            stack.pop();
          } else {
            SafetyThrowExpr(stack);
          }
          auto result = new OperandToken<T>(
              binary->Calculate(first_operand, second_operand));
          stack.push(result);
          delete first_operand;
          delete second_operand;
        } else if (OperatorToken<T, false>* unary =
                       dynamic_cast<OperatorToken<T, false>*>(kTokens[i]);
                   unary != nullptr) {
          OperandToken<T>* last_operand;
          if (!stack.empty()) {
            last_operand = stack.top();
            stack.pop();
          } else {
            SafetyThrowExpr(stack);
          }
          auto result = new OperandToken<T>(unary->Calculate(last_operand));
          stack.push(result);
          delete last_operand;
        }
      }
      if (stack.size() != 1) {
        SafetyThrowExpr(stack);
      }

      T result = stack.top()->GetValue();
      auto value = stack.top();
      delete stack.top();
      stack.pop();
      return result;
    } catch (const InvalidExpr& e) {
      std::cerr << e.what();
      SafetyThrowExpr(stack);
    }
    return 0;
  }

 private:
  static void SafetyThrowExpr(std::stack<OperandToken<T>*>& stack) {
    while (!stack.empty()) {
      delete stack.top();
      stack.pop();
    }
    throw std::runtime_error("Calculator!");
  }
  struct InvalidExpr : std::exception {
    [[nodiscard]] const char* what() const noexcept override {
      return "Invalid expression!";
    }
  };
};