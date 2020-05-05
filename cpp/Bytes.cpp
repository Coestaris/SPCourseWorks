#include "Delimiter.h"
#include "Bytes.h"
#include "Assembly.h"
#include <cassert>

static unordered_map<string, uint8_t> regCodes;
static unordered_map<string, uint8_t> segCodes;

static const uint8_t mod_sum = 0b00;
static const uint8_t mod_reg = 0b11;

static const uint8_t rm_bx_si = 0b000;
static const uint8_t rm_bx_di = 0b001;
static const uint8_t rm_bp_si = 0b010;
static const uint8_t rm_bp_di = 0b011;

static uint8_t get_rm_byte_tokens(string& tok1, string& tok2)
{
	if (tok1 == "bx" && tok2 == "si") return rm_bx_si;
	if (tok1 == "bx" && tok2 == "di") return rm_bx_di;
	if (tok1 == "bp" && tok2 == "si") return rm_bp_si;
	if (tok1 == "bp" && tok2 == "di") return rm_bp_di;

	// And vice versa
	if (tok2 == "bx" && tok1 == "si") return rm_bx_si;
	if (tok2 == "bx" && tok1 == "di") return rm_bx_di;
	if (tok2 == "bp" && tok1 == "si") return rm_bp_si;
	if (tok2 == "bp" && tok1 == "di") return rm_bp_di;
	
	assert("You passed some shit to me...");
}

static uint8_t get_modrm(uint8_t mod, uint8_t reg, uint8_t rm)
{
	return (mod & 0b11) << 6 | (reg & 0b111) << 3 | (rm & 0b111);
}

void Bytes::setOpcode(uint8_t opcode_val)
{
	has_opcode = true;
	opcode = opcode_val;
}

void Bytes::packRegister(string reg)
{
	has_opcode = true;
	opcode |= regCodes[reg];
}

void Bytes::setExp()
{
	exp_prefix = true;
}

void Bytes::setSeg(string token)
{
	has_seg_prefix = true;
	seg_prefix = segCodes[token];
}

void Bytes::setModrmReg(string reg)
{
	has_modrm = true;
	modrm |= get_modrm(0, regCodes[reg], 0);
}

void Bytes::setModrmRegConst(uint8_t c)
{
	has_modrm = true;
	modrm |= get_modrm(0, c, 0);
}

void Bytes::setModrmModReg(string reg)
{
	has_modrm = true;
	modrm |= get_modrm(mod_reg, 0, regCodes[reg]);
}

void Bytes::setModrmModSum(string reg1, string reg2)
{
	has_modrm = true;
	uint8_t rm = get_rm_byte_tokens(reg1, reg2);
	modrm |= get_modrm(mod_sum, 0, rm);
}

void Bytes::setImm(int size, uint32_t imm_val)
{
	imm_size = size;
	if (imm_size == 1)
		imm = imm_val & 0xFF;
	else if(imm_size == 2)
		imm = imm_val & 0xFFFF;
	else
		imm = imm_val & 0xFFFFFF;
}

void Bytes::setImm(int size, string imm_str)
{
	imm_size = size;
	if (imm_size <= 4)
	{
		imm = 0;
		for (int i = 0; i < imm_size; i++)
			imm |= (imm_str[i] << ((imm_size - i - 1) * 8)) & 0xFF;
	}
	else longimm = imm_str;
}

string Bytes::toString()
{
	string result = "";
	if (has_seg_prefix)
		result = padTo(seg_prefix, 2, '0', true) + ": ";
	
	if (exp_prefix)
		result += "0F ";

	if (has_opcode)
		result += padTo(opcode, 2, '0', true) + " ";

	if (has_modrm)
		result += padTo(modrm, 2, '0', true) + " ";

	if (imm_size != 0)
	{
		if (imm_size > 4) {
			for (auto c : longimm)
				result += padTo(c, 2, '0', true);
			result += " ";
		}
		else
			result += padTo(imm, imm_size * 2, '0', true) + " ";
	}

	return result;
}

void setupByteDict()
{
	regCodes["ax"] = 0b000;
	regCodes["cx"] = 0b001;
	regCodes["dx"] = 0b010;
	regCodes["bx"] = 0b011;
	regCodes["sp"] = 0b100;
	regCodes["bp"] = 0b101;
	regCodes["si"] = 0b110;
	regCodes["di"] = 0b111;

	regCodes["al"] = 0b000;
	regCodes["cl"] = 0b001;
	regCodes["dl"] = 0b010;
	regCodes["bl"] = 0b011;
	regCodes["ah"] = 0b100;
	regCodes["ch"] = 0b101;
	regCodes["dh"] = 0b110;
	regCodes["bh"] = 0b111;

	segCodes["es"] = 0x26;
	segCodes["cs"] = 0x2E;
	segCodes["ss"] = 0x36;
	segCodes["ds"] = 0x3E;
	segCodes["fs"] = 0x64;
	segCodes["gs"] = 0x65;
}
