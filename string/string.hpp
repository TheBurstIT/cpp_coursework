#pragma once
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

class String {
 public:
  String() = default;
  ~String();

  String& operator=(String s);
  String(size_t size, char character);
  String(const String& s);
  String(const char* s);

  void Swap(String& s);
  void Clear();

  void PushBack(char character);
  void PopBack();

  void Resize(size_t new_size, char character = '\0');
  void Reserve(size_t new_cap);
  void ShrinkToFit();

  size_t Size() const;
  size_t Capacity() const;
  const char* Data() const;
  char* Data();
  bool Empty() const;
  friend bool operator<(const String&, const String&);
  friend bool operator<=(const String&, const String&);
  friend bool operator>(const String&, const String&);
  friend bool operator>=(const String&, const String&);
  friend bool operator==(const String&, const String&);
  friend bool operator!=(const String&, const String&);
  char& operator[](size_t i);
  char operator[](size_t i) const;

  char& Front();
  char& Back();
  char Front() const;
  char Back() const;

  String& operator+=(const String& s);
  String& operator*=(size_t n);

  std::vector<String> Split(const String& delim = " ");
  String Join(const std::vector<String>& strings) const;

 private:
  size_t size_ = 0;
  size_t capacity_ = 0;
  char* string_ = nullptr;
};

std::ostream& operator<<(std::ostream& outstreaming, const String& str);
std::istream& operator>>(std::istream& instreaming, String& str);

String operator+(const String& left, const String& right);
String operator*(const String& str, size_t n);
