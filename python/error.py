class Error:
    def __init__(self, message, token):
        self.message = message
        self.token = token

    def __str__(self):
        return "Error message \"{}\" near {}".format(self.message, self.token)