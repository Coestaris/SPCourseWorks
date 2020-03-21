class ASMUserSegment:
    def __init__(self):
        self.close = None
        self.open = None

    def opened(self):
        return self.open is not None

    def closed(self):
        return self.close is not None

    def name(self):
        if self.opened():
            return self.open.string_value
        elif self.closed():
            return self.close.string_value
        return "---"

class ASMVariable:
    def __init__(self, directive, name, value):
        self.directive = directive
        self.name = name
        self.value = value