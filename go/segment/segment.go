package segment

import (
	"course_work_2/tokens"
	"course_work_2/types"
)

type segment struct {
	open  types.Token
	close types.Token
	size  int
}

func (s *segment) GetSize() int {
	return s.size
}

func (s *segment) SetSize(value int) {
	s.size = value
}

func (s *segment) SetOpen(value types.Token) {
	s.open = value
}

func (s *segment) SetClose(value types.Token) {
	s.close = value
}

func ProceedSegments(a types.ASM) {
	var curSeg types.Segment

	for _, l := range a.GetLexemes() {
		ts := l.GetTokens()
			switch tsLen := len(ts); {
			case tsLen == 2 && ts[1].GetTokenType() == tokens.DATA:
				curSeg = a.GetDataSegment()
			case tsLen == 2 && ts[1].GetTokenType() == tokens.CODE:
				curSeg = a.GetCodeSegment()
			default:
				if curSeg != nil {
					l.SetSegment(curSeg)
				}
			}
	}
}

func NewSegment() types.Segment {
	return &segment{}
}

func (s *segment) GetOpen() types.Token{
	return s.open
}

func (s *segment) GetClose() types.Token {
	return s.close
}

func (s *segment) GetName() string {
	if s.open != nil {
		return s.open.ToString()
	} else if s.close != nil {
		return s.close.ToString()
	} else {
		return "---"
	}
}
