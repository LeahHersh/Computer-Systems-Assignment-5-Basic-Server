#ifndef MESSAGE_SERIALIZATION_H
#define MESSAGE_SERIALIZATION_H

#include "message.h"

namespace MessageSerialization {

  void encode(const Message &msg, std::string &encoded_msg);

  void decode(const std::string &encoded_msg, Message &msg);

  /* 
   * Throws an InvalidMessage exception if an encoded Message exceeds MAX_ENCODED_LEN. 
   * @param msg The encoded message.
   * @throws InvalidMessage exception.
  */
  void check_message_size(std::string msg);

  /* 
  * Finds the text between quotation marks and pushes it to a Message's arguments.
  * @param msg The encoded message.
  * @param quoted_text The text between the quotation marks.
  * @throws InvalidMessage if the encoded Message doesn't include a quotation.
  */
  void extract_quoted_text_arg(Message msg, std::string quoted_text);

  /* 
  * Takes a stringstream of arguments and pushes each word in the stringstream as a Message's argument.
  * @param msg The encoded message.
  * @param ss The stringstream of arguments.
  */
  void extract_single_word_args(Message msg, std::stringstream& ss);

};

#endif // MESSAGE_SERIALIZATION_H
