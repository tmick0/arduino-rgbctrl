import bitstruct
import argparse

class opcode(int):
    signedness = 'u'
    width = 4

class register(int):
    signedness = 'u'
    width = 4

class immediate(int):
    width = 8
    def __init__(self, val):
        self.signedness = 's' if val < 0 else 'u'
    def __new__(cls, val):
        return int.__new__(cls, val)
        
class padding(int):
    signedness = 'u'
    def __init__(self, length):
        self.width = length
    def __new__(cls, length):
        return int.__new__(cls, 0)

def decode_operand(s):
    if s[0] == 'r':
        return register(int(s[1:]))
    else:
        return immediate(int(s))

INSTRUCTIONS = {}

def instruction(mnemonic):
    def fn(impl):
        INSTRUCTIONS[mnemonic] = impl
        return fn
    return fn

class instruction_base (object):
    def __init__(self, *args):
        self.args = args
        print("assembling instruction: {} {}".format(self.opcode, self.args))
    
    def bytes(self):
        parts = [self.opcode] + list(self.args)

        bytes_out = []
        byte_len = 0
        byte_assembly = []
        for p in parts:
            byte_len += p.width
            byte_assembly.append(p)
            if byte_len % 8 == 0:
                fmt = ''.join('>{:s}{:d}'.format(p.signedness, p.width) for p in byte_assembly) + '<'
                bytes_out.append(bitstruct.pack(fmt, *byte_assembly[::-1]))
                byte_assembly = []
                byte_len = 0

        if len(byte_assembly):
            raise RuntimeError("got an incomplete byte")

        res = b''.join(bytes_out)
        print("args: {} -> {}".format(parts, res.hex()))
        return res

@instruction("nop")
class instruction_nop (instruction_base):
    opcode = opcode(0x0)
    def op(self):
        pass

class instruction_arith_base (instruction_base):
    def __init__(self, dst, src):
        if isinstance(src, immediate):
            super().__init__(dst, register(0xf), padding(4), src)
        else:
            super().__init__(dst, src, padding(4))

@instruction("set")
class instruction_set (instruction_arith_base):
    opcode = opcode(0x1)

@instruction("add")
class instruction_add (instruction_arith_base):
    opcode = opcode(0x2)

@instruction("mul")
class instruction_mul (instruction_arith_base):
    opcode = opcode(0x3)

@instruction("div")
class instruction_div (instruction_arith_base):
    opcode = opcode(0x4)

@instruction("mod")
class instruction_mod (instruction_arith_base):
    opcode = opcode(0x5)

@instruction("write")
class instruction_write (instruction_base):
    opcode = opcode(0x6)
    def __init__(self, r, g, b, o):
        super().__init__(r, g, b, o)

@instruction("hsv2rgb")
class instruction_hsv2rgb (instruction_base):
    opcode = opcode(0x7)
    def __init__(self, r, g, b):
        super().__init__(r, g, b)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input")
    parser.add_argument("output")

    args = parser.parse_args()

    with open(args.output, 'wb') as fho, open(args.input, 'r') as fhi:
        for i, line in enumerate(fhi):
            try:
                line = line.strip()

                if len(line) == 0 or line[0] == '#':
                    continue
                
                parts = line.split(' ')
                instr = INSTRUCTIONS[parts[0]]
                opers = [decode_operand(s) for s in parts[1:]]
                
                v = instr(*opers)
                fho.write(v.bytes())
            except:
                print("failed on line {:d}: {}".format(i, line))
                raise


if __name__ == "__main__":
    main()
