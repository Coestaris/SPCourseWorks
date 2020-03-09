from asmparser import get_tokens
TEST_FILE = "test.asm"

if __name__ == "__main__":
    source = ""
    with open(TEST_FILE, "r") as file:
        source = file.read()

    tokens = get_tokens(TEST_FILE, source)

    for token in tokens:
        print(token)