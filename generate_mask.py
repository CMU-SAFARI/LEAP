import sys

def generate_sse():
    for i in range (0, 128):
        for j in range (0, i/8):
            sys.stdout.write("0xff, ")

        if (i%8 < 4):
            sys.stdout.write("0x0")
        else:
            sys.stdout.write("0x")

        lastbyte = 0
        for j in range (0, i%4):
           lastbyte = (lastbyte << 1) + 1
        sys.stdout.write(str(lastbyte) )
        
        if (i%8 >= 4):
            sys.stdout.write("f, ")
        else:
            sys.stdout.write(", ")

        for j in range (i/8 + 1, 16):
            sys.stdout.write("0x00, ")

        print ""

def generate_avx():
    for i in range (0, 256):
        for j in range (0, i/8):
            sys.stdout.write("0xff, ")

        if (i%8 < 4):
            sys.stdout.write("0x0")
        else:
            sys.stdout.write("0x")

        lastbyte = 0
        for j in range (0, i%4):
           lastbyte = (lastbyte << 1) + 1
        sys.stdout.write(str(lastbyte) )
        
        if (i%8 >= 4):
            sys.stdout.write("f, ")
        else:
            sys.stdout.write(", ")

        for j in range (i/8 + 1, 32):
            sys.stdout.write("0x00, ")

        print ""

generate_avx()
