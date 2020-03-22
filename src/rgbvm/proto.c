#include "proto.h"

void proto_state_machine_init(struct proto_state_machine *psm) {
  psm->state = PROTO_STATE_INIT;
}

int proto_state_machine_ingest(struct rgbvm_state *vm, uint8_t *code,
                               struct proto_state_machine *psm,
                               const uint8_t byte, enum proto_msg *res,
                               proto_callback cb) {
  switch (psm->state) {
  case PROTO_STATE_INIT: {
    if (byte == PROTO_MSG_HELLO) {
      psm->state = PROTO_STATE_SIZEL;
      *res = PROTO_MSG_OK;
      return 1;
    }
    *res = PROTO_MSG_ERR;
    return 1;
  }
  case PROTO_STATE_SIZEL: {
    ((uint8_t *)&vm->ip_max)[0] = byte;
    psm->state = PROTO_STATE_SIZEH;
    return 0;
  }
  case PROTO_STATE_SIZEH: {
    ((uint8_t *)&vm->ip_max)[1] = byte;
    if (vm->ip_max > 128) {
      psm->state = PROTO_STATE_INIT;
      *res = PROTO_MSG_ERR;
      return 1;
    }
    psm->state = PROTO_STATE_CODE;
    psm->offset = 0;
    *res = PROTO_MSG_OK;
    return 1;
  }
  case PROTO_STATE_CODE: {
    code[psm->offset++] = byte;
    if (psm->offset == vm->ip_max) {
      cb(psm);
      psm->state = PROTO_STATE_INIT;
      *res = PROTO_MSG_OK;
      return 1;
    }
    return 0;
  }
  default: {
    psm->state = PROTO_STATE_INIT;
    *res = PROTO_MSG_ERR;
    return 1;
  }
  }
}
