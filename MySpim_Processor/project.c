#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	*Zero = 0; //default as zero
	switch(ALUControl) //switch is better than a million if statments
	{
		case 0:
		{
			*ALUresult = A+B; break; //ADD
		}
		case 1:
		{
			*ALUresult = A-B; break; //SUBTRACT
		}
		case 2:
		{
			if ((int)A < (int)B) //Check if A is less than B. Explicitly made SIGNED
			{
				*ALUresult = 1;
			}
			else
			{
				*ALUresult = 0;
			}
			break;
		}
		case 3:
		{
			if (A < B) //Check if A is less than B. Assumes UNSIGNED
			{
				*ALUresult = 1;
			}
			else
			{
				*ALUresult = 0;
			}
			break;
		}
		case 4:
		{
			*ALUresult = A & B; break; //Binary AND operation
		}
		case 5:
		{
			*ALUresult = A | B; break; //Binary OR operation
		}
		case 6:
		{
			*ALUresult = B << 16;	break; //Binary BITSHIFT by 16
		}
		case 7:
		{
			*ALUresult = ~A; break; //BInary NOT operation
		}
		default:{*ALUresult = 0;} //if nothing can be done, assume no operation (0)
	}

	if (*ALUresult == 0) //if the result is zero
	{
		*Zero = 1; //zero is active
	}
	else //otherwise
	{
		*Zero = 0; //Zero is not used
	}
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{

}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	//right shift, then mask in order to only get the bits for the needed field
	*op = (instruction >> 26) & 0x3F; //only needs the last 6 bits

	*r1 = (instruction >> 21) & 0x1F; //bits 26-21, and only 5 bits saved

	*r2 = (instruction >> 16) & 0x1F; //bits 21-16, and only 5 bits saved

	*r3 = (instruction >> 11) & 0x1F; //bits 16-11, and only 5 bits saved

	*funct = instruction  & 0x3F; //Gathers the first 6 bits

	*offset = instruction  & 0xFFFF; //Gathers the first 16 bits

	*jsec = instruction & 0x3FFFFFF; //Gathers the first 26 bits
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	*data1= Reg[r1]; //take the data within the given register (Reg[]) index (rX) and apply to data
	*data2 =Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
	//TODO look at this later
	//If the value is a negative represented value. So it commits a AND operation with 1 the 16th bit
	//the static value is mainly just looking for 1 at the very left handside of the binary at 16 bits
	//if "1" is found against the passed offset on the furthest left bit (for 16 bits) then this procs
	// 0x8000 = 1000000000000000 and doing the AND operation will leave the 16th bit as 1 if the passed value is
	//in fact signed with a 1 at the same position
	if (offset&0x8000)
	{
		//This will do a binary arithmetic OR operation. It will take what is effectiverly all 1's
		//on the left handside of the new 32 bit value, the section that is before the passed binary value
		*extended_value = offset | 0xFFFF0000;
	}
	else
	{
		//if the 16 bit binary is not a negative number, nothing needs to be done and the first 16 bits of the new
		//32 bit binary will just be 0's
		*extended_value = offset;
	}
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	//if ALUSrc says to use the data2 value (from register) by being set to 0, then a temp unsigned int will be used and have its value from data2, otherwise if ALUSrc = 1, use immediate value which is extended_value
	unsigned Value_2;
	if (ALUSrc == 0) { Value_2 = data2; }
	else {	Value_2 = extended_value;}


	//TODO Look at this later
	char Operation_type; //temp variable for the operation code that will be run
	if (ALUOp == 7) //if the operation code is 7, check the specified function type to run
	{
		switch(funct) //switch statement is nicer than a million if's
		{
			case 32:
			{
				Operation_type = 0; break;
			}
			case 34:
			{
				Operation_type = 1; break;
			}
			case 36:
			{
				Operation_type = 4; break;
			}
			case 37:
			{
				Operation_type = 5; break;
			}
			case 42:
			{
				Operation_type = 2; break;
			}
			default:
			{
				return 1;
			}
		}
	}
	else { Operation_type = ALUOp; } //otherwise, the passed operation and type are the same

	//Pass values to the actual ALU logic
	ALU(data1, Value_2, Operation_type, ALUresult, Zero); //run the operation

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	if (RegWrite == 0){return;} //if writing to the register is false, then do nothing

	unsigned Register_Index; //Make it exist on the stack
	if (RegDst == 0) {Register_Index = r2;} //check if we should use r2 or r3
	else {Register_Index = r3;}

	unsigned Data; //Make it exist on the stack
	if (MemtoReg == 0){Data = ALUresult;} //check if ALUresult or memdata is used based on MemtoReg value
	else {Data = memdata;}

	Reg[Register_Index] = Data; //Apply specified data to specified Register
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	unsigned Update = *PC + 4; //Get the base case for the new PC value

    if (Branch == 1 && Zero == 1) //if branch is true and zero is true, use the branch path
    {
		Update = (*PC + 4) + (extended_value << 2); //take the value of PC+4 added with the shifted value of the passed 32 bit binary shifted left by 2
    }

    if (Jump == 1) //if jump is active take the PC+4 (masked with upper 4 bits) and OR it with the shifted jsec (lower 28 bits)
    {
		Update = ( (*PC + 4)&0xF0000000 ) | (jsec << 2); //extra parenthesis just in case for correct order
    }

    *PC = Update; //update PC
}

