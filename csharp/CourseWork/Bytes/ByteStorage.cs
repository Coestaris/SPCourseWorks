using System;
using System.Collections.Generic;
using System.Text;
using CourseWork.LexicalAnalysis;

namespace CourseWork.Bytes
{
    public class ByteStorage
    {
        public List<byte> Prefixes { get; private set; }
        public byte? Opcode { get; private set; }
        public byte? ModRM { get; private set; }
        public byte? SIB { get; private set; }
        public byte[] Disp { get; private set; }
        public byte[] IMM { get; private set; }

        public ByteStorage()
        {
            Prefixes = new List<byte>();
            Opcode = null;
            ModRM = null;
            SIB = null;
            Disp = null;
            IMM = null;
        }

        public void SetExpPrefix()
        {
            Prefixes.Add(ModRMSIB.PrefixExp);
        }

        public void SetSegmentPrefix(string segment)
        {
            Prefixes.Add(ModRMSIB.SegmentCodes[segment]);
        }

        public void SetSegmentPrefix(Token segment)
        {
            SetSegmentPrefix(segment.StringValue);
        }

        public void SetOpcode(byte opcode)
        {
            Opcode = opcode;
        }

        public void PackRegister(Token regiser)
        {
            Opcode |= ModRMSIB.RegisterCodes[regiser.StringValue];
        }

        public void SetReg(byte constant)
        {
            if (!ModRM.HasValue)
                ModRM = 0;
            ModRM |= ModRMSIB.GetModRegRM(0, constant, 0);
        }

        public void SetReg(Token reg)
        {
            if (!ModRM.HasValue)
                ModRM = 0;
            ModRM |= ModRMSIB.GetModRegRM(0, ModRMSIB.RegisterCodes[reg.StringValue], 0);
        }

        public void SetRM(Token reg)
        {
            if (!ModRM.HasValue)
                ModRM = 0;
            ModRM |= ModRMSIB.GetModRegRM(ModRMSIB.ModRegister, 0,
                ModRMSIB.RegisterCodes[reg.StringValue]);
        }

        public static bool CheckRM(Token index, Token @base)
        {
            if (index.StringValue == "ESP")
                return false;
            if (@base.StringValue == "EBP")
                return false;
            return true;
        }

        public void SetRM(Token index, Token @base, int disp)
        {
            if (!ModRM.HasValue) ModRM = 0;
            if (!SIB.HasValue) SIB = 0;

            ModRM |= ModRMSIB.GetModRegRM(ModRMSIB.ModDisp32, 0, ModRMSIB.RmSIB);
            SIB |= ModRMSIB.GetSIB(0,
                ModRMSIB.RegisterCodes[index.StringValue],
                ModRMSIB.RegisterCodes[@base.StringValue]);
            Disp = ModRMSIB.ToBytes32(disp);
        }

        public void SetDisp(byte[] disp)
        {
            Disp = disp;
        }

        public void SetIMM(byte[] imm)
        {
            IMM = imm;
        }

        private string Pad(byte b)
        {
            return b.ToString("X").PadLeft(2, '0');
        }

        public override string ToString()
        {
            var sb = new StringBuilder();

            if(Prefixes != null)
                foreach (var prefix in Prefixes)
                    sb.AppendFormat("{0}: ", Pad(prefix));

            if (Opcode.HasValue)
                sb.AppendFormat("{0}| ", Pad(Opcode.Value));

            if (ModRM.HasValue && SIB.HasValue)
                sb.AppendFormat("{0}{1} ", Pad(ModRM.Value), Pad(SIB.Value));
            else if (ModRM.HasValue)
                sb.AppendFormat("{0} ", Pad(ModRM.Value));

            if (Disp != null)
            {
                foreach (var @byte in Disp)
                    sb.AppendFormat("{0}", Pad(@byte));
                sb.Append(" ");
            }

            if(IMM != null)
            {
                foreach (var @byte in IMM)
                    sb.AppendFormat("{0}", Pad(@byte));
                sb.Append(" ");
            }

            return sb.ToString();
        }
    }
}