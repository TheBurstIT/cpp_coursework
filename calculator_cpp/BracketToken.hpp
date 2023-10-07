#pragma once
#include "AbstractToken.hpp"

class BracketToken : public AbstractToken {
 public:
  BracketToken(const std::string& str) : AbstractToken(str) {}

  bool IsOpening() const { return GetStringToken() == "("; }
};