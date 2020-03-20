using CourseWork.LexicalAnalysis;

namespace CourseWork.DataStructures
{
    public class UserSegment
    {
        public bool Opened => OpenToken != null;
        public bool Closed => CloseToken != null;
        public string Name => Opened ? OpenToken.StringValue : CloseToken.StringValue;

        public Token OpenToken { get; internal set; }
        public Token CloseToken { get; internal set; }
        public int Index { get; internal set; }

        public UserSegment() { }

        public override string ToString()
        {
            return $"UserSegment \"{Name}\" opened near {OpenToken}";
        }
    }
}
