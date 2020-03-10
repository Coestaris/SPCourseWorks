using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace CourseWork
{
    public class AssemblySource
    {
        public string Source { get; }

        internal string FileName;

        public AssemblySource(Stream stream)
        {
            if(stream == null)
                throw new ArgumentNullException(nameof(stream));

            if(stream.CanRead == false)
                throw new ArgumentException("Passed stream doesn't support reading");

            if(stream is FileStream)
                FileName = (stream as FileStream).Name;

            long length = -1;
            try { length = stream.Length; }
            catch { } // Unable to get length from stream :(

            if (length != -1)
            {
                var buffer = new byte[length];
                if(stream.Read(buffer, 0, (int)length) != length)
                    throw new IOException();

                Source = new string(buffer.Select(p => (char)p).ToArray());
            }
            else
            {
                var chars = new List<byte>();
                var buffer = new byte[100];
                var end = false;

                while (!end)
                {
                    if (stream.Read(buffer, 0, (int) length) < length)
                        end = true;

                    chars.AddRange(buffer);
                }

                Source = new string(chars.Select(p => (char)p).ToArray());
            }
        }
    }
}