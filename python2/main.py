import asmtokenize

INPUT_FILE = "test.asm"
# Set None for automatic name
OUTPUT_FILE = None

if __name__ == '__main__':
    if OUTPUT_FILE is None:
        OUTPUT_FILE = ".".join(INPUT_FILE.split('.')[:-1]) + ".log"

    asmtokenize.tokenize(INPUT_FILE, OUTPUT_FILE, True, False, False)
