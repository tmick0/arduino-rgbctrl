#ifndef rgbvm_h_
#define rgbvm_h_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*rgbvm_rgb_output)(uint8_t, uint8_t, uint8_t, uint8_t);

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

  // color manipulation
  RGBVM_OP_HSV2RGB = 0x7,

  // branching
  RGBVM_OP_GOTO = 0x8,
  RGBVM_OP_BRNE = 0xa,
  RGBVM_OP_BREQ = 0xb
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

enum rgbvm_flag { RGBVM_FLAG_EMPTY = 0x0, RGBVM_FLAG_EQUAL = 0x1 };

enum rgbvm_status {
  RGBVM_STATUS_OK = 0,
  RGBVM_STATUS_ILL = 1 // illegal instruction
};

struct rgbvm_instruction {
  enum rgbvm_opcode opcode : 4;
};

struct rgbvm_arithmetic_instruction {
  enum rgbvm_opcode opcode : 4;
  enum rgbvm_reg dst : 4;
  enum rgbvm_reg src : 4;
  uint8_t padding : 4;
  uint8_t imm[];
};

struct rgbvm_output_instruction {
  enum rgbvm_opcode opcode : 4;
  enum rgbvm_reg srcr : 4;
  enum rgbvm_reg srcg : 4;
  enum rgbvm_reg srcb : 4;
  uint8_t output;
};

struct rgbvm_hsv2rgb_instruction {
  enum rgbvm_opcode opcode : 4;
  enum rgbvm_reg h_r : 4;
  enum rgbvm_reg s_g : 4;
  enum rgbvm_reg v_b : 4;
};

struct rgbvm_branch_instruction {
  enum rgbvm_opcode opcode : 4;
  uint16_t padding : 4;
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
  enum rgbvm_flag flag;
};

void rgbvm_state_init(struct rgbvm_state *vm, const uint16_t code_len);

enum rgbvm_status rgbvm_apply(rgbvm_rgb_output output, struct rgbvm_state *vm,
                              const struct rgbvm_instruction *inst);

#ifdef __cplusplus
}
#endif

#endif