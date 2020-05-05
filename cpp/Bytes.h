#pragma once
#include <string>
#include <cstdint>

using namespace std;

struct Bytes
{
	bool has_opcode = false;
	uint8_t opcode;

	bool exp_prefix = false;
	
	bool has_seg_prefix = false;
	uint8_t seg_prefix;

	bool has_modrm = false;
	uint8_t modrm = 0;

	int imm_size = 0;
	uint32_t imm = 0;
	string longimm;

	void setOpcode(uint8_t opcode);
	void packRegister(string reg);
	void setExp();
	void setSeg(string token);
	void setModrmReg(string reg);
	void setModrmRegConst(uint8_t c);
	void setModrmModReg(string reg);
	void setModrmModSum(string reg1, string reg2);
	void setImm(int size, uint32_t imm);
	void setImm(int size, string imm);
	string toString();
};

void setupByteDict();
void getBytes();
void printBytes();