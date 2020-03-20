namespace CourseWork.Bytes
{
    public class ModRM
    {
        public const byte MOD_DEF      = 0b00;
        public const byte MOD_DISP8    = 0b01;
        public const byte MOD_DISP32   = 0b10;
        public const byte MOD_REGISTER = 0b11;

        public const byte RM_EAX = 0b000;
        public const byte RM_ECX = 0b001;
        public const byte RM_EDX = 0b010;
        public const byte RM_EBX = 0b011;
        public const byte RM_SIB = 0b100;
        public const byte RM_ESI = 0b001;
        public const byte RM_EDI = 0b001;

        public const byte REG_EAX = 0b000;
        public const byte REG_ECX = 0b001;
        public const byte REG_EDX = 0b010;
        public const byte REG_EBX = 0b011;
        public const byte REG_ESP = 0b100;
        public const byte REG_EBP = 0b101;
        public const byte REG_ESI = 0b110;
        public const byte REG_EDI = 0b111;

        public const byte REG_AL = 0b000;
        public const byte REG_CL = 0b001;
        public const byte REG_DL = 0b010;
        public const byte REG_BL = 0b011;
        public const byte REG_AH = 0b100;
        public const byte REG_CH = 0b101;
        public const byte REG_DH = 0b110;
        public const byte REG_BH = 0b111;

        public static byte GetByte(byte mod, byte reg, byte rm)
        {
            mod &= 0b011;
            reg &= 0b111;
            rm  &= 0b111;
            return (byte) (mod << 6 | reg << 3 | rm);
        }
    }
}