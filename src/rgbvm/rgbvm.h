#ifndef rgbvm_h_
#define rgbvm_h_

#include "driver.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*rgbvm_delay)(unsigned long);

enum rgbvm_opcode {
  // no op
  RGBVM_OP_NOP = 0x0,

  // arithmetic operations
  RGBVM_OP_SET = 0x1,
  RGBVM_OP_ADD = 0x2,
  RGBVM_OP_MUL = 0x3,
  RGBVM_OP_DIV = 0x4,
  RGBVM_OP_MOD = 0x5,
  RGBVM_OP_CMP = 0x9,

  // i/o operations
  RGBVM_OP_WRITE = 0x6,
  RGBVM_OP_INIT = 0xc,
  RGBVM_OP_SEND = 0xd,
  RGBVM_OP_INPUT = 0xe,
  RGBVM_OP_REMOTE = 0xa,

  // color manipulation
  RGBVM_OP_HSV2RGB = 0x7,

  // branching
  RGBVM_OP_BRANCH = 0x8
};

enum rgbvm_reg {
  RGBVM_REG_R0 = 0x0,
  RGBVM_REG_R1 = 0x1,
  RGBVM_REG_R2 = 0x2,
  RGBVM_REG_R3 = 0x3,
  RGBVM_REG_R4 = 0x4,
  RGBVM_REG_R5 = 0x5,
  RGBVM_REG_R6 = 0x6,
  RGBVM_REG_R7 = 0x7,
  RGBVM_REG_R8 = 0x8,
  RGBVM_REG_R9 = 0x9,
  RGBVM_REG_Ra = 0xa,
  RGBVM_REG_Rb = 0xb,
  RGBVM_REG_Rc = 0xc,
  RGBVM_REG_Rd = 0xd,
  RGBVM_REG_Re = 0xe,
  RGBVM_REG_IM =
      0xf // indicates source operand is an immediate in imm[0], not a register
};

enum rgbvm_flag {
  RGBVM_FLAG_EMPTY = 0x0,
  RGBVM_FLAG_EQUAL = 0x1,
  RGBVM_FLAG_LESS = 0x2
};

enum rgbvm_status {
  RGBVM_STATUS_OK = 0,
  RGBVM_STATUS_ILL = 1 // illegal instruction
};

enum rgbvm_branch {
  RGBVM_BRANCH_GOTO = 0x0,
  RGBVM_BRANCH_EQ = 0x1,
  RGBVM_BRANCH_NE = 0x2,
  RGBVM_BRANCH_LT = 0x3,
  RGBVM_BRANCH_LE = 0x4,
  RGBVM_BRANCH_GT = 0x5,
  RGBVM_BRANCH_GE = 0x6
};

struct rgbvm_instruction {
  enum rgbvm_opcode opcode : 4;
};

struct rgbvm_nop_instruction {
  enum rgbvm_opcode opcode : 4;
  uint8_t delay : 4;
};

struct rgbvm_arithmetic_instruction {
  enum rgbvm_opcode opcode : 4;
  enum rgbvm_reg dst : 4;
  enum rgbvm_reg src : 4;
  uint8_t padding : 4;
  uint8_t imm[];
};

struct rgbvm_write_instruction {
  enum rgbvm_opcode opcode : 4;
  enum rgbvm_reg srcr : 4;
  enum rgbvm_reg srcg : 4;
  enum rgbvm_reg srcb : 4;
  uint8_t channel : 2;
  uint8_t padding : 6;
};

struct rgbvm_init_instruction {
  enum rgbvm_opcode opcode : 4;
  enum driver_type driver : 4;
  uint8_t channel : 2;
  uint8_t arg : 6;
};

struct rgbvm_send_instruction {
  enum rgbvm_opcode : 4;
  uint8_t channel : 2;
  uint8_t padding : 2;
};

struct rgbvm_input_instruction {
  enum rgbvm_opcode : 4;
  enum rgbvm_reg dest : 4;
  uint8_t channel : 4;
  uint8_t padding : 4;
};

struct rgbvm_remote_instruction {
  enum rgbvm_opcode : 4;
  enum rgbvm_reg dest : 4;
  uint8_t channel : 4;
  uint8_t padding : 4;
};

struct rgbvm_hsv2rgb_instruction {
  enum rgbvm_opcode opcode : 4;
  enum rgbvm_reg h_r : 4;
  enum rgbvm_reg s_g : 4;
  enum rgbvm_reg v_b : 4;
};

struct rgbvm_branch_instruction {
  enum rgbvm_opcode opcode : 4;
  enum rgbvm_branch mode : 4;
  uint16_t dest;
};

struct rgbvm_state {
  // instruction pointer
  uint16_t ip;

  // length of code segment (after which ip will reset to 0)
  uint16_t ip_max;

  // 15 "general purpose" registers
  uint8_t reg[15];

  // flag register
  int flag;

  // output buffers
  struct rgbvm_driver outputs[4];

  // remote control register values
  uint8_t remote_values[9];
};

void rgbvm_state_init(struct rgbvm_state *vm, const uint16_t code_len);

enum rgbvm_status rgbvm_apply(rgbvm_delay delay, struct rgbvm_state *vm,
                              const struct rgbvm_instruction *inst);

#ifdef __cplusplus
}
#endif

#endif