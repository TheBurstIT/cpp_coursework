#pragma once
#include <sstream>

#include "AbstractToken.hpp"

template <typename T>
class OperandToken : public AbstractToken {
 public:
  OperandToken(const std::string& str);
  OperandToken(const T& value);
  const T& GetValue() const;

 private:
  T value_;
};

template <typename T>
OperandToken<T>::OperandToken(const std::string& str) : AbstractToken(str) {
  std::istringstream iss(str);
  iss >> value_;
}

template <typename T>
OperandToken<T>::OperandToken(const T& value)
    : AbstractToken(std::to_string(value)), value_(value) {}

template <typename T>
const T& OperandToken<T>::GetValue() const {
  return value_;
}