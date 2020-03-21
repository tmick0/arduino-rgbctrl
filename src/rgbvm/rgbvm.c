#include "rgbvm.h"

typedef void (*rgbvm_arith_op_impl)(struct rgbvm_state *, unsigned char *,
                                    unsigned char);

void rgbvm_state_init(struct rgbvm_state *vm, unsigned int code_len);

unsigned char *rgbvm_decode_reg(struct rgbvm_state *vm, enum rgbvm_reg reg);

int rgbvm_decode_val(struct rgbvm_state *vm, const enum rgbvm_reg reg,
                     const struct rgbvm_arithmetic_instruction *inst,
                     unsigned char *dest, unsigned char *imm_flag);

void rgbvm_set_impl(struct rgbvm_state *vm, unsigned char *dest,
                    unsigned char src);
void rgbvm_add_impl(struct rgbvm_state *vm, unsigned char *dest,
                    unsigned char src);
void rgbvm_mul_impl(struct rgbvm_state *vm, unsigned char *dest,
                    unsigned char src);
void rgbvm_div_impl(struct rgbvm_state *vm, unsigned char *dest,
                    unsigned char src);
void rgbvm_mod_impl(struct rgbvm_state *vm, unsigned char *dest,
                    unsigned char src);

int rgbvm_decode_arithmetic(struct rgbvm_state *vm,
                            const struct rgbvm_arithmetic_instruction *inst,
                            unsigned char **dest, unsigned char *src,
                            rgbvm_arith_op_impl *op, unsigned char *size);

void rgbvm_state_init(struct rgbvm_state *vm, unsigned int code_len) {
  vm->ip = 0;
  vm->ip_max = code_len;
  for (char i = 0; i < RGBVM_REG_IM; ++i) {
    vm->reg[i] = 0;
  }
}

unsigned char *rgbvm_decode_reg(struct rgbvm_state *vm, enum rgbvm_reg reg) {
  switch (reg) {
  case RGBVM_REG_R0:
  case RGBVM_REG_R1:
  case RGBVM_REG_R2:
  case RGBVM_REG_R3:
  case RGBVM_REG_R4:
  case RGBVM_REG_R5:
  case RGBVM_REG_R6:
  case RGBVM_REG_R7:
  case RGBVM_REG_R8:
  case RGBVM_REG_R9:
  case RGBVM_REG_Ra:
  case RGBVM_REG_Rb:
  case RGBVM_REG_Rc:
  case RGBVM_REG_Rd:
  case RGBVM_REG_Re:
    return &vm->reg[reg];
  default:
    return 0;
  }
}

int rgbvm_decode_val(struct rgbvm_state *vm, const enum rgbvm_reg reg,
                     const struct rgbvm_arithmetic_instruction *inst,
                     unsigned char *dest, unsigned char *imm_flag) {
  if (reg == RGBVM_REG_IM) {
    *dest = inst->imm[0];
    *imm_flag = 1;
    return 0;
  }

  *imm_flag = 0;

  const unsigned char *src = rgbvm_decode_reg(vm, reg);
  if (src == 0) {
    return 1;
  }

  *dest = *src;
  return 0;
}

void rgbvm_set_impl(struct rgbvm_state *vm, unsigned char *dest,
                    unsigned char src) {
  *dest = src;
}

void rgbvm_add_impl(struct rgbvm_state *vm, unsigned char *dest,
                    unsigned char src) {
  *dest += src;
}

void rgbvm_mul_impl(struct rgbvm_state *vm, unsigned char *dest,
                    unsigned char src) {
  *dest *= src;
}

void rgbvm_div_impl(struct rgbvm_state *vm, unsigned char *dest,
                    unsigned char src) {
  *dest /= src;
}

void rgbvm_mod_impl(struct rgbvm_state *vm, unsigned char *dest,
                    unsigned char src) {
  *dest %= src;
}

int rgbvm_decode_arithmetic(struct rgbvm_state *vm,
                            const struct rgbvm_arithmetic_instruction *inst,
                            unsigned char **dest, unsigned char *src,
                            rgbvm_arith_op_impl *op, unsigned char *size) {

  switch (inst->opcode) {
  case RGBVM_OP_SET:
    *op = &rgbvm_set_impl;
    break;
  case RGBVM_OP_ADD:
    *op = &rgbvm_add_impl;
    break;
  case RGBVM_OP_MUL:
    *op = &rgbvm_mul_impl;
    break;
  case RGBVM_OP_DIV:
    *op = &rgbvm_div_impl;
    break;
  case RGBVM_OP_MOD:
    *op = &rgbvm_mod_impl;
    break;
  default:
    return 1;
  }

  *dest = rgbvm_decode_reg(vm, inst->dst);
  if (*dest == 0) {
    return 1;
  }

  if (rgbvm_decode_val(vm, inst->src, inst, src, size)) {
    return 1;
  }

  *size += 2;

  return 0;
}

void rgbvm_increment_ip(struct rgbvm_state *vm, const unsigned int inst_len) {
  vm->ip += inst_len;
  if (vm->ip >= vm->ip_max) {
    vm->ip = 0;
  }
}

void rgbvm_hsv2rgb_impl(unsigned char *h_r, unsigned char *s_g,
                        unsigned char *v_b) {

  // if saturation is 0, copy value into all channels
  if (*s_g == 0) {
    *h_r = *v_b;
    *s_g = *v_b;
    return;
  }

  const unsigned char region = *h_r / 43;
  const unsigned char remainder = (*h_r - (region * 43)) * 6;

  // TODO: are these automatically upgraded to 16 bit ints on avr? does the
  // arithmetic work?
  const unsigned char p = (*v_b * (255 - *s_g)) >> 8;
  const unsigned char q = (*v_b * (255 - ((*s_g * remainder) >> 8))) >> 8;
  const unsigned char t =
      (*v_b * (255 - ((*s_g * (255 - remainder)) >> 8))) >> 8;

  switch (region) {
  case 0:
    *h_r = *v_b;
    *s_g = t;
    *v_b = p;
    break;
  case 1:
    *s_g = *v_b;
    *h_r = q;
    *v_b = p;
    break;
  case 2:
    *s_g = *v_b;
    *h_r = p;
    *v_b = t;
    break;
  case 3:
    *h_r = p;
    *s_g = q;
    break;
  case 4:
    *h_r = t;
    *s_g = p;
    break;
  default:
    *h_r = *v_b;
    *s_g = p;
    *v_b = q;
    break;
  }
}

enum rgbvm_status rgbvm_apply(rgbvm_rgb_output output, struct rgbvm_state *vm,
                              const struct rgbvm_instruction *inst) {
  switch (inst->opcode) {
  case RGBVM_OP_NOP:
    rgbvm_increment_ip(vm, 1);
    return RGBVM_STATUS_OK;
  case RGBVM_OP_SET:
  case RGBVM_OP_ADD:
  case RGBVM_OP_MUL:
  case RGBVM_OP_DIV:
  case RGBVM_OP_MOD: {
    unsigned char *dest;
    unsigned char src;
    unsigned char size;
    rgbvm_arith_op_impl op;
    if (rgbvm_decode_arithmetic(vm, (struct rgbvm_arithmetic_instruction *)inst,
                                &dest, &src, &op, &size)) {
      return RGBVM_STATUS_ILL;
    }
    op(vm, dest, src);
    rgbvm_increment_ip(vm, size);
    return RGBVM_STATUS_OK;
  }
  case RGBVM_OP_WRITE: {
    const struct rgbvm_output_instruction *i =
        (const struct rgbvm_output_instruction *)inst;
    const unsigned char *r = rgbvm_decode_reg(vm, i->srcr);
    const unsigned char *g = rgbvm_decode_reg(vm, i->srcg);
    const unsigned char *b = rgbvm_decode_reg(vm, i->srcb);
    if (r == 0 || g == 0 || b == 0) {
      return RGBVM_STATUS_ILL;
    }
    output(*r, *g, *b, i->output);
    rgbvm_increment_ip(vm, 3);
    return RGBVM_STATUS_OK;
  }
  case RGBVM_OP_HSV2RGB: {
    const struct rgbvm_hsv2rgb_instruction *i =
        (const struct rgbvm_hsv2rgb_instruction *)inst;
    unsigned char *h_r = rgbvm_decode_reg(vm, i->h_r);
    unsigned char *s_g = rgbvm_decode_reg(vm, i->s_g);
    unsigned char *v_b = rgbvm_decode_reg(vm, i->v_b);
    if (h_r == 0 || s_g == 0 || v_b == 0) {
      return RGBVM_STATUS_ILL;
    }
    rgbvm_hsv2rgb_impl(h_r, s_g, v_b);
    rgbvm_increment_ip(vm, 2);
    return RGBVM_STATUS_OK;
  }
  default:
    return RGBVM_STATUS_ILL;
  }
}
