#if __linux__
	#ifdef __GNUC__
	#pragma implementation "coding.h"
	#endif
#else
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "coding.h"

#include <string.h>

#include "errors.h"

// Means that field is not defined 
#define EMPTY ((uint8_t)-1)

// REG mod field
#define MOD_REG 0b11
// SIB + DISP8 mod field
#define MOD_MEM_PLUS_DISP8  0b01
// SIB + DISP8 mod field
#define MOD_MEM_PLUS_DISP32 0b10
// SIB rm field
#define RM_SIB 0b100
// Expansion prefix constant
#define EXP_PREFIX 0x0F

// Pair of Name:Value to implement some kind of dictionary...
struct code_elem
{
   const char* name;
   uint8_t code;
}
// Segment change prefix codes
SEG_CODES[] =
{
   {"ES", 0x26 },
   {"CS", 0x2E },
   {"SS", 0x36 },
   {"DS", 0x3E },
   {"FS", 0x64 },
   {"GS", 0x65 },
};

// 8/32 Register codes
struct code_elem REG_CODES[] =
{
   {"EAX", 0b000 },
   {"ECX", 0b001 },
   {"EDX", 0b010 },
   {"EBX", 0b011 },
   {"ESP", 0b100 },
   {"EBP", 0b101 },
   {"ESI", 0b110 },
   {"EDI", 0b111 },
   {"AL",  0b000 },
   {"CL",  0b001 },
   {"DL",  0b010 },
   {"BL",  0b011 },
   {"AH",  0b100 },
   {"CH",  0b101 },
   {"DH",  0b110 },
   {"BH",  0b111 },
};

// Returns seg prefix code that corresponds to a given token
static uint8_t get_seg_code(token_t* t)
{
   for(size_t i = 0; i < 6; i++)
      if(!strcmp(t->string, SEG_CODES[i].name))
         return SEG_CODES[i].code;
   return EMPTY;
}

// Returns register code that corresponds to a given token
static uint8_t get_reg_code(token_t* t)
{
   for(size_t i = 0; i < 16; i++)
      if(!strcmp(t->string, REG_CODES[i].name))
         return REG_CODES[i].code;
   return EMPTY;
}

// Assembles modregrm byte from separate fields
static uint8_t get_modrm(uint8_t mod, uint8_t reg, uint8_t rm)
{
   return (mod & 0b11U) << 6U | (reg & 0b111U) << 3U | (rm & 0b111U);
}

// Assembles sib byte from separate fields
static uint8_t get_sib(uint8_t scale, uint8_t index, uint8_t base)
{
   return (scale & 0b11U) << 6U | (index & 0b111U) << 3U | (base & 0b111U);
}

// Useful function for joining strings
static void c_append_string(char* ptr, size_t len, const char* str)
{
   strncat(ptr, str, len);
}

// Useful function for joining padded converted numbers to a string
static void c_append_hex(char* ptr, size_t len, size_t value, int8_t value_len)
{
   char buff[30];
   snprintf(buff, sizeof(buff), "%zX", value);

   int32_t str_len = strlen(buff);

   memmove(buff + (value_len - str_len), buff, sizeof(buff) - (value_len - str_len) - 1);
   memset(buff, '0', value_len - str_len);

   strncat(ptr, buff, len);
}

//
// c_fill()
//
void c_fill(coding_t* coding)
{
   memset(coding, 0, sizeof(coding_t));
}

//
// c_string()
//
void c_string(char* ptr, size_t size, coding_t* coding)
{
   memset(ptr, 0, size);
   if(coding->has_prefixes)
   {
      if(coding->prefixes[0] != 0)
      {
         c_append_hex(ptr, size, coding->prefixes[0], 2);
         c_append_string(ptr, size, ": ");
      }
      if(coding->prefixes[1] != 0)
      {
         c_append_hex(ptr, size, coding->prefixes[1], 2);
         c_append_string(ptr, size, ": ");
      }
   }

   if(coding->has_opcode)
   {
      c_append_hex(ptr, size, coding->opcode, 2);
      c_append_string(ptr, size, "| ");
   }

   if(coding->has_modrm)
   {
      c_append_hex(ptr, size, coding->modrm, 2);
      c_append_string(ptr, size, " ");
   }

   if(coding->has_sib)
   {
      c_append_hex(ptr, size, coding->sib, 2);
      c_append_string(ptr, size, " ");
   }

   if(coding->disp_size != 0)
   {
      for(size_t i = 0; i < coding->disp_size; i++)
      {
         uint8_t byte = (coding->disp >> ((coding->disp_size - i - 1) * 8U)) & 0xFFU;
         c_append_hex(ptr, size, byte, 2);
      }
   }

   if(coding->imm_size != 0)
   {
      for(size_t i = 0; i < coding->imm_size; i++)
      {
         uint8_t byte = (coding->imm >> ((coding->imm_size - i - 1) * 8U)) & 0xFFU;
         c_append_hex(ptr, size, byte, 2);
      }
   }
}

//
// c_set_opcode()
//
bool c_set_opcode(coding_t* c, uint8_t opcode)
{
   e_assert(c, "Passed NULL argument");

   c->has_opcode = true;
   c->opcode = opcode;

   return false;
}

//
// c_set_prefix_exp()
//
bool c_set_prefix_exp(coding_t* c)
{
   e_assert(c, "Passed NULL argument");

   c->has_prefixes = true;
   c->prefixes[0] = EXP_PREFIX;

   return false;
}

//
// c_set_prefix_exp()
//
bool c_set_prefix_seg(coding_t* c, token_t* segment)
{
   e_assert(c, "Passed NULL argument");

   uint8_t seg_prefix = get_seg_code(segment);
   if(seg_prefix == EMPTY) return false;

   if(seg_prefix == 0x3E)
      return false;

   c->has_prefixes = true;
   c->prefixes[1] = seg_prefix;
   return false;
}

//
// c_set_reg()
//
bool c_set_reg(coding_t* c, token_t* reg)
{
   e_assert(c, "Passed NULL argument");

   uint8_t reg_code = get_reg_code(reg);
   if(reg_code == EMPTY) return false;

   c->has_modrm = true;
   c->modrm |= get_modrm(0, reg_code, 0);

   return false;
}

//
// c_set_reg_const()
//
bool c_set_reg_const(coding_t* c, uint8_t constant)
{
   e_assert(c, "Passed NULL argument");

   c->has_modrm = true;
   c->modrm |= get_modrm(0, constant, 0);

   return false;
}

//
// c_set_rm_reg()
//
bool c_set_rm_reg(coding_t* c, token_t* reg)
{
   e_assert(c, "Passed NULL argument");

   uint8_t reg_code = get_reg_code(reg);
   if(reg_code == EMPTY) return false;

   c->has_modrm = true;
   c->modrm |= get_modrm(MOD_REG, 0, reg_code);

   return false;
}

//
// c_set_rm_m()
//
bool c_set_rm_m(coding_t* c, token_t* reg1, token_t* reg2, token_t* scale, token_t* disp)
{
   e_assert(c, "Passed NULL argument");

   int64_t disp_val = t_num(disp);
   int64_t scale_val = t_num(scale);


   uint8_t base_code = get_reg_code(reg1);
   uint8_t index_code = get_reg_code(reg2);
   uint8_t scale_code = 0;

   switch(scale_val)
   {
      case 1: scale_code = 0;
         break;
      case 2: scale_code = 1;
         break;
      case 4: scale_code = 2;
         break;
      case 8: scale_code = 3;
         break;
   }

   c->has_modrm = true;
   c->has_sib = true;

   c->modrm |= get_modrm(disp_val <= 255 ? MOD_MEM_PLUS_DISP8 : MOD_MEM_PLUS_DISP32, 0, RM_SIB);
   c->sib |= get_sib(scale_code, index_code, base_code);
   c->disp_size = disp_val <= 255 ? 1 : 4;
   c->disp = disp_val;

   return false;
}

//
// c_set_imm()
//
bool c_set_imm(coding_t* c, uint64_t imm, uint8_t size)
{
   e_assert(c, "Passed NULL argument");

   c->imm = imm;
   c->imm_size = size;
   return false;
}