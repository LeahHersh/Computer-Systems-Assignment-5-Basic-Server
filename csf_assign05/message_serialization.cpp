#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message_serialization.h"

void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  MessageType m_type = msg.get_message_type();
  
  // Add the message's type to the encoded message
  switch (m_type) {

  case MessageType::BEGIN: encoded_msg = "BEGIN\n";
    break;
  case MessageType::COMMIT: encoded_msg = "COMMIT\n";
    break;
  case MessageType::POP: encoded_msg = "POP\n";
    break;
  case MessageType::TOP: encoded_msg = "TOP\n";
    break;
  case MessageType::ADD: encoded_msg = "ADD\n";
    break;
  case MessageType::SUB: encoded_msg = "SUB\n";
    break;
  case MessageType::MUL: encoded_msg = "MUL\n";
    break;
  case MessageType::DIV: encoded_msg = "DIV\n";
    break;
  case MessageType::BYE: encoded_msg = "BYE\n";
    break;
  case MessageType::OK: encoded_msg = "OK\n";
    break;
  case MessageType::FAILED: encoded_msg = "FAILED\n";
    break;
  case MessageType::ERROR: encoded_msg = "ERROR\n";
    break;
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
  case MessageType::DATA: encoded_msg = "DATA";
    break;
  }

  // Add message arguments to the encoded message
  int num_args = msg.get_num_args();
  for (int i = 0; i < num_args; i++) {
    encoded_msg += " ";
    encoded_msg += msg.get_arg(i);
  }

  // Add newline character
  encoded_msg += "\n";

  // Check if the message is too long
  check_message_size(encoded_msg);
}

void check_message_size(std::string encoded_msg) {

  if (encoded_msg.length() > Message::MAX_ENCODED_LEN) {
    throw InvalidMessage("Message is too long.");
  }
}

void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{
  // TODO: implement
}
