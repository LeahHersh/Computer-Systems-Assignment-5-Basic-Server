#include "value_stack.h"
#include "exceptions.h"

ValueStack::ValueStack()
{
}

ValueStack::~ValueStack()
{
}

bool ValueStack::is_empty() const
{
  if (stack.empty()) {
    return true;
  }
  return false;
}

void ValueStack::push( const std::string &value )
{
  stack.push(value);
}

std::string ValueStack::get_top() const
{
  if (stack.empty()) {
    throw OperationException("Tried to call TOP on an empty operations stack.");
  }

  return stack.top();
}

void ValueStack::pop()
{
  if (stack.empty()) {
    throw OperationException("Tried to call POP on an empty operations stack.");
  }

  stack.pop();
}
