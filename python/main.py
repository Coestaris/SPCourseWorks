from asmparser import get_tokens
TEST_FILE = "test.asm"

if __name__ == "__main__":
    with open(TEST_FILE, "r") as file:
        source = file.read().lower()

    error, tokens = get_tokens(TEST_FILE, source)
    if error != None:
        print(error)
        exit(1)

    for token in tokens:
        print(str(token))