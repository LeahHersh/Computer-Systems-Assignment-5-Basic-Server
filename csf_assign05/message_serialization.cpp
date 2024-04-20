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
  default: throw InvalidMessage("Message must have a type.");
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
  check_message_size(encoded_msg_);

  if (encoded_msg_.back() != '\n') {
    throw InvalidMessage("Encoded message is missing a newline.");
  }

  // Get the string containing the encoded message's type
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
  } else {
    throw InvalidMessage("Message must have a type.");
  }

  // Clear msg's arguments
  msg.clear_args();

  // Convert the rest of the stringstream to a string, and make sure it ends with a newline
  std::string remaining_text;
  std::getline(ss, remaining_text);

  // If the rest of the string is a quoted text, push the text as the Message's only argument
  MessageType msg_type = msg.get_message_type();
  if (msg_type == MessageType::FAILED || msg_type == MessageType::ERROR) {
    extract_quoted_text_arg(msg, remaining_text);
  }
  // Otherwise, set the decoded message's arguments word by word
  else {
    extract_single_word_args(msg, remaining_text);
  }

  // If the resulting Message is not valid
  if (!msg.is_valid()) {
    throw InvalidMessage("Message is not valid.");
  }
}


void MessageSerialization::check_message_size(std::string msg) {

  if (msg.length() > Message::MAX_ENCODED_LEN) {
    throw InvalidMessage("Message is too long.");
  }
}


void MessageSerialization::extract_quoted_text_arg(Message msg, std::string quoted_text) {

    size_t open_quote_ind = quoted_text.find_first_of('"');
    size_t close_quote_ind = quoted_text.find_last_of('"');

    // If none of the text is surrounded by quotations
    if (open_quote_ind == std::string::npos || close_quote_ind == std::string::npos) {
      throw InvalidMessage("Message whose type requires a quoted text argument does not contain a quotation.");
    }

    quoted_text = quoted_text.substr(open_quote_ind + 1, close_quote_ind - open_quote_ind - 1);
    msg.push_arg(quoted_text);
}


void MessageSerialization::extract_single_word_args(Message msg, std::string args_list) {
  
  std::stringstream ss(args_list);
  std::string curr_arg;

  while(ss >> curr_arg) {
    msg.push_arg(curr_arg);
  }
}


