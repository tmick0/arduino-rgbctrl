#include "proto.h"

void proto_state_machine_init(struct proto_state_machine *psm) {
  psm->state = PROTO_STATE_INIT;
}

int proto_state_machine_ingest(struct proto_state_machine *psm,
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
    ((uint8_t *)&psm->size)[0] = byte;
    psm->state = PROTO_STATE_SIZEH;
    return 0;
  }
  case PROTO_STATE_SIZEH: {
    ((uint8_t *)&psm->size)[1] = byte;
    if (psm->size > 128) {
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
    psm->code[psm->offset++] = byte;
    if (psm->offset == psm->size) {
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
