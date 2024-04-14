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

/* Checks that Network Protocols are followed. Protocols are checked separately for readability. */
bool Message::is_valid() const
{
  // If a request that takes no arguments has an incorrect number of arguments
  if      ((get_message_type() == MessageType::POP   || get_message_type() == MessageType::TOP || 
            get_message_type() == MessageType::BEGIN || get_message_type() == MessageType::COMMIT ||
            get_message_type() == MessageType::BYE) 
            && (m_args.size() != 0)) {
  
    return false;
  }

  // If a request that takes one argument has an incorrect number of arguments
  else if ((get_message_type() == MessageType::LOGIN || get_message_type() == MessageType::CREATE || 
            get_message_type() == MessageType::PUSH  || get_message_type() == MessageType::SET ||
            get_message_type() == MessageType::GET) 
            && (m_args.size() != 1)) {

    return false;
  }

  // If a request that takes two arguments has an incorrect number of arguments
  else if ((get_message_type() == MessageType::ADD || get_message_type() == MessageType::MUL || 
            get_message_type() == MessageType::SUB || get_message_type() == MessageType::DIV) 
            && (m_args.size() != 2)) {

    return false;
  }

  // If the argument is an identifier 
  else if (get_message_type() == MessageType::LOGIN || get_message_type() == MessageType::CREATE ||
           get_message_type() == MessageType::SET   || get_message_type() == MessageType::GET) {

    // and if the first character is not a letter
    if (!(m_args[0].at(0) >= 'A' && m_args[0].at(0) <= 'Z') || (m_args[0].at(0) >= 'a' && m_args[0].at(0) <= 'z')) {
      return false;
    }

    // or the rest of the identifier contains an invalid character
    int identifier_length = m_args[0].length();
    for (int i = 1; i < identifier_length; i++) {

      if (!(m_args[0].at(i) >= 'A' && m_args[0].at(i) <= 'Z') || (m_args[0].at(i) >= 'a' && m_args[0].at(i) <= 'z') ||
           (m_args[0].at(i) >= '0' && m_args[0].at(i) <= '9') || m_args[0].at(i) == '_') {

        return false;
      }
    }
            
  return true;
}
}
