#include "string.hpp"

String::String(size_t size, char character = '\0') {
  size_ = size;
  capacity_ = size_;
  string_ = new char[size_];
  memset(string_, character, size_);
}

String::String(const String& s) {
  size_ = s.size_;
  capacity_ = size_;
  string_ = new char[size_];
  memcpy(string_, s.string_, size_);
}

String::String(const char* s) {
  size_ = strlen(s);
  capacity_ = size_ + 1;
  string_ = new char[capacity_];
  memcpy(string_, s, capacity_);
}

String& String::operator=(String s) {
  Swap(s);
  return *this;
}

void String::Swap(String& s) {
  std::swap(size_, s.size_);
  std::swap(capacity_, s.capacity_);
  std::swap(s.string_, string_);
}

void String::Resize(size_t new_size, char character) {
  if (size_ > new_size) {
    size_ = new_size;
    memset(string_ + size_, '\0', (capacity_ - size_));
  } else if (size_ < new_size) {
    if (capacity_ > new_size) {
      memset(string_ + size_, '\0', (new_size - size_));
      size_ = new_size;
    } else {
      String temp(2 * new_size, character);
      memcpy(temp.string_, string_, size_);
      Swap(temp);
      capacity_ = 2 * new_size;
      size_ = new_size;
    }
  }
}

void String::Clear() {
  memset(string_, '\0', size_);
  size_ = 0;
}

void String::PushBack(char character) {
  if (capacity_ == size_) {
    Reserve(2 * capacity_ + 1);
  }
  string_[size_] = character;
  ++size_;
}

void String::PopBack() {
  if (!Empty()) {
    string_[size_ - 1] = '\0';
    --size_;
  }
}

void String::Reserve(size_t new_cap) {
  if (new_cap > capacity_) {
    String temp;
    temp.size_ = size_;
    temp.capacity_ = new_cap;
    temp.string_ = new char[new_cap];
    memcpy(temp.string_, string_, size_);
    Swap(temp);
  }
}

bool String::Empty() const { return size_ == 0; }
size_t String::Size() const { return size_; }
size_t String::Capacity() const { return capacity_; }

const char* String::Data() const { return string_; }
char* String::Data() { return string_; }

void String::ShrinkToFit() {
  if (capacity_ != size_) {
    String temp;
    temp.size_ = size_;
    temp.capacity_ = size_;
    temp.string_ = new char[size_];
    memcpy(temp.string_, string_, size_);
    Swap(temp);
  }
}

bool operator==(const String& s_1, const String& s_2) {
  return !(s_1 > s_2 || s_2 > s_1);
}
bool operator<(const String& s_1, const String& s_2) {
  size_t count = 0;
  while ((count != s_1.size_) && (count != s_2.size_) &&
         *(s_2.string_ + count) == *(s_1.string_ + count)) {
    ++count;
  }
  if (count == s_1.size_ && count == s_2.size_) {
    return false;
  }
  if (count == s_1.size_) {
    return true;
  }
  if (count == s_2.size_) {
    return false;
  }
  return s_2.string_[count] > s_1.string_[count];
}

bool operator>(const String& s_1, const String& s_2) { return (s_2 < s_1); }

bool operator!=(const String& s_1, const String& s_2) {
  return ((s_1 < s_2) || (s_1 > s_2));
}

bool operator<=(const String& s_1, const String& s_2) {
  return (s_1 < s_2) || (s_1 == s_2);
}

bool operator>=(const String& s_1, const String& s_2) {
  return (s_1 > s_2) || (s_1 == s_2);
}

char& String::operator[](size_t i) { return *(string_ + i); }

char String::operator[](size_t i) const { return *(string_ + i); }

char& String::Front() { return *string_; }

char String::Front() const { return *string_; }

char& String::Back() { return *(string_ + size_ - 1); }

char String::Back() const { return *(string_ + size_ - 1); }

String& String::operator+=(const String& s) {
  size_t prev_size = size_;
  Resize(s.size_ + size_);
  memcpy((string_ + prev_size), s.string_, s.size_);
  return *this;
}

String operator+(const String& left, const String& right) {
  String joint = left;
  joint += right;
  return joint;
}

String& String::operator*=(size_t n) {
  String temp;
  size_t iter = 0;
  while (iter < n) {
    temp += *this;
    ++iter;
  }
  Swap(temp);
  return *this;
}

String operator*(const String& str, size_t n) {
  String temp = str;
  temp *= n;
  return temp;
}

std::vector<String> String::Split(const String& delim) {
  std::vector<String> substring;
  String sub;
  size_t i = 0;
  while (size_ != 0 && i <= size_ - delim.size_) {
    String temp(delim.size_);
    memcpy(temp.string_, string_ + i, delim.size_);
    if (temp == delim) {
      substring.push_back(sub);
      sub.Clear();
      i += delim.size_;
    } else {
      String new_char(1);
      memcpy(new_char.string_, string_ + i, sizeof(char));
      sub += new_char;
      ++i;
    }
  }
  if (i != Size()) {
    String temp(Size() - i);
    memcpy(temp.string_, string_ + i, (Size() - i));
    if (temp != delim) {
      sub += temp;
    }
  }
  substring.push_back(sub);
  return substring;
}
std::ostream& operator<<(std::ostream& outstreaming, const String& str) {
  for (size_t i = 0; i < str.Size(); ++i) {
    outstreaming << str.Data()[i];
  }
  return outstreaming;
}

std::istream& operator>>(std::istream& instreaming, String& str) {
  str.Clear();
  char c = '0';
  while (instreaming.get(c) && c != '\n') {
    str.PushBack(c);
  }
  return instreaming;
}

String String::Join(const std::vector<String>& strings) const {
  String joint;
  if (strings.empty()) {
    return joint;
  }
  size_t iter = 0;
  while (iter < strings.size() - 1) {
    joint += strings[iter];
    joint += *this;
    ++iter;
  }
  joint += strings.back();
  return joint;
}

String::~String() { delete[] string_; }
