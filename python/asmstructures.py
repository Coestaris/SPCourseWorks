from typing import Optional

from asmtoken import ASMToken


class ASMUserSegment:
    def __init__(self):
        self.close: Optional[ASMToken] = None
        self.open: Optional[ASMToken] = None
        self.size: int = 0

    def opened(self) -> bool:
        return self.open is not None

    def closed(self) -> bool:
        return self.close is not None

    def name(self) -> str:
        if self.opened():
            return self.open.string_value
        elif self.closed():
            return self.close.string_value
        return "---"


class ASMVariable:
    def __init__(self, directive: ASMToken, name: ASMToken, value: ASMToken):
        self.directive: ASMToken = directive
        self.name: ASMToken = name
        self.value: ASMToken = value
