#ifdef __GNUC__
#pragma implementation "lexeme.h"
#endif
#include "lexeme.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EmptyDeclOrStmt"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "assembly.h"
#include "errors.h"

#define DEF_NO_EXP_PREF false
#define DEF_NO_MODRM ((uint8_t)-1)
#define DEF_NO_IMM ((uint8_t)-1)
#define DEF_NO_OC_REG false
#define INSTRUCTIONS 21

/*
 * clc
 *   F8 CLC
 *
 * neg reg
 *    F6 /3 NEG r/m8
 *    F7 /3 NEG r/m32
 *
 * dec mem
 *    FE /1 DEC r/m8
 *    FF /1 DEC r/m32
 *
 * add reg, imm
 *    80 /0 ib ADD r/m8, imm8
 *    83 /0 ib ADD r/m32, imm8
 *    81 /0 id ADD r/m32, imm32
 *
 * test reg, mem
 *    84 /r	TEST r8, r/m8
 *    85 /r	TEST r32, r/m32
 *
 * and mem,reg
 *    20 /r AND r/m8, r8
 *    21 /r AND r/m32, r32
 *
 * mov reg, reg
 *    88 /r MOV r/m8, r8
 *    89 /r MOV r/m32, r32
 *
 * cmp mem,imm
 *    80 /7 ib CMP r/m8, imm8
 *    83 /7 ib CMP r/m32, imm8
 *    81 /7 id CMP r/m32, imm32
 *
 * jnbe
 *       77 cb JNBE rel8
 *    OF 87 cw JNBE rel16
 *
 * jmp
 *    EB cb JMP rel8
 *    E9 cw JMP rel16
*/

// All possible instruction of current tasks
static struct instruction_info instruction_infos[INSTRUCTIONS] =
{
      { .name = "CLC", .opcode = 0xF8, .ex_pr = false, .op_cnt = 0, .modrm_index = DEF_NO_MODRM,
        .const_imm = DEF_NO_IMM },

      { .name = "NEG", .opcode = 0xF6, .ex_pr = false, .op_cnt = 1, .modrm_index = 0,
        .const_modrm = 3, .const_imm = DEF_NO_IMM, .op = { OT_REG8 } },
      { .name = "NEG", .opcode = 0xF7, .ex_pr = false, .op_cnt = 1, .modrm_index = 0,
        .const_modrm = 3, .const_imm = DEF_NO_IMM, .op = { OT_REG32 } },

      { .name = "DEC", .opcode = 0xFE, .ex_pr = false, .op_cnt = 1, .modrm_index = 0,
        .const_modrm = 1, .const_imm = DEF_NO_IMM, .op = { OT_MEM8 } },
      { .name = "DEC", .opcode = 0xFF, .ex_pr = false, .op_cnt = 1, .modrm_index = 0,
        .const_modrm = 1, .const_imm = DEF_NO_IMM, .op = { OT_MEM32 } },

      { .name = "ADD", .opcode = 0x80, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = 0, .const_imm = 1, .op = { OT_REG8, OT_IMM8 } },
      { .name = "ADD", .opcode = 0x83, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = 0, .const_imm = 1, .op = { OT_REG32, OT_IMM8 } },
      { .name = "ADD", .opcode = 0x81, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = 0, .const_imm = 4, .op = { OT_REG32, OT_IMM32 } },

      { .name = "TEST", .opcode = 0x84, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = DEF_NO_MODRM, .const_imm = DEF_NO_IMM, .op = { OT_REG8, OT_MEM8 } },
      { .name = "TEST", .opcode = 0x85, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = DEF_NO_MODRM, .const_imm = DEF_NO_IMM, .op = { OT_REG32, OT_MEM32 } },

      { .name = "AND", .opcode = 0x20, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = DEF_NO_MODRM, .const_imm = DEF_NO_IMM, .op = { OT_MEM8, OT_REG8 } },
      { .name = "AND", .opcode = 0x21, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = DEF_NO_MODRM, .const_imm = DEF_NO_IMM, .op = { OT_MEM32, OT_REG32 } },

      { .name = "MOV", .opcode = 0x88, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = DEF_NO_MODRM, .const_imm = DEF_NO_IMM, .op = { OT_REG8, OT_REG8 } },
      { .name = "MOV", .opcode = 0x89, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = DEF_NO_MODRM, .const_imm = DEF_NO_IMM, .op = { OT_REG32, OT_REG32 } },

      { .name = "CMP", .opcode = 0x80, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = 7, .const_imm = 1, .op = { OT_MEM8, OT_IMM8 } },
      { .name = "CMP", .opcode = 0x83, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = 7, .const_imm = 1, .op = { OT_MEM32, OT_IMM8 } },
      { .name = "CMP", .opcode = 0x81, .ex_pr = false, .op_cnt = 2, .modrm_index = 0,
        .const_modrm = 7, .const_imm = 4, .op = { OT_MEM32, OT_IMM32 } },

      { .name = "JNBE", .opcode = 0x77, .ex_pr = false, .op_cnt = 1, .modrm_index = DEF_NO_MODRM,
        .const_modrm = DEF_NO_MODRM, .const_imm = 4, .op = { OT_LABEL_BACKWARD } },
      { .name = "JNBE", .opcode = 0x87, .ex_pr = true,  .op_cnt = 1, .modrm_index = DEF_NO_MODRM,
        .const_modrm = DEF_NO_MODRM, .const_imm = 4, .op = { OT_LABEL_FORWARD } },

      { .name = "JMP", .opcode = 0xEB, .ex_pr = false, .op_cnt = 1, .modrm_index = DEF_NO_MODRM,
        .const_modrm = DEF_NO_MODRM, .const_imm = 4, .op = { OT_LABEL_BACKWARD } },
      { .name = "JMP", .opcode = 0xE9, .ex_pr = false, .op_cnt = 1, .modrm_index = DEF_NO_MODRM,
        .const_modrm = DEF_NO_MODRM, .const_imm = 4, .op = { OT_LABEL_FORWARD } },
};

//
// l_create()
//
lexeme_t* l_create(size_t line)
{
   lexeme_t* l = malloc(sizeof(lexeme_t));
   memset(l, 0, sizeof(lexeme_t));
   l->line = line;
   return l;
}


//
// l_free()
//
void l_free(lexeme_t* lexeme)
{
   e_assert(lexeme, "Passed NULL argument");

   if(lexeme->data)
      free(lexeme->data);

   for(size_t i = 0; i < lexeme->tokens_cnt; i++)
      free(lexeme->tokens[i].string);
   free(lexeme);
}

//
// l_fetch_lexeme_type()
//
bool l_fetch_lexeme_type(lexeme_t* lexeme)
{
   e_assert(lexeme, "Passed NULL argument");

   if(   lexeme->tokens_cnt == 2 &&
         lexeme->tokens[0].type == TT_IDENTIFIER &&
         lexeme->tokens[1].type == TT_SEGMENT_DIRECTIVE)
      lexeme->type = LT_SEGMENT_DEFINITION;

   else if(lexeme->tokens_cnt == 2 &&
         lexeme->tokens[0].type == TT_IDENTIFIER &&
         lexeme->tokens[1].type == TT_ENDS_DIRECTIVE)
      lexeme->type = LT_SEGMENT_END;

   else if(lexeme->tokens_cnt == 3 &&
         lexeme->tokens[0].type == TT_IDENTIFIER &&
         (lexeme->tokens[1].type == TT_DB_DIRECTIVE || lexeme->tokens[1].type == TT_DW_DIRECTIVE || lexeme->tokens[1].type == TT_DD_DIRECTIVE))
      lexeme->type = LT_VAR_DEFINITION;

   else if(lexeme->tokens_cnt == 2 &&
         lexeme->tokens[0].type == TT_IDENTIFIER &&
         lexeme->tokens[1].string[0] == ':')
      lexeme->type = LT_LABEL;

   else if(lexeme->tokens_cnt == 1 &&
         lexeme->tokens[0].type == TT_END_DIRECTIVE)
      lexeme->type = LT_END;

   else
   {
      if(lexeme->has_label && lexeme->tokens_cnt >= 3 && lexeme->tokens[2].type == TT_INSTRUCTION)
         lexeme->type = LT_INSTRUCTION;
      else if(!lexeme->has_label && lexeme->tokens_cnt >= 1 && lexeme->tokens[0].type == TT_INSTRUCTION)
         lexeme->type = LT_INSTRUCTION;
      else
      {
         T_SE(lexeme, "Wrong token in lexeme", 0);
         return false;
      }
   }

   return true;
}

//
// l_fetch_op_info()
//
bool l_fetch_op_info(lexeme_t* lexeme, void* assembly_ptr)
{
   e_assert(lexeme, "Passed NULL argument");
   e_assert(assembly_ptr, "Passed NULL argument");

   assembly_t* assembly = assembly_ptr;

   if(!lexeme->has_instruction || lexeme->operands_count == 0)
      return true;

   for(size_t i = 0; i < lexeme->operands_count; i++)
   {
      struct operand* op = &lexeme->operands_info[i];
      if(op->op_length == 1)
      {
         // Reg, label or imm
         op->operand = &lexeme->tokens[op->op_index];
         switch (op->operand->type)
         {
            case TT_REGISTER8:
               op->type = OT_REG8;
               break;

            case TT_REGISTER32:
               op->type = OT_REG32;
               break;

            case TT_NUMBER2:
            case TT_NUMBER10:
            case TT_NUMBER16:
            {
               int64_t value = t_num(op->operand);
               if(labs(value) < 255)
                  op->type = OT_IMM8;
               else
                  op->type = OT_IMM32;
               break;
            }

            case TT_IDENTIFIER:
            {
               // Label
               struct label* lbl = a_get_label(assembly, op->operand->string);
               if(!lbl)
               {
                  T_SE(lexeme, "Undefined reference", lexeme->operands_info[i].op_index);
                  return false;
               }

               // declared in next lines - forward labeling
               if(lbl->line > lexeme->line)
                  op->type = OT_LABEL_FORWARD;
               else
                  op->type = OT_LABEL_BACKWARD;
               break;
            }

            default:
            {
               T_SE(lexeme, "Wrong token in operator", lexeme->operands_info[i].op_index + 1);
               return false;
            }
         }
      }
      else
      {
         // mem
         op->type = OT_MEM;

         size_t offset = 0;

         // Minimal length: 9 tokens
         // [ ecx + edi * 8 + 1 ]
         // Maximum length: 13 tokens
         // dword ptr ES:[edx+esi*4+6]
         if(!(op->op_length <= 13 && op->op_length >= 9))
         {
            T_SE(lexeme, "Instruction has wrong format (wrong length)",
                  lexeme->operands_info[i].op_index + offset);
            return false;
         }

         // Meh, kinda splice...
         token_t* op_tokens = &lexeme->tokens[op->op_index];

         // Has type specifier
         if(op_tokens[0].type == TT_DWORD || op_tokens[0].type == TT_BYTE || op_tokens[0].type == TT_WORD)
         {
            // After type always must be PTR keyword
            if(op_tokens[1].type != TT_PTR)
            {
               T_SE(lexeme, "Instruction has wrong format (expected PTR)",
                    lexeme->operands_info[i].op_index + offset);
               return false;
            }

            if(op_tokens[0].type == TT_DWORD)
               op->type = OT_MEM32;
            else
               op->type = OT_MEM8;

            op->type_keyword = &op_tokens[0];
            offset += 2;
         }

         // Has segment prefix
         if(op_tokens[offset].type == TT_REGISTER_SEG)
         {
            // Ignore DS prefix because we already in DS segment
            if(strcmp(op_tokens[offset].string, "DS") != 0)
            {
               // After SegReg always must be colon
               if(!(op_tokens[offset + 1].type == TT_SYMBOL && op_tokens[offset + 1].string[0] == ':'))
               {
                  T_SE(lexeme, "Instruction has wrong format (expected ':')",
                       lexeme->operands_info[i].op_index + offset);
                  return false;
               }

               op->segment = &op_tokens[offset];
            }
            offset += 2;
         }

         // Check for minimal length
         if(op->op_length - offset != 9)
         {
                 T_SE(lexeme, "Instruction has wrong format (wrong length)",
                       lexeme->operands_info[i].op_index + offset);
            return false;
         }

         // Check for [REG + REG * NUM + NUM] format
         if(!(op_tokens[offset + 0].type == TT_SYMBOL && op_tokens[offset + 0].string[0] == '['))
         {
            T_SE(lexeme, "Instruction has wrong format (expected '[')",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

         if(!(op_tokens[offset + 1].type == TT_REGISTER8 || op_tokens[offset + 1].type == TT_REGISTER32))
         {
            T_SE(lexeme, "Instruction has wrong format (expected register)",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

         if(!(op_tokens[offset + 2].type == TT_SYMBOL && op_tokens[offset + 2].string[0] == '+'))
         {
            T_SE(lexeme, "Instruction has wrong format (expected '+)",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

         if(!(op_tokens[offset + 3].type == TT_REGISTER8 || op_tokens[offset + 3].type == TT_REGISTER32))
         {
            T_SE(lexeme, "Instruction has wrong format (expected register)",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

         if(!(op_tokens[offset + 4].type == TT_SYMBOL && op_tokens[offset + 4].string[0] == '*'))
         {
            T_SE(lexeme, "Instruction has wrong format (expected '*')",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

         if(!(
            op_tokens[offset + 5].type == TT_NUMBER2 ||
            op_tokens[offset + 5].type == TT_NUMBER10 ||
            op_tokens[offset + 5].type == TT_NUMBER16))
         {
            T_SE(lexeme, "Instruction has wrong format (expected constant)",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

         if(!(op_tokens[offset + 6].type == TT_SYMBOL && op_tokens[offset + 6].string[0] == '+'))
         {
            T_SE(lexeme, "Instruction has wrong format (expected '+')",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

         if(!(
               op_tokens[offset + 7].type == TT_NUMBER2 ||
               op_tokens[offset + 7].type == TT_NUMBER10 ||
               op_tokens[offset + 7].type == TT_NUMBER16))
         {
            T_SE(lexeme, "Instruction has wrong format (expected constant)",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

         if(!(op_tokens[offset + 8].type == TT_SYMBOL && op_tokens[offset + 8].string[0] == ']'))
         {
            T_SE(lexeme, "Instruction has wrong format (expected ']')",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

         op->base = &op_tokens[offset + 1];
         op->index = &op_tokens[offset + 3];
         op->scale = &op_tokens[offset + 5];
         op->disp = &op_tokens[offset + 7];

         // Registers must have same size
         if(!(op->base->type == op->index->type))
         {
            T_SE(lexeme, "Base and index register must have same size",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

         // Scale can be only: 1, 2, 4, 8
         if(!(
               op->scale->string[0] == '1' || op->scale->string[0] == '2' ||
               op->scale->string[0] == '4' || op->scale->string[0] == '8'))
         {
            T_SE(lexeme, "Scale could be only: 1, 2, 4 or 8",
                 lexeme->operands_info[i].op_index + offset);
            return false;
         }

      }
   }

   return true;
}

//
// l_assign_instruction()
//
bool l_assign_instruction(lexeme_t* lexeme)
{
   e_assert(lexeme, "Passed NULL argument");

   if(!lexeme->has_instruction)
      return true;

   token_t* instruction = &(lexeme->tokens[lexeme->instr_index]);

   for(size_t i = 0; i < INSTRUCTIONS; i++)
   {
      struct instruction_info* info = &(instruction_infos[i]);

      // Compare by name
      if(strcmp(instruction->string, info->name) != 0)
         continue;

      // Compare by operands count
      if(lexeme->operands_count != info->op_cnt)
         continue;

      bool type_mismatch = false;

      // Compare by operand types. We should be careful with OT_MEM because its size undefined.
      for(size_t j = 0; j < info->op_cnt; j++)
      {
         if(lexeme->operands_info[j].type != info->op[j])
         {
            type_mismatch = true;

            // Give him another chance by assuming that MEM is MEM32
            if((lexeme->operands_info[j].type == OT_MEM && info->op[j] == OT_MEM32))
            {
               type_mismatch = false;
               continue;
            }

            // Give him another chance by assuming that MEM is MEM8
            if((lexeme->operands_info[j].type == OT_MEM && info->op[j] == OT_MEM8))
            {
               type_mismatch = false;
               continue;
            }

            break;
         }
      }

      if(type_mismatch)
         continue;

      lexeme->info = info;
      return true;
   }

   T_SE(lexeme, "Unable to find matching instruction", lexeme->instr_index);
   return false;
}

size_t l_get_size(lexeme_t* lexeme, void* assembly_ptr)
{
   e_assert(lexeme, "Passed NULL argument");
   e_assert(assembly_ptr, "Passed NULL argument");

   assembly_t* assembly = assembly_ptr;

   if(lexeme->type == LT_VAR_DEFINITION)
   {
      struct variable* var = a_get_variable_by_line(assembly, lexeme->line);
      switch(var->type->type)
      {
         case TT_DB_DIRECTIVE:
            lexeme->size = 1;
            return 1;
         case TT_DW_DIRECTIVE:
            lexeme->size = 2;
            return 2;
         case TT_DD_DIRECTIVE:
            lexeme->size = 4;
            return 4;
      }
   }
   else if(lexeme->info)
   {
      size_t size = 0;

      if(!strcmp(lexeme->info->name, "JMP") || !strcmp(lexeme->info->name, "JNBE"))
      {
         struct operand* operand = &lexeme->operands_info[0];
         struct label* lbl = a_get_label(assembly, operand->operand->string);
         lexeme_t* lbl_lexeme = a_get_lexeme_by_line(assembly, lbl->line);

         int32_t diff = (int32_t)lexeme->offset - (int32_t)lbl_lexeme->offset;

         bool jmp = !strcmp(lexeme->info->name, "JMP");
         bool far = abs(diff) > 127;
         bool back = operand->type == OT_LABEL_BACKWARD;

         // JMP NEAR BACKWARD
         if(jmp && !far && back)
            return 2;
         // JMP FAR BACKWARD, NEAR FORWARD, FAR FORWARD
         if(jmp)
            return 5;

         // JNBE NEAR BACKWARD
         if(!far && back)
            return 2;

         // JNBE FAR BACKWARD, NEAR FORWARD, FAR FORWARD
         return 6;
      }


      // Opcode
      size += 1;

      // Two-byte opcode
      if(lexeme->info->ex_pr)
         size += 1;

      // ModR/M
      if(lexeme->info->modrm_index != DEF_NO_MODRM)
         size += 1;

      // SIB set only if we have memory operand
      // In that case we also have DISP8
      // + possible prefix
      for(size_t i = 0; i < lexeme->operands_count; i++)
      {
         // If at least one of operands is memory
         if(lexeme->operands_info[i].type == OT_MEM ||
            lexeme->operands_info[i].type == OT_MEM32 ||
            lexeme->operands_info[i].type == OT_MEM8)
         {
            size += 1; // SIB

            int64_t disp = t_num(lexeme->operands_info[i].disp);
            if(labs(disp) < 255)
               size += 1; // DISP8
            else
               size += 4; // DISP32

            if(lexeme->operands_info[i].segment)
               size += 1; // Change segment prefix

            break;
         }
      }

      // IMM
      if(lexeme->info->const_imm != DEF_NO_IMM)
         size += lexeme->info->const_imm;

      return size;
   }

   return 0;
}

//
// l_structure()
//
void l_structure(lexeme_t* lexeme)
{
   e_assert(lexeme, "Passed NULL argument");

   size_t off = 0;

   // has label
   if(lexeme->tokens_cnt >= 2 &&
      lexeme->tokens[0].type == TT_IDENTIFIER &&
      lexeme->tokens[1].type == TT_SYMBOL &&
      lexeme->tokens[1].string[0] == ':')
   {
      lexeme->has_label = true;
      off += 2; // lbl :
   }

   if(lexeme->tokens_cnt <= off)
   {
      // only label
      lexeme->has_instruction = false;
      return;
   }

   if(lexeme->tokens[off].type == TT_IDENTIFIER)
   {
      //has name
      lexeme->has_label = true;
      off += 1;
   }

   if(lexeme->tokens[off].type == TT_INSTRUCTION ||
      lexeme->tokens[off].type == TT_DW_DIRECTIVE ||
      lexeme->tokens[off].type == TT_DB_DIRECTIVE ||
      lexeme->tokens[off].type == TT_DD_DIRECTIVE ||
      lexeme->tokens[off].type == TT_END_DIRECTIVE ||
      lexeme->tokens[off].type == TT_ENDS_DIRECTIVE ||
      lexeme->tokens[off].type == TT_SEGMENT_DIRECTIVE)
   {
      lexeme->has_instruction = true;
   }
   else
   {
      return;
   }

   lexeme->instr_index = off;
   off += 1;

   if(lexeme->tokens_cnt <= off)
   {
      // has instruction only
      return;
   }

   lexeme->operands_info[0].op_index = off;
   for(size_t i = off; i < lexeme->tokens_cnt; i++)
   {
      token_t* t = &lexeme->tokens[i];
      if(t->type == TT_SYMBOL && t->string[0] == ',')
      {
         lexeme->operands_info[lexeme->operands_count + 1].op_index =
               lexeme->operands_info[lexeme->operands_count].op_length + lexeme->operands_count + 1 + off;
         lexeme->operands_count++;
      }
      else
      {
         lexeme->operands_info[lexeme->operands_count].op_length += 1;
      }
   }

   lexeme->operands_count++;
}

//
// l_string()
//
void l_string(char* buffer, size_t len, lexeme_t* lexeme)
{
   e_assert(buffer, "Passed NULL argument");
   e_assert(lexeme, "Passed NULL argument");

   buffer[0] = 0;
   for(size_t i = 0; i < lexeme->tokens_cnt; i++)
   {
      token_t* tk = &lexeme->tokens[i];
      bool before = false, after = false;

      if(tk->type == TT_INSTRUCTION || tk->type == TT_DWORD || tk->type == TT_BYTE || tk->type == TT_PTR)
         after = true;
      else if(tk->type == TT_DW_DIRECTIVE || tk->type == TT_DB_DIRECTIVE || tk->type == TT_DD_DIRECTIVE)
      {
         before = true;
         after = true;
      }
      else if(tk->type == TT_SEGMENT_DIRECTIVE || tk->type == TT_ENDS_DIRECTIVE)
         before = true;
      else if(tk->type == TT_SYMBOL && (tk->string[0] == '+' || tk->string[0] == '*'))
      {
         after = true;
         before = true;
      }
      else if(tk->type == TT_SYMBOL && tk->string[0] == ',')
         after = true;

      if(before)
         strncat(buffer, " ", len);
      strcat(buffer, tk->string);
      if(after)
         strncat(buffer, " ", len);
   }

   size_t identation = 0;
   if(lexeme->type == LT_SEGMENT_DEFINITION || lexeme->type == LT_SEGMENT_END || lexeme->type == LT_END)
      identation = 0;
   else if(lexeme->type == LT_LABEL || lexeme->type == LT_VAR_DEFINITION)
      identation = 3;
   else
      identation = 6;

   size_t l = strlen(buffer);
   if(l + identation >= len)
      l = len - identation;

   memmove(buffer + identation, buffer, l + 1);
   memset(buffer, ' ', identation);
}

#pragma clang diagnostic pop