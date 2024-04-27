#ifndef VALUE_STACK_H
#define VALUE_STACK_H

#include <vector>
#include <stack>
#include <string>

class ValueStack {
private:
  std::stack<std::string> stack;

public:
  ValueStack();
  ~ValueStack();

  bool is_empty() const;
  void push( const std::string &value );

  std::string get_top() const;
  void pop();

  int get_size();
};

#endif // VALUE_STACK_H
