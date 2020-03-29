﻿using System;
using System.IO;
using System.Xml.Schema;
using CourseWork.DataStructures;
using CourseWork.LexicalAnalysis;

namespace CourseWork
{
    internal class Program
    {
        private const string TestFile = "test.asm";

        private static void WriteAsm(Assembly assembly)
        {
            var i = 0;
            Console.WriteLine(new string('=', 68));
            Console.WriteLine(" #|  Cnt  | Offset |                      Souce                    |");
            Console.WriteLine(new string('=', 68));

            foreach (var lexeme in assembly.Lexemes)
            {
                var s = string.Format("{4,2}|  {5,3}  |  {3} |  {0}{1}{2}",
                    lexeme.Segment == null ? "" : "   ",
                    lexeme.Structure.HasName ? "" : "   ",
                    lexeme.ToTable(true),
                    lexeme.HasOffset ? lexeme.Offset.ToString("X").PadLeft(5, '0') : "-----",
                    i++, lexeme.GetSize());
                Console.WriteLine(s.PadRight(67,' ') + "|");
            }
            Console.WriteLine(new string('=', 68));
        }

        private static void WriteSegmentsTable(Assembly assembly)
        {
            Console.WriteLine(new string('=', 40));
            Console.WriteLine("# | Segment name | Bit depth  | Offset |");
            Console.WriteLine(new string('=', 40));
            var i = 0;
            foreach (var segment in assembly.UserSegments)
                Console.WriteLine("{0} | {1,6}       | {2,6}     | {3,2}     |", i++ + 1, segment.Name, 32, segment.OpenToken.Line);
            Console.WriteLine(new string('=', 40));
        }

        private static void WriteSegRegAssignmentTable(Assembly assembly)
        {
            Console.WriteLine(new string('=', 36));
            Console.WriteLine("# | Segment register | Destination |");
            Console.WriteLine(new string('=', 36));
            Console.WriteLine("0 |      CS          |   {0,6}    |", assembly.UserSegments[1].Name);
            Console.WriteLine("1 |      DS          |   {0,6}    |", assembly.UserSegments[0].Name);
            Console.WriteLine("2 |      SS          |    NOTHING  |");
            Console.WriteLine("3 |      ES          |    NOTHING  |");
            Console.WriteLine("4 |      GS          |    NOTHING  |");
            Console.WriteLine("5 |      FS          |    NOTHING  |");
            Console.WriteLine(new string('=', 36));
        }

        private static void WriteEQUsTable(Assembly assembly)
        {
            Console.WriteLine(new string('=', 40));
            Console.WriteLine("# |   Equ name  |  Equ value  | Offset |");
            Console.WriteLine(new string('=', 40));
            var i = 0;
            foreach (var equ in assembly.Equs)
                Console.WriteLine("{0} | {1,6}      | {2,5}       | {3,2}     |", i++,
                    equ.Key, equ.Value.StringValue, equ.Value.Line);
            Console.WriteLine(new string('=', 40));
        }


        private static void WriteUserNamesTable(Assembly assembly)
        {
            Console.WriteLine(new string('=', 46));
            Console.WriteLine("# |    Name     |     Type         | Mem off |");
            Console.WriteLine(new string('=', 46));
            var i = 0;

            foreach (var segment in assembly.UserSegments)
                Console.WriteLine("{0} | {1,9}   |     {2,9}    |  {3,2}     |", i++,
                    segment.Name, "SEGMENT", "--");

            foreach (var variable in assembly.UserVariables)
                Console.WriteLine("{0} | {1,9}   |  {2,15} |  {3,2}     |", i++,
                    variable.Name.StringValue, "DATA " + variable.Type.StringValue.ToUpper() + " at " +
                                               variable.Name.ParentLexeme.Segment.Name.ToUpper(),
                    variable.Name.ParentLexeme.Offset);

            foreach (var label in assembly.UserLabels)
                Console.WriteLine("{0} | {1,9}   |     {2,9}    |  {3,2}     |", i++,
                    label.StringValue, "LABEL", "--");
            Console.WriteLine(new string('=', 46));
        }

        private static void DoTest(string fileName)
        {
            if (!File.Exists(fileName))
            {
                Console.WriteLine("Unable to find file {0}", fileName);
                return;
            }

            AssemblySource source = null;
            using (var stream = File.OpenRead(fileName))
                source = new AssemblySource(stream);

            var assembly = new Assembly(source);

            Error error;
            if ((error = assembly.Parse()) != null)
            {
                Console.WriteLine(error);
                return;
            }

            if ((error = assembly.FirstPass()) != null)
            {
                Console.WriteLine(error);
                return;
            }


            Console.WriteLine("TOKENS: ");
            WriteAsm(assembly);

            Console.WriteLine();
            Console.WriteLine("LOGICAL SEGMENTS: ");
            WriteSegmentsTable(assembly);

            Console.WriteLine();
            Console.WriteLine("SEGMENT REGISTER DESTINATIONS: ");
            WriteSegRegAssignmentTable(assembly);

            Console.WriteLine();
            Console.WriteLine("EQUs: ");
            WriteEQUsTable(assembly);

            Console.WriteLine();
            Console.WriteLine("USER DEFINED NAMES: ");
            WriteUserNamesTable(assembly);

        }

        public static void Main(string[] args)
        {
            DoTest(TestFile);

            if(Environment.OSVersion.Platform != PlatformID.Unix)
                Console.ReadKey();
        }
    }
}