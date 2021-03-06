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
        
class address(int):
    signedness = 'u'
    width = 16

class padding(int):
    signedness = 'u'
    def __init__(self, length):
        self.width = length
    def __new__(cls, length):
        return int.__new__(cls, 0)

LABELS = {}

def decode_operand(s, ignore_labels=False):
    if s[0] == 'r':
        return register(int(s[1:]))
    elif s[0] == ':':
        if ignore_labels:
            return address(0)
        return address(LABELS[s[1:]])
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
    
    def bytes(self, verbose=False):
        parts = [self.opcode] + list(self.args)

        bytes_out = []
        byte_len = 0
        byte_assembly = []
        for p in parts:
            byte_len += p.width
            byte_assembly.append(p)
            if byte_len % 8 == 0:
                byte_assembly = byte_assembly[::-1]
                fmt = ''.join('>{:s}{:d}'.format(p.signedness, p.width) for p in byte_assembly) + '<'
                bytes_out.append(bitstruct.pack(fmt, *byte_assembly))
                byte_assembly = []
                byte_len = 0

        if len(byte_assembly):
            raise RuntimeError("got an incomplete byte")

        res = b''.join(bytes_out)
        if verbose:
            print("assembling {:s} ({:02x}) {} -> {}".format(str(self.__class__.__name__), self.opcode, self.args, res.hex()))
        return res

@instruction("nop")
class instruction_nop (instruction_base):
    opcode = opcode(0x0)
    def __init__(self, delay=immediate(0)):
        delay.width = 4
        super().__init__(delay)

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

@instruction("cmp")
class instruction_cmp (instruction_arith_base):
    opcode = opcode(0x9)

@instruction("write")
class instruction_write (instruction_base):
    opcode = opcode(0x6)
    def __init__(self, r, g, b, o):
        super().__init__(r, g, b, o)

@instruction("init")
class instruction_init (instruction_base):
    opcode = opcode(0xc)
    def __init__(self, driver, channel, arg=immediate(0)):
        arg.width = 6
        driver.width = 4
        channel.width = 2
        super().__init__(driver, channel, arg)

@instruction("send")
class instruction_send (instruction_base):
    opcode = opcode(0xd)
    def __init__(self, channel):
        channel.width = 2
        super().__init__(channel, padding(2))

@instruction("input")
class instruction_input (instruction_base):
    opcode = opcode(0xe)
    def __init__(self, reg, pin):
        pin.width = 4
        super().__init__(reg, pin, padding(4))

@instruction("hsv2rgb")
class instruction_hsv2rgb (instruction_base):
    opcode = opcode(0x7)
    def __init__(self, r, g, b):
        super().__init__(r, g, b)

class branch_instruction_base (instruction_base):
    opcode = opcode(0x8)
    def __init__(self, addr):
        super().__init__(self.mode, addr)

@instruction("goto")
class instruction_goto (branch_instruction_base):
    mode = register(0x0)

@instruction("breq")
class instruction_breq (branch_instruction_base):
    mode = opcode(0x1)

@instruction("brne")
class instruction_brne (branch_instruction_base):
    mode = register(0x2)

@instruction("brlt")
class instruction_brlt (branch_instruction_base):
    mode = register(0x3)

@instruction("brle")
class instruction_brle (branch_instruction_base):
    mode = register(0x4)

@instruction("brgt")
class instruction_brgt (branch_instruction_base):
    mode = register(0x5)

@instruction("brge")
class instruction_brge (branch_instruction_base):
    mode = register(0x6)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input")
    parser.add_argument("output")
    parser.add_argument('--verbose', '-v', action='store_true', default=False)

    args = parser.parse_args()

    # first pass: calculate label addresses
    with open(args.input, 'r') as fhi:
        ip = 0
        for i, line in enumerate(fhi):
            try:
                line = line.strip()
                if len(line) == 0 or line[0] == '#':
                    continue
                if line[0] == ':':
                    LABELS[line[1:]] = ip
                    continue
                parts = line.split(' ')
                instr = INSTRUCTIONS[parts[0]]
                opers = [decode_operand(s, True) for s in parts[1:]]
                v = instr(*opers)
                ip += len(v.bytes())
            except:
                print("failed on line {:d}: {}".format(i, line))
                raise

    # second pass: generate code
    with open(args.input, 'r') as fhi, open(args.output, 'wb') as fho:
        ip = 0
        for i, line in enumerate(fhi):
            try:
                line = line.strip()
                if len(line) == 0 or line[0] == '#' or line[0] == ':':
                    continue
                parts = line.split(' ')
                instr = INSTRUCTIONS[parts[0]]
                opers = [decode_operand(s) for s in parts[1:]]
                v = instr(*opers)
                fho.write(v.bytes(args.verbose))
            except:
                print("failed on line {:d}: {}".format(i, line))
                raise


if __name__ == "__main__":
    main()
