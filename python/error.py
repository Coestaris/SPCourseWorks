from typing import Optional

class Error:
    def __init__(self, message: str, token: Optional['ASMToken']):
        self.message: str = message
        self.token: Optional['ASMToken'] = token

    def __str__(self):
        if self.token is None:
            return "Error message \"{}\"".format(self.message)

        return "Error message \"{}\" near {}".format(self.message, self.token)
