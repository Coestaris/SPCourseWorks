class Error:
    def __init__(self, message, token):
        self.message = message
        self.token = token

    def __str__(self):
        if self.token is None:
            return "Error message \"{}\"".format(self.message)

        return "Error message \"{}\" near {}".format(self.message, self.token)