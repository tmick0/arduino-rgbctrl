import serial
import argparse
import os.path
import struct
import time

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("bin")
    parser.add_argument("port")
    parser.add_argument("--baud", "-b", default=9600)
    args = parser.parse_args()

    port = serial.Serial(port=args.port, baudrate=args.baud, timeout=1, write_timeout=1)

    time.sleep(1)

    for i in range(100):
        port.write(b'\x11')
        if port.read(1) == b'\x22':
            break
    else:
        raise RuntimeError("device did not respond to hello")

    size = os.path.getsize(args.bin)
    sizeb = struct.pack('<Bx', size)
    port.write(sizeb)

    b = port.read(1)
    if not b == b'\x22':
        raise RuntimeError("device reported an error on size: {:s}".format(str(b)))

    with open(args.bin, 'rb') as fh:
        port.write(fh.read())

    if not port.read() == b'\x22':
        raise RuntimeError("device reported an error on code")

    print("successfully flashed {:d} bytes".format(size))

if __name__ == "__main__":
    main()
