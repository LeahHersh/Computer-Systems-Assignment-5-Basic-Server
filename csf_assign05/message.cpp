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
  // TODO: double check
  get_key() = rhs.get_key();
  get_value() = rhs.get_value();

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
/*   if (!(get_message_type() == MessageType::LOGIN && is_valid())) {
    throw InvalidMessage;
  } */

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
  // TODO: implement
  return "";
}

void Message::push_arg( const std::string &arg )
{
  m_args.push_back( arg );
}

bool Message::is_valid() const
{
  // TODO: implement
  return true;
}
