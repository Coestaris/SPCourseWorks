﻿using System;
using System.Data;
using System.IO;
using System.Linq;
using System.Xml.Schema;
using CourseWork.DataStructures;
using CourseWork.LexicalAnalysis;

namespace CourseWork
{
    internal class Program
    {
        private const string TestFile = "test.asm";
        private const string Output = "test.out";

        private static void WriteAsm(Assembly assembly, bool bytes)
        {
            var i = 0;
            var err = 0;

            if (!bytes)
            {
                Console.WriteLine(new string('=', 68));
                Console.WriteLine(" #|  Cnt  | Offset |                      Source                   |");
                Console.WriteLine(new string('=', 68));

                foreach (var lexeme in assembly.Lexemes)
                {
                    var indentation = "";
                    if (lexeme.Segment != null) indentation = "   ";
                    if (lexeme.Structure == null || !lexeme.Structure.HasName) indentation += "   ";

                    var s = string.Format("{3,2}|  {4,3}  |  {2} |  {0}{1}",
                        indentation,
                        lexeme.ToTable(true),
                        lexeme.HasOffset ? lexeme.Offset.ToString("X").PadLeft(5, '0') : "-----",
                        i++, lexeme.Size);
                    Console.WriteLine(s.PadRight(67, ' ') + "|");
                    if (lexeme.Error != null)
                    {
                        err++;
                        Console.WriteLine(lexeme.Error);
                    }
                }

                Console.WriteLine(new string('=', 68));
                Console.WriteLine("{0} errors\n", err);
            }
            else
            {
                Console.WriteLine(new string('=', 96));
                Console.WriteLine(
                    " #|  Cnt  | Offset |              Bytes            |                        Source                     |");
                Console.WriteLine(new string('=', 96));

                foreach (var lexeme in assembly.Lexemes)
                {
                    var indentation = "";
                    if (lexeme.Segment != null) indentation = "   ";
                    if (lexeme.Structure == null || !lexeme.Structure.HasName) indentation += "   ";

                    var s = string.Format("{3,2}|  {4,3}  |  {2} |  {5,-28} | {0}{1}",
                        indentation,
                        lexeme.ToTable(true),
                        lexeme.HasOffset ? lexeme.Offset.ToString("X").PadLeft(5, '0') : "-----",
                        i++, lexeme.Size,
                        lexeme.Storage == null ? "" : lexeme.Storage.ToString());

                    Console.WriteLine(s.PadRight(95, ' ') + "|");
                    if (lexeme.Error != null)
                    {
                        err++;
                        Console.WriteLine(lexeme.Error);
                    }
                }

                Console.WriteLine(new string('=', 96));
                Console.WriteLine("{0} errors\n", err);
            }
        }

        private static void WriteSegmentsTable(Assembly assembly)
        {
            Console.WriteLine(new string('=', 50));
            Console.WriteLine("# | Segment name | Bit depth  | Offset |  Size   |");
            Console.WriteLine(new string('=', 50));
            var i = 0;
            foreach (var segment in assembly.UserSegments)
                Console.WriteLine("{0} | {1,6}       | {2,6}     | {3,2}     |  {4,4:X}   |", i++ + 1, segment.Name, 32,
                    segment.OpenToken.Line, segment.Size);
            Console.WriteLine(new string('=', 50));
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

        private static void WriteUserNamesTable(Assembly assembly)
        {
            Console.WriteLine(new string('=', 48));
            Console.WriteLine("  # |    Name     |     Type         |  Value  |");
            Console.WriteLine(new string('=', 48));
            var i = 0;

            foreach (var segment in assembly.UserSegments)
                Console.WriteLine("{0,3} | {1,9}   |     {2,9}    |  {3,5}  |", i++,
                    segment.Name, "SEGMENT", "--");

            foreach (var variable in assembly.UserVariables)
                Console.WriteLine("{0,3} | {1,9}   |  {2,15} |  {3,5}  |", i++,
                    variable.Name.StringValue,
                    "DATA " + variable.Type.StringValue.ToUpper() +
                        " : " +
                        variable.Name.ParentLexeme.Segment.Name.ToUpper(),
                    variable.Name.ParentLexeme.Offset.ToString("X").PadLeft(4, '0'));

            foreach (var label in assembly.UserLabels)
                Console.WriteLine("{0,3} | {1,9}   |  {2,15} |  {3,5}  |", i++,
                    label.StringValue, "LABEL : " + label.ParentLexeme.Segment.Name.ToUpper(),
                    label.ParentLexeme.Offset.ToString("X").PadLeft(4, '0'));

            foreach (var equ in assembly.UserEqus)
            {
                var type = "EQU (";
                if (equ.EquTokens.Count == 1)
                    type += equ.EquTokens[0].Type + ")";
                else
                    type += "text)";

                var value = string.Join(" ", equ.EquTokens.Select(p => p.StringValue));

                Console.WriteLine("{0,3} | {1,9}   |  {2,15} |  {3,5}  |", i++,
                    equ.Name.StringValue, type,
                    value);
            }

            Console.WriteLine(new string('=', 48));
        }

        private static void WriteSecondStage(Assembly assembly)
        {
            Console.WriteLine("TOKENS: ");
            foreach (var lexeme in assembly.Lexemes)
            {
                Console.WriteLine("Source   | {0}", lexeme.ToTable(true));
                if (lexeme.Error != null)
                {
                    Console.WriteLine(lexeme.Error);
                    Console.WriteLine();
                    continue;
                }
                Console.WriteLine("Tokens   | {0}", lexeme.ToTable(false));
                Console.WriteLine("Sentence |{0}", lexeme.Structure.ToTable());
                Console.WriteLine();
            }
        }

        private static void WriteThirdStage(Assembly assembly)
        {
            Console.WriteLine("TOKENS: ");
            WriteAsm(assembly, false);

            Console.WriteLine();
            Console.WriteLine("LOGICAL SEGMENTS: ");
            WriteSegmentsTable(assembly);

            Console.WriteLine();
            Console.WriteLine("SEGMENT REGISTER DESTINATIONS: ");
            WriteSegRegAssignmentTable(assembly);

            Console.WriteLine();
            Console.WriteLine("USER DEFINED NAMES: ");
            WriteUserNamesTable(assembly);
        }

        private static void WriteFourthStage(Assembly assembly)
        {
            Console.WriteLine("TOKENS: ");
            WriteAsm(assembly, true);

            Console.WriteLine();
            Console.WriteLine("LOGICAL SEGMENTS: ");
            WriteSegmentsTable(assembly);

            Console.WriteLine();
            Console.WriteLine("SEGMENT REGISTER DESTINATIONS: ");
            WriteSegRegAssignmentTable(assembly);

            Console.WriteLine();
            Console.WriteLine("USER DEFINED NAMES: ");
            WriteUserNamesTable(assembly);
        }


        private static void DoTest(string inputFile, string outputFile)
        {
            if (!File.Exists(inputFile))
            {
                Console.WriteLine("Unable to find file {0}", inputFile);
                return;
            }

            AssemblySource source = null;
            using (var stream = File.OpenRead(inputFile))
                source = new AssemblySource(stream);

            var assembly = new Assembly(source);

            var oldOut = Console.Out;

            using (var writer = File.CreateText(outputFile))
            {
                Console.SetOut(writer);

                assembly.Parse();
                assembly.FirstPass();
                assembly.SecondPass();

                //WriteSecondStage(assembly);
                //WriteThirdStage(assembly);
                WriteFourthStage(assembly);
            }

            Console.SetOut(oldOut);
        }

        public static void Main(string[] args)
        {
            DoTest(TestFile, Output);

            if(Environment.OSVersion.Platform != PlatformID.Unix)
                Console.ReadKey();
        }
    }
}