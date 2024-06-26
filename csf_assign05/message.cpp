#include <set>
#include <map>
#include <regex>
#include <cassert>
#include <cctype>
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
  m_message_type = rhs.get_message_type();
  
  // Set up new m_args array that will hold rhs's args
  int num_args_rhs = rhs.get_num_args();
  std::vector<std::string> new_args;
  new_args.resize(num_args_rhs);

  // Set rhs's args to "this'" args
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
  return m_args[0];
}

std::string Message::get_table() const
{
  return m_args[0];
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

/* Checks that Network Protocols are followed. Protocols are checked separately for readability. */
bool Message::is_valid() const
{
  MessageType msg_type = get_message_type();

  // If a request that takes no arguments has an incorrect number of arguments
  if      ((msg_type == MessageType::POP || msg_type == MessageType::TOP || msg_type == MessageType::ADD   || msg_type == MessageType::MUL || 
            msg_type == MessageType::SUB || msg_type == MessageType::DIV || msg_type == MessageType::BEGIN || msg_type == MessageType::COMMIT ||
            msg_type == MessageType::BYE) 
            && (m_args.size() != 0)) {
  
    return false;
  }

  // If a request that takes one argument has an incorrect number of arguments
  else if ((msg_type == MessageType::LOGIN || msg_type == MessageType::CREATE || msg_type == MessageType::PUSH || msg_type == MessageType::DATA) 
            && (m_args.size() != 1)) {

    return false;
  }

  // If a request that takes two arguments has an incorrect number of arguments
  else if ((msg_type == MessageType::SET || msg_type == MessageType::GET) && (m_args.size() != 2)) {

    return false;
  }

  // If the first argument is an identifier 
  else if (msg_type == MessageType::LOGIN || msg_type == MessageType::CREATE ||
           msg_type == MessageType::SET   || msg_type == MessageType::GET) {

    if (!is_valid_identifier(m_args[0])) { return false; }

    // And there are two arguments, with the second being an identifier
    if (msg_type == MessageType::SET || msg_type == MessageType::GET) {
      if (!is_valid_identifier(m_args[1])) { return false; }
    }
  }

  // If the argument is a value
  else if (msg_type == MessageType::PUSH) {

    // and only contains white space
    if (std::all_of(m_args[0].begin(),m_args[0].end(),isspace)) { return false; }
  }

  // Otherwise
  return true;
}


bool Message::is_valid_identifier(std::string arg) const {
  // and the first character is not a letter
    if (!((arg.at(0) >= 'A' && arg.at(0) <= 'Z') || (arg.at(0) >= 'a' && arg.at(0) <= 'z'))) {
      return false;
    }

    // or the rest of the identifier contains an invalid character
    int identifier_length = arg.length();
    for (int i = 1; i < identifier_length; i++) {

      if (!((arg.at(i) >= 'A' && arg.at(i) <= 'Z') || (arg.at(i) >= 'a' && arg.at(i) <= 'z') ||
            (arg.at(i) >= '0' && arg.at(i) <= '9') ||  arg.at(i) == '_')) {

        return false;
      }
    }

    return true;
}
