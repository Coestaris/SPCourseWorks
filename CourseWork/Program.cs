using System;
using System.IO;
using CourseWork.LexicalAnalysis;

namespace CourseWork
{
    internal class Program
    {
        public static void Main(string[] args)
        {
            AssemblySource source = null;
            using (var stream = File.OpenRead("test.asm"))
                source = new AssemblySource(stream);

            Console.WriteLine(source.Source);
            var lexemes = Tokenizer.Tokenize(source.Source, "test.asm");

            foreach (var lexeme in lexemes)
                Console.WriteLine(lexeme);
        }
    }
}