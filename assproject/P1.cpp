/*
	This is just a skeleton. It DOES NOT implement all the requirements. 
	It only recognized the "ADD" and the "LW" instructions and prints 
	"Unkown Instruction" for all other instructions!
*/

// WTF , This comment is added by Yehia.

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include<string>

using namespace std;

int regs[32]={0};
char memory[8*1024];	// only 8KB of data memory 
						// Starts from address 0x10010000

void emitError(char *s)
{
	cout << s;
	exit(0);
}

void decodeReg( unsigned int r, string reg)
{
	switch(r){
	case(0): reg="zero";break;
	case(1): reg="at";break;
	case(2): reg="v0";break;
	case(3): reg="v1";break;
	case(4): reg="a0";break;
	case(5): reg="a1";break;
	case(6): reg="a2";break;
	case(7): reg="a3";break;
	case(8): reg="t0";break;
	case(9): reg="t1";break;
	case(10): reg="t2";break;
	case(11): reg="t3";break;
	case(12): reg="t4";break;
	case(13): reg="t5";break;
	case(14): reg="t6";break;
	case(15): reg="t7";break;
	case(16): reg="s0";break;
	case(17): reg="s1";break;
	case(18): reg="s2";break;
	case(19): reg="s3";break;
	case(20): reg="s4";break;
	case(21): reg="s5";break;
	case(22): reg="s6";break;
	case(23): reg="s7";break;
	case(24): reg="t8";break;
	case(25): reg="t9";break;
	case(26): reg="k0";break;
	case(27): reg="k1";break;
	case(28): reg="gp";break;
	case(29): reg="sp";break;
	case(30): reg="fp";break;
	case(31): reg="ra";break;

	default: reg="unknown reg";

	}
}

void decodeInst(unsigned int instWord)
{
	unsigned int rd, rs, rt, func, shamt, imm, opcode;
	unsigned int address;
	int sImm,zImm,bAdd;	//sign extension and zero extension and branch address
	static unsigned int pc = 0x00400000 ;
	
	opcode = instWord >> 26;
	
	cout << "<-----" << opcode <<"\n";
	
	if(0 == (opcode)) {
		//R-Format
		func  = instWord & 0x3F;
		shamt = (instWord>>6) & 0x1f;
		rd    = (instWord>>11) & 0x1f;
		rt    = (instWord>>16) & 0x1f;
		rs    = (instWord>>21) & 0x1f;
		string rS,rT,rD;
		switch(func){
			//add
			decodeReg(rs,rS);
			decodeReg(rt,rT);
			decodeReg(rd,rD);

			case 0x20:
				cout << "0x" << hex << pc << "\tADD\t$" <<dec<<rs<< ", $" << rd << ", $" << rt << "\n" ;
			
				regs[rd]=regs[rs]+regs[rt];	
				break;

			//addu
			case 0x21: cout << "0x" << hex << pc << "\tADDU\t$" << dec << rd << ", $" << rs << ", $" << rt << "\n" ;
			
				regs[rd]=regs[rs]+regs[rt];	
				break;
		
				//sub
			case 0x22: cout << "0x" << hex << pc << "\tSUB\t$" << dec << rd << ", $" << rs << ", $" << rt << "\n" ;
			
				regs[rd]=regs[rs]-regs[rt];	
				break;

					//and
			case 0x24: cout << "0x" << hex << pc << "\tAND\t$" << dec << rd << ", $" << rs << ", $" << rt << "\n" ;
			
				regs[rd]=regs[rs]&regs[rt];	
				break;

					//or
			case 0x25: cout << "0x" << hex << pc << "\tOR\t$" << dec << rd << ", $" << rs << ", $" << rt << "\n" ;
			
				regs[rd]=regs[rs]|regs[rt];	
				break;

					//xor
			case 0x26: cout << "0x" << hex << pc << "\tXOR\t$" << dec << rd << ", $" << rs << ", $" << rt << "\n" ;
			
				regs[rd]=regs[rs]^regs[rt];	
				break;

					//slt
			case 0x2A: cout << "0x" << hex << pc << "\tSLT\t$" << dec << rd << ", $" << rs << ", $" << rt << "\n" ;
			
				regs[rd]=(regs[rs]<regs[rt])? 1:0;	
				break;

					//srl
			case 0x02: cout << "0x" << hex << pc << "\tSRL\t$" << dec << rd << ", $" << rt << ", " << shamt << "\n" ;
			
				regs[rd]=regs[rt]>>shamt;	
				break;

					//sll
			case 0x00: cout << "0x" << hex << pc << "\tSLL\t$" << dec << rd << ", $" << rt << ", " << shamt << "\n" ;
			
				regs[rd]=regs[rt]<<shamt;	
				break;


					//jr
			case 0x08: cout << "0x" << hex << pc << "\tJR\t$" << dec << rs << "\n" ;
			
				pc=regs[rs];	
				break;

					//syscall
			case 0x0C: cout << "0x" << hex << pc << "\tSYSCALL\n" ;
				
				break;



			
			
			default: cout << "Unkown R-Format Instruction \n";
		}
	} else if( 	0!= opcode && 
			2!=opcode && 
			3!=opcode && 
			16!=opcode && 
			17!=opcode && 
			18!=opcode && 
			19!=opcode
		) {
		// I-Format
		rt    	= (instWord>>16) & 0x1f;
		rs    	= (instWord>>21) & 0x1f;
		imm	  	= (instWord & 0x0000FFFF);
		
		switch(opcode) {
			case 0x23:	//LW
					sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					address 	= regs[rs] + sImm;
					cout << "0x" << hex << pc << "\tLW\t$" << dec << rt << "," << sImm << "($" << rs << ")\n";
				//	regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
				break;

			case 0x2B:	//sw
					sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					address 	= regs[rs] + sImm;
					cout << "0x" << hex << pc << "\tSW\t$" << dec << rt << "," << sImm << "($" << rs << ")\n";
				//	regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
				break;
			case 0x08:	//addi
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					cout << "0x" << hex << pc << "\tADDI\t$" << dec << rt << ", $" << rs << ", " << sImm << "\n" ;
			//
				regs[rt]=regs[rs]+sImm;
				break;

				case 0x09:	//addiu
					sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					cout << "0x" << hex << pc << "\tADDIU\t$" << dec << rt << ", $" << rs << ", " << sImm << "\n" ;
			//
				regs[rt]=regs[rs]+sImm;
				break;

				case 0x0C:	//andi
					zImm 	= 0x00000000 | imm;	// sign extending the immediate field
					cout << "0x" << hex << pc << "\tANDI\t$" << dec << rt << ", $" << rs << ",  0x" << hex<<zImm << "\n" ;
			//
				regs[rt]=regs[rs]&zImm;
				break;

				case 0x0D:	//ori
					zImm 	= 0x00000000 | imm;	// sign extending the immediate field
					cout << "0x" << hex << pc << "\tORI\t$" << dec << rt << ", $" << rs << ",  0x" << hex<<zImm << "\n" ;
			//
				regs[rt]=regs[rs]|zImm;
				break;

				case 0x0E:	//xori
					zImm 	= 0x00000000 | imm;	// sign extending the immediate field
					cout << "0x" << hex << pc << "\tXORI\t$" << dec << rt << ", $" << rs << ",  0x" << hex<<zImm << "\n" ;
			//
				regs[rt]=regs[rs]^zImm;
				break;

				case 0x28:	//sb
					sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					address 	= regs[rs]	+ sImm;
					cout << "0x" << hex << pc << "\tSB\t$" << dec << rt << "," << sImm << "($" << rs << ")\n";
					//regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
				break;

				case 0x29:	//sh
					sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					address 	= regs[rs]	+ sImm;
					cout << "0x" << hex << pc << "\tSH\t$" << dec << rt << "," << sImm << "($" << rs << ")\n";
					//regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
				break;

			case 0x20:	//lb
					sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					address 	= regs[rs]	+ sImm;
					cout << "0x" << hex << pc << "\tLB\t$" << dec << rt << "," << sImm << "($" << rs << ")\n";
				//	regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
				break;


			case 0x21:	//lh
					sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					address 	= regs[rs]	+ sImm;
					cout << "0x" << hex << pc << "\tLH\t$" << dec << rt << "," << sImm << "($" << rs << ")\n";
				//	regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
				break;

			case 0x04:	//beq
					sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					bAdd		= (imm &0x4000)? (0xFFFC0000 | (imm*4) ): (0x00000000 | (imm*4) );
					if(regs[rs]==regs[rt]) pc=pc+4+bAdd;
				cout << "0x" << hex << pc << "\tBEQ\t$" << dec << rs << ", $" << rt << ", " << sImm << "\n" ;
					//regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
				break;

			case 0x05:	//bne
					sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					bAdd		= (imm &0x4000)? ( 0xFFFC0000 | (imm*4) ): ( 0x00000000 | (imm*4) );
					if(regs[rs]!=regs[rt]) pc=pc+4+bAdd;
				cout << "0x" << hex << pc << "\tBNE\t$" << dec << rs << ", $" << rt << ", " << sImm << "\n" ;
					//regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
				break;

			case 0x0A:	//slti
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					cout << "0x" << hex << pc << "\tSLTI\t$" << dec << rt << ", $" << rs << ", " << sImm << "\n" ;
			//
				regs[rd]=(regs[rs]<sImm)? 1:0;
				break;

			case 0x0F:	//lui
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
					cout << "0x" << hex << pc << "\tLUI\t$" << dec << rt << ", $" << rs << ",  0x" << hex<<sImm << "\n" ;
			//
				regs[rt]=imm*65536; //regs[rt]={imm,16'b0)
				break;

			default:
				cout << "0x" << hex << pc << "\tUnkown I-Format Instruction \n";
		
		}
	} else if(opcode==2 || opcode==3){
		// J-Format (J & Jal)

			unsigned int Address  	= (instWord & 0x03FFFFFF);
			unsigned int Nadd;
			unsigned int add_2;


			switch(opcode) {
				
				case 0x02:	//j
					
				Nadd= ((pc+4) & 0xF0000000);  //Nadd= pc+4[31:28]
				add_2=(Nadd | (Address*4) );	//{ PC[31:28], address, 2’b0 }

				cout << "0x" << hex << pc << "\tJ\t"<<"0x" << hex<< add_2 << "\n" ;
				pc=add_2;
				break;

				case 0x03:	//jAL
					
				Nadd= ((pc+4) & 0xF0000000);  //Nimm= pc+4[31:28]
				add_2=(Nadd | (Address*4) );	//{ PC[31:28], address, 2’b0 }

				cout << "0x" << hex << pc << "\tJAL\t" <<"0x"<< hex<< add_2 << "\n" ;
				regs[31]=pc;
				pc=add_2;
				break;

				default:
				cout << "0x" << hex << pc << "\tUnkown J-Format Instruction \n";
						}

	}
	else {
		cout << "Opcode: "<< opcode << " - Unkown Instruction \n";
	}

	pc += 4;
}

int main(int argc, char *argv[]){
	
	unsigned int instWord=0;
	ifstream inFile;
	ofstream outFile;
	
	if(argc<2) emitError("use: mipsim <machine_code_file_name>\n");
	
	inFile.open(argv[1], ios::in | ios::binary);
	
	if(inFile.is_open())
	{
		while(true){
			if(!inFile.read ((char *)&instWord, 4)) break;
			decodeInst(instWord);
		}
		for(int i=0;i<32;i++)
				cout<<dec<<i<<": \t0x"<<hex<< regs[i]<<"\n";		///////////
	} else emitError("Cannot access input file\n");

	system ("pause");
	return 0;

}
