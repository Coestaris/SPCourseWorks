using System;
using System.IO;
using CourseWork.LexicalAnalysis;

namespace CourseWork
{
    internal class Program
    {
        private const string TestFile = "test.asm";

        private static void DoTest(string fileName)
        {
            AssemblySource source = null;
            using (var stream = File.OpenRead(fileName))
                source = new AssemblySource(stream);

            Console.WriteLine(source.Source);
            var assembly = new Assembly(source);

            Error error;
            if ((error = assembly.Parse()) != null)
            {
                Console.WriteLine(error);
                return;
            }

            Console.WriteLine("Labels: ");
            foreach (var label in assembly.UserLabels)
                Console.WriteLine(label);
            Console.WriteLine("====");

            Console.WriteLine("UserSegments: ");
            foreach (var userSegment in assembly.UserSegments)
                Console.WriteLine(userSegment);
            Console.WriteLine("====");

            Console.WriteLine("Tokens: ");
            foreach (var lexeme in assembly.Lexemes)
                Console.WriteLine(lexeme);
            Console.WriteLine("====");
        }

        public static void Main(string[] args)
        {
            DoTest(TestFile);

            if(Environment.OSVersion.Platform != PlatformID.Unix)
                Console.ReadKey();
        }
    }
}