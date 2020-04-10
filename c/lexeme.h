#ifndef LEXEME_H
#define LEXEME_H

#include <stddef.h>
#include <stdbool.h>

#include "tokenizer.h"
#include "coding.h"

// Set error.
#define T_SE(l, msg, t) { l->err = msg; l->err_tk = t; }

// Mnemonic type of the lexeme
typedef enum _lexeme_type
{
   LT_SEGMENT_DEFINITION,
   LT_VAR_DEFINITION,
   LT_SEGMENT_END,
   LT_LABEL,
   LT_INSTRUCTION,
   LT_END

} lexeme_type_t;

// Mnemonic type of each lexeme's operand
typedef enum _operand_type
{
   OT_REG8,
   OT_REG32,
   OT_MEM,
   OT_MEM8,
   OT_MEM32,
   OT_LABEL_FORWARD,
   OT_LABEL_BACKWARD,
   OT_IMM8,
   OT_IMM32

} operand_type_t;

// Contains all information about instruction
typedef struct instruction_info {

   // Instruction string value
   const char* name;

   // Has expansion prefix (0x0F)
   bool ex_pr;
   // Instruction opcode
   uint8_t opcode;

   // Operands count and its types
   size_t op_cnt;
   operand_type_t op[10];

   // ModR/M index. If set to 0 - first operand: reg/mem, second: reg and vice verse
   // If set to -1 instruction doesn't have ModRM byte
   uint8_t modrm_index;

   // Constant value storing in REG field of ModR/M
   uint8_t const_modrm;

   // Size instruction's IMM field.
   // If set to -1, instruction doesn't have this field at all
   uint8_t const_imm;

} instruction_info_t;


// Represent assembly line
typedef struct _lexeme
{
   // If everything OK set to null. Sets to error msg when something went wrong.
   const char* err;
   size_t err_tk;

   // Line tokens
   size_t tokens_cnt;
   token_t tokens[100];

   // Line index
   size_t line;

   // Instruction size in assembly. Set by l_get_size()
   size_t size;
   // Instruction offset in assembly
   size_t offset;

   // Type of the lexeme. Set by l_fetch_lexeme_type()
   lexeme_type_t type;

   // Lexeme structure. Set by l_structure()
   bool has_instruction;
   bool has_label;
   size_t instr_index;
   size_t operands_count;

   // Info about lexeme's instruction. Set by l_assign_instruction()
   instruction_info_t* info;

   // Bytes of the instruction
   coding_t data;

   // Information about operands. Set by l_fetch_op_info()
   struct operand
   {
      // Index of the operand in lexeme.tokens
      size_t op_index;
      // Count of tokens in current operand
      size_t op_length;

      // Type of the current operand
      operand_type_t type;

      // For immX: Given Constant
      // For regX: Given register
      // For labelX: Given label
      // For memX set to NULL
      token_t* operand;

      // Set when OT_MEM, OT_MEM8 or OT_MEM32
      // Otherwise all these fields set to NULL
      // dword  ptr  ES : [ edx + esi * 4 + 6 ]
      //   1          2      3     4    5   6
      // 1 - Type keyword. Could be NULL
      // 2 - Segment. Could be NULL
      // 3 - Base register. Always set
      // 4 - Index register. Always set
      // 5 - Scale. Always set.
      // 6 - Displacement. Always set
      token_t* type_keyword;
      token_t* segment;
      token_t* base;
      token_t* index;
      token_t* scale;
      token_t* disp;

   } operands_info[5];

} lexeme_t;

// Allocates new lexeme
lexeme_t* l_create(size_t line);

// Frees lexeme
void l_free(lexeme_t* lexeme);

// Converts lexeme to a pretty string
void l_string(char* buffer, size_t len, lexeme_t* lexeme);

//
// FIRST STAGE METHODS
//
// Determines structure of the instruction
void l_structure(lexeme_t* lexeme);

//
// FIRST PASS METHODS
//
// Determines lexeme's type
bool l_fetch_lexeme_type(lexeme_t* lexeme);

// Gets all necessary information about instruction operands according to KR task
// Assembly - casted to void* parent Assembly object.
// Used this trick because C can't handle circular dependencies in headers
bool l_fetch_op_info(lexeme_t* lexeme, void* assembly);

// Find instruction that fits current lexeme
bool l_assign_instruction(lexeme_t* lexeme);

// Calculates size of the current instruction / variable definition
// Assembly is the same as in l_fetch_op_info().
size_t l_get_size(lexeme_t* lexeme, void* assembly);

//
// SECOND PASS METHODS
//
// Calculates bytes using all collected information
void l_get_bytes(lexeme_t* lexeme, void* assembly);

#endif // LEXEME_H
