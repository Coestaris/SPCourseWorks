using System.Collections.Generic;

namespace CourseWork.Bytes
{
    public static class ModRMSIB
    {
        public const byte PrefixExp = 0x0F;

        public const byte ModMem      = 0b00;
        public const byte ModDisp8    = 0b01;
        public const byte ModDisp32   = 0b10;
        public const byte ModRegister = 0b11;

        public const byte RmSIB = 0b100;

        public static readonly Dictionary<string, byte> SegmentCodes = new Dictionary<string, byte>
        {
            { "cs", 0x2E },
            { "ss", 0x36 },
            { "ds", 0x3E },
            { "es", 0x26 },
            { "fs", 0x64 },
            { "gs", 0x65 }
        };

        public static readonly Dictionary<string, byte> RegisterCodes = new Dictionary<string, byte>
        {
            {"eax", 0b000},
            {"ecx", 0b001},
            {"edx", 0b010},
            {"ebx", 0b011},
            {"esp", 0b100},
            {"ebp", 0b101},
            {"esi", 0b110},
            {"edi", 0b111},
            {"al", 0b000},
            {"cl", 0b001},
            {"dl", 0b010},
            {"bl", 0b011},
            {"ah", 0b100},
            {"ch", 0b101},
            {"dh", 0b110},
            {"bh", 0b111}
        };

        public static byte GetSIB(byte scale, byte index, byte @base)
        {
            scale &= 0b011;
            index &= 0b111;
            @base &= 0b111;
            return (byte) (scale << 6 | index << 3 | @base);
        }

        public static byte GetModRegRM(byte mod, byte reg, byte rm)
        {
            mod &= 0b011;
            reg &= 0b111;
            rm  &= 0b111;
            return (byte) (mod << 6 | reg << 3 | rm);
        }

        public static byte[] ToBytes8(int v)
        {
            return new[]
            {
                (byte) (v & 0xFF)
            };
        }

        public static byte[] ToBytes16(int v)
        {
            return new[]
            {
                (byte) ((v >> 8) & 0xFF),
                (byte) (v & 0xFF)
            };
        }

        public static byte[] ToBytes32(int v)
        {
            return new[]
            {
                (byte) ((v >> 24) & 0xFF),
                (byte) ((v >> 16) & 0xFF),
                (byte) ((v >> 8) & 0xFF),
                (byte) (v & 0xFF)
            };
        }
    }
}