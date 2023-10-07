#pragma once
#include <iostream>

#include "OperandToken.hpp"

template <typename T, bool IsBinary>
class OperatorToken : public AbstractToken {
 public:
  OperatorToken(const std::string& str) : AbstractToken(str) {}

  bool IsBinaryFunc() const { return true; }

  T Calculate(const OperandToken<T>* lhs, const OperandToken<T>* rhs) const {
    assert(lhs != nullptr && rhs != nullptr);
    T result = 0;
    if (this->GetStringToken() == "+") {
      result = lhs->GetValue() + rhs->GetValue();
    } else if (this->GetStringToken() == "-") {
      result = lhs->GetValue() - rhs->GetValue();
    } else if (this->GetStringToken() == "*") {
      result = lhs->GetValue() * rhs->GetValue();
    } else if (this->GetStringToken() == "/") {
      result = lhs->GetValue() / rhs->GetValue();
    } else {
      delete lhs;
      delete rhs;
      throw std::runtime_error("Invalid binary operator.");
    }
    return result;
  }
};

template <typename T>
class OperatorToken<T, false> : public AbstractToken {
 public:
  OperatorToken(const std::string& str) : AbstractToken(str) {}

  bool IsBinaryFunc() const { return false; }

  T Calculate(const OperandToken<T>* operand) const {
    assert(operand != nullptr);
    T result = 0;
    if (this->GetStringToken() == "+") {
      result = operand->GetValue();
    } else if (this->GetStringToken() == "-") {
      result = -operand->GetValue();
    } else {
      delete operand;
      throw std::runtime_error("Invalid unary operator.");
    }
    return result;
  }
};