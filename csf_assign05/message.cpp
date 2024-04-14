#include <set>
#include <map>
#include <regex>
#include <cassert>
#include "message.h"

Message::Message()
  : m_message_type(MessageType::NONE)
{
}

Message::Message( MessageType message_type, std::initializer_list<std::string> args )
  : m_message_type( message_type )
  , m_args( args )
{
}

Message::Message( const Message &other )
  : m_message_type( other.m_message_type )
  , m_args( other.m_args )
{
}

Message::~Message()
{
}

Message &Message::operator=( const Message &rhs )
{
  // Set up new m_args array that will hold rhs's args
  int num_args_rhs = rhs.get_num_args();
  std::vector<std::string> new_args;
  new_args.resize(num_args_rhs);

  // Set rhs's args to "this'" args
  m_message_type = rhs.get_message_type();
  for (int i = 0; i < num_args_rhs; i++) {
    new_args[i] = rhs.get_arg(i);
  }
  m_args = new_args;

  return *this;
}

MessageType Message::get_message_type() const
{
  return m_message_type;
}

void Message::set_message_type(MessageType message_type)
{
  m_message_type = message_type;
}

std::string Message::get_username() const
{
   /* if (!(get_message_type() == MessageType::LOGIN && is_valid())) {
    return "";
  }  */

  return m_args[0]; //TODO: correct?
}

std::string Message::get_table() const
{
  return m_args[0]; //TODO: correct?
}

std::string Message::get_key() const
{
  return m_args[1];
}

std::string Message::get_value() const
{
  return m_args[0];
}

std::string Message::get_quoted_text() const
{
  return m_args[0];
}

void Message::push_arg( const std::string &arg )
{
  m_args.push_back( arg );
}

bool Message::is_valid() const
{
  if (get_message_type() == MessageType::LOGIN && )
  return true;
}
