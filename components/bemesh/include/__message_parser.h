/*
 * __message_parser.h
 * Handles message parsing
 * A MessageParser is an object which is able to parse an input message formed by
 * at most MSG_PARSER_BUF_SIZE bytes (uint8_t).
 * During the parse phase, the user may execute the feed method in order to feed
 * the parser with a new byte.
 * Following, a set of parsing function (msg_parser_fn) will perfom integrity
 * checks on the incoming message.
 * In case errors occur, it is possible to reset the Parser buffer, which is also
 * automatically executed after each succesful parsed message.
 * With the installCallback method, the user can install an automatic callback
 * function, which gets executed when the a message is correctly parsed.
 */
#pragma once

#include "__status.h"
#include <vector>
#include <tuple>
#include <map>

namespace bemesh {

  #define MSG_PARSER_BUF_SIZE 255

  typedef void (*msg_callback_fn)(void*);
  typedef ErrStatus (*msg_parser_fn)(uint8_t);
    
  struct MessageParser {
    // buffer index
    uint8_t m_buf_idx;
    // input buffer
    uint8_t m_in_buf[MSG_PARSER_BUF_SIZE];

    // on parsing completition event
    msg_callback_fn m_on_recv_fn;
    void* m_on_recv_args;

    // function that handles parsing
    msg_parser_fn m_curr_parse_fn;

    MessageParser();
    ErrStatus feed(uint8_t t_char);
    ErrStatus installCallback(msg_callback_fn t_fn, void* t_args);
    ErrStatus removeCallback(void);
    ErrStatus reset(void);

  };
}
