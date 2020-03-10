class ASMUserSegment:
    def __init__(self):
        self.close = None
        self.open = None

    def opened(self):
        return self.open != None

    def closed(self):
        return self.close != None

    def name(self):
        if self.opened(): return self.open.string_value
        elif self.closed(): return self.close.string_value
        return "---"