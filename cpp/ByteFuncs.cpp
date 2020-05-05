#include "Assembly.h"

#include <string>
#include <iomanip>
#include <cassert>

#include "TokenType.h"
#include "Delimiter.h"
#include "Bytes.h"

static vector<end_token*> get_operand(int l, int op)
{
	vector<end_token*> result;
	if (lexems[l].numberOfOperands < op + 1)
		return result;
	int start = lexems[l].operandIndices[op + 1];
	int end = start + lexems[l].operandLengths[op + 1];
	for (int i = start; i < end; i++)
		result.push_back(&vectorOfTokens[l][i]);
	return result;
}

static void SetDBBytes(int l)
{
	auto op = get_operand(l, 0);
	if (op[0]->type == Text)
	{
		string toPack = op[0]->token.substr(1, op[0]->token.length() - 2);
		lexems[l].bytes.setImm(op[0]->token.length() - 2, toPack);
	}
	else
	{
		auto imm = tokenToNumber(op[0]);
		lexems[l].bytes.setImm(1, imm);
	}
}

static void SetDDBytes(int l)
{
	auto op = get_operand(l, 0);
	auto imm = tokenToNumber(op[0]);
	lexems[l].bytes.setImm(4, imm);
}

static void SetDWBytes(int l)
{
	auto op = get_operand(l, 0);
	auto imm = tokenToNumber(op[0]);
	lexems[l].bytes.setImm(2, imm);
}

static void SetInstructionBytes(int l)
{
	// 3F  AAS

	// FE /0 INC r/m8 
	// 40+rw INC r16

	// F6 /3 NEG r/m8
	// F7 /3 NEG r/m16

	// 0F A3 BT r/m16, r16 

	// 22 /r AND r8, r/m8 
	// 23 /r AND r16, r/m16 

	// 38 /r CMP r/m8, r8
	// 39 /r CMP r/m16, r16

	// B0+rb MOV reg8, imm8
	// B8+rw MOV reg16, imm16

	// 80 /1 ib OR r/m8, imm8
	// 83 /1 ib OR r/m16, imm8
	// 81 /1 iw OR r/m16, imm16

	// 7C cb JL rel8
	// OF 8C cw JL rel16 

	end_token& instruction = vectorOfTokens[l][lexems[l].instrIndex];
	operandType op1 = lexems[l].operandTypes[0];
	operandType op2 = lexems[l].operandTypes[1];
	vector<end_token*> op1t;
	vector<end_token*> op2t;

	bool smallMode = true;
	if (lexems[l].numberOfOperands >= 1)
	{
		op1t = get_operand(l, 0);
		if (op1 == OT_Register16 || op1 == OT_Memory16)
			smallMode = false;
	}
	if (lexems[l].numberOfOperands == 2)
	{
		op2t = get_operand(l, 1);
		if (op2 == OT_Register16 || op2 == OT_Memory16)
		{
			if (smallMode)
			{
				lexems[l].SetError("Operand types mismatch", *op2t[0]);
				return;
			}
			smallMode = false;
		}
	}
	Bytes* b = &lexems[l].bytes;

	if (lexems[l].has_segment_prefix)
	{
		if (lexems[l].segment_prefix.token == "ss")
		{
			if (lexems[l].sum1_tk->token != "bp" && lexems[l].sum2_tk->token != "bp")
				b->setSeg(lexems[l].segment_prefix.token);
		}
		else if (lexems[l].segment_prefix.token == "ds")
		{
			if (lexems[l].sum1_tk->token == "bp" || lexems[l].sum2_tk->token == "bp")
				b->setSeg(lexems[l].segment_prefix.token);
		}
		else
			b->setSeg(lexems[l].segment_prefix.token);
	}

	if (instruction.token == "aas") {
		b->setOpcode(0x3F);
	}
	else if (instruction.token == "inc") {
		if (smallMode)
		{
			b->setOpcode(0xFE);
			b->setModrmModReg(op1t[0]->token);
			b->setModrmRegConst(0);
		}
		else
		{
			b->setOpcode(0x40);
			b->packRegister(op1t[0]->token);
		}
	}
	else if (instruction.token == "neg") {
		if (smallMode) b->setOpcode(0xF6);
		else b->setOpcode(0xF7);
		
		b->setModrmModSum(lexems[l].sum1_tk->token, lexems[l].sum2_tk->token);
		b->setModrmRegConst(3);
	}
	else if (instruction.token == "bt") {
		b->setExp();
		b->setOpcode(0xA3);
		b->setModrmModReg(op1t[0]->token);
		b->setModrmReg(op2t[0]->token);
	}
	else if (instruction.token == "and") {
		if (smallMode) b->setOpcode(0x22);
		else b->setOpcode(0x23);

		b->setModrmModSum(lexems[l].sum1_tk->token, lexems[l].sum2_tk->token);
		b->setModrmReg(op1t[0]->token);
	}
	else if (instruction.token == "cmp") {
		if (smallMode) b->setOpcode(0x38);
		else  b->setOpcode(0x39);

		b->setModrmModSum(lexems[l].sum1_tk->token, lexems[l].sum2_tk->token);
		b->setModrmReg(op2t[0]->token);
	}
	else if (instruction.token == "mov") {
		if (smallMode)
		{
			b->setOpcode(0xB0);
			b->setImm(1, tokenToNumber(op2t[0]));
		}
		else 
		{
			b->setOpcode(0xB8);
			b->setImm(2, tokenToNumber(op2t[0]));
		}
		b->packRegister(op1t[0]->token);
	}
	else if (instruction.token == "or") {
		if (smallMode)
		{
			b->setOpcode(0x80);
			b->setImm(1, tokenToNumber(op2t[0]));
		}
		else
		{
			if (op2 == OT_Const8)
			{
				b->setOpcode(0x83);
				b->setImm(1, tokenToNumber(op2t[0]));
			}
			else
			{
				b->setOpcode(0x81);
				b->setImm(2, tokenToNumber(op2t[0]));
			}
		}
		
		b->setModrmRegConst(1);
		b->setModrmModSum(lexems[l].sum1_tk->token, lexems[l].sum2_tk->token);
	}
	else if (instruction.token == "jl") {
		UserName* un = getUserName(NT_Label, op1t[0]->token);
		assert(un);

		int diff = lexems[un->begin].offset - lexems[l].offset - lexems[l].size;
		bool far = abs(diff) > 127;

		if (far)
		{
			b->setOpcode(0x8C);
			b->setExp();
			b->setImm(2, diff);
		}
		else
		{
			b->setOpcode(0x7C);
			if (op1 == OT_LabelBack)
				b->setImm(1, diff);
			else
			{
				int a = (diff + 2) << 16 | 0x9090;
				b->setImm(3, a);
			}
		}
	}
}

void getBytes()
{
	for (int l = 0; l < vectorOfTokens.size(); l++)
	{
		if (lexems[l].has_error)
			continue;

		UserName* currSegment = getUserName(NT_Segment, l);
		if (!currSegment)
			continue;

		if (lexems[l].hasInstruction)
		{
			end_token& tok = vectorOfTokens[l][lexems[l].instrIndex];
			if (tok.type == DbDirective)
				SetDBBytes(l);
			else if (tok.type == DwDirective)
				SetDWBytes(l);
			else if (tok.type == DdDirective)
				SetDDBytes(l);
			else if (tok.type == Instruction)
				SetInstructionBytes(l);
		}
	}
}

void printBytes()
{
	// Print lexeme list
	cout << " # | SIZE|  OFFSET  |         BYTES       |            LINE    \n";
	cout << "===========================================================================\n";
	for (int l = 0; l < vectorOfTokens.size(); l++)
	{
		UserName* currSegment = getUserName(NT_Segment, l);

		// Output line index
		cout << padTo(l, 2, '0');
		cout << " | ";

		// Output size or macro 
		if (lexems[l].hasMacro && !lexems[l].hasLabel)
		{
			cout << "   ---- M ---  |:";
		}
		else if(lexems[l].has_error)
		{
			cout << "ERROR \t     ";
		}
		else
		{
			if (currSegment)
			{
				cout << padTo(lexems[l].size, 2, '0', true) << " :|  ";
				cout << padTo(lexems[l].offset, 6, '0', true) << " :| ";
			}
			else
			{
				cout << "--- | -------- |:";
			}
		}

		// Print bytes
		if (!lexems[l].has_error && lexems[l].hasInstruction)
		{
			end_token& tok = vectorOfTokens[l][lexems[l].instrIndex];
			if (!(tok.type == Instruction || tok.type == DbDirective || tok.type == DdDirective || tok.type == DwDirective) || !currSegment)
			{
				cout << "                   :| ";
			}
			else
			{
				cout << setw(19) << left << lexems[l].bytes.toString() << ":| ";
			}
		}
		else
		{
			cout << "                   :| ";
		}

		// Output indentation
		if (!lexems[l].hasLabel)
		{
			if (lexems[l].hasInstruction)
			{
				TokenType type = vectorOfTokens[l][lexems[l].instrIndex].type;
				if (type != EndmKeyword && type != EndsKeyword && type != SegmentKeyword)
					cout << "  ";
			}
			else cout << "  ";
		}
		if (currSegment && currSegment->begin != 0)
			cout << "  ";

		// Output tokens
		for (int t = 0; t < vectorOfTokens[l].size(); t++)
			cout << vectorOfTokens[l][t].token << " ";
		cout << "\n";
	}
	cout << "===========================================================================\n";
}