#ifndef CODING_H
#define CODING_H

#include <stdint.h>
#include <stdbool.h>
#include "tokenizer.h"

typedef struct _coding
{
   bool has_prefixes;
   uint8_t prefixes[2];

   bool has_opcode;
   uint8_t opcode;

   bool has_modrm;
   uint16_t modrm;

   bool has_sib;
   uint16_t sib;

   uint8_t disp_size;
   uint32_t disp;

   uint8_t imm_size;
   uint64_t imm;

} coding_t;

// Fills coding object with initial parameters
void c_fill(coding_t* coding);

// Prints coding object to a passed string
void c_string(char* ptr, size_t size, coding_t* coding);

// coding_t object setters.
// Returns false if something went wrong
bool c_set_opcode(coding_t* c, uint8_t opcode);
bool c_set_prefix_exp(coding_t* c);
bool c_set_prefix_seg(coding_t* c, token_t* seg);
bool c_set_reg(coding_t* c, token_t* reg);
bool c_set_reg_const(coding_t* c, uint8_t constant);
bool c_set_rm_reg(coding_t* c, token_t* reg);
bool c_set_rm_m(coding_t* c, token_t* reg1, token_t* reg2, token_t* scale, token_t* disp);
bool c_set_imm(coding_t* c, uint64_t imm, uint8_t size);

#endif // CODING_H
