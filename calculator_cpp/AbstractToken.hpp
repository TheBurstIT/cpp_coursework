#pragma once

#include <iostream>
#include <stack>

class AbstractToken {
 public:
  AbstractToken(const std::string& str) : str_(str) {}
  virtual ~AbstractToken() = default;

  const std::string& GetStringToken() const { return str_; }

 private:
  std::string str_;
};