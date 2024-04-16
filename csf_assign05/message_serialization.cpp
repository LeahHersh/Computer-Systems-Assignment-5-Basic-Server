#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message_serialization.h"

void check_message_size(std::string encoded_msg) {

  if (encoded_msg.length() > Message::MAX_ENCODED_LEN) {
    throw InvalidMessage("Message is too long.");
  }
}

void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  MessageType m_type = msg.get_message_type();
  
  // Add the message's type to the encoded message
  switch (m_type) {

  case MessageType::BEGIN: encoded_msg = "BEGIN";
    break;
  case MessageType::COMMIT: encoded_msg = "COMMIT";
    break;
  case MessageType::POP: encoded_msg = "POP";
    break;
  case MessageType::TOP: encoded_msg = "TOP";
    break;
  case MessageType::ADD: encoded_msg = "ADD";
    break;
  case MessageType::SUB: encoded_msg = "SUB";
    break;
  case MessageType::MUL: encoded_msg = "MUL";
    break;
  case MessageType::DIV: encoded_msg = "DIV";
    break;
  case MessageType::BYE: encoded_msg = "BYE";
    break;
  case MessageType::OK: encoded_msg = "OK";
    break;
  case MessageType::FAILED: encoded_msg = "FAILED";
    break;
  case MessageType::ERROR: encoded_msg = "ERROR";
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

void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{
  
  std::stringstream ss(encoded_msg_);
  std::string m_type; 
  ss >> m_type;

  // Add the message's type to the decoded message
  if (m_type == "BEGIN") {
    msg.set_message_type(MessageType::BEGIN);
  }
  else if (m_type == "COMMIT") {
    msg.set_message_type(MessageType::COMMIT);
  }
  else if (m_type == "POP") {
    msg.set_message_type(MessageType::POP);
  }
  else if (m_type == "TOP") {
    msg.set_message_type(MessageType::TOP);
  }
  else if (m_type == "ADD") {
    msg.set_message_type(MessageType::ADD);
  }
  else if (m_type == "SUB") {
    msg.set_message_type(MessageType::SUB);
  }
  else if (m_type == "MUL") {
    msg.set_message_type(MessageType::MUL);
  }
  else if (m_type == "DIV") {
    msg.set_message_type(MessageType::DIV);
  }
  else if (m_type == "BYE") {
    msg.set_message_type(MessageType::BYE);
  }
  else if (m_type == "OK") {
    msg.set_message_type(MessageType::OK);
  }
  else if (m_type == "FAILED") {
    msg.set_message_type(MessageType::FAILED);
  }
  else if (m_type == "ERROR") {
    msg.set_message_type(MessageType::ERROR);
  }
  else if (m_type == "LOGIN") {
    msg.set_message_type(MessageType::LOGIN);
  }
  else if (m_type == "CREATE") {
    msg.set_message_type(MessageType::CREATE);
  }
  else if (m_type == "PUSH") {
    msg.set_message_type(MessageType::PUSH);
  }
  else if (m_type == "SET") {
    msg.set_message_type(MessageType::SET);
  }
  else if (m_type == "GET") {
    msg.set_message_type(MessageType::GET);
  }
  else if (m_type == "DATA") {
    msg.set_message_type(MessageType::DATA);
  }

  // Clear msg's arguments
  msg.clear_args();

  // If the rest of the string is a quoted text, push it as the only argument
  if (msg.get_message_type() == MessageType::FAILED || msg.get_message_type() == MessageType::ERROR) {
    
    std::string use_up_type;
    ss >> use_up_type;

    std::string quoted_text = ss.str();

    msg.push_arg(quoted_text);

    return;
  }

  // Set the decoded message's arguments
  std::string curr_arg;
  while(ss >> curr_arg) {
    msg.push_arg(curr_arg);
  }
}
