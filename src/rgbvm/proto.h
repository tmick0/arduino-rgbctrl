#ifndef proto_h_
#define proto_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

enum proto_msg {
  PROTO_MSG_HELLO = 0x11,
  PROTO_MSG_OK = 0x22,
  PROTO_MSG_ERR = 0x33
};

enum proto_state {
  PROTO_STATE_INIT,
  PROTO_STATE_SIZEH,
  PROTO_STATE_SIZEL,
  PROTO_STATE_CODE
};

struct proto_state_machine {
  enum proto_state state;
  uint16_t size;
  uint16_t offset;
  uint8_t code[128];
};

void proto_state_machine_init(struct proto_state_machine *psm);

typedef void (*proto_callback)(const struct proto_state_machine *);

int proto_state_machine_ingest(struct proto_state_machine *psm,
                               const uint8_t byte, enum proto_msg *res,
                               proto_callback cb);

#ifdef __cplusplus
}
#endif

#endif