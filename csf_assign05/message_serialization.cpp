#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message_serialization.h"

void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  MessageType m_type = msg.get_message_type();
  
  switch (m_type) {

  case MessageType::BEGIN: encoded_msg = "BEGIN\n";
    return;
  case MessageType::COMMIT: encoded_msg = "COMMIT\n";
    return;
  case MessageType::POP: encoded_msg = "POP\n";
    return;
  case MessageType::TOP: encoded_msg = "TOP\n";
    return;
  case MessageType::ADD: encoded_msg = "ADD\n";
    return;
  case MessageType::SUB: encoded_msg = "SUB\n";
    return;
  case MessageType::MUL: encoded_msg = "MUL\n";
    return;
  case MessageType::DIV: encoded_msg = "DIV\n";
    return;
  case MessageType::BYE: encoded_msg = "BYE\n";
    return;

  case MessageType::LOGIN: encoded_msg = "LOGIN";
    break;
  case MessageType::CREATE: encoded_msg = "CREATE";
    break;
  case MessageType::PUSH: encoded_msg = "PUSH";
    break;
  case MessageType::SET: encoded_msg = "SET";
    break;
  case MessageType::GET: encoded_msg = "GET";
    break;
  }

  int num_args = msg.get_num_args();
  for (int i = 0; i < num_args; i++) {
    encoded_msg += " ";
    encoded_msg += msg.get_arg(i);
  }
}

void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{
  // TODO: implement
}
