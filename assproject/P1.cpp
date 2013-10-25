/*
	This is just a skeleton. It DOES NOT implement all the requirements. 
	It only recognized the "ADD" and the "LW" instructions and prints 
	"Unkown Instruction" for all other instructions!
*/

// Yet another update :D
// My first commit


#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <string>

using namespace std;

int regs[32]={0};
char memory[8*1024];	// only 8KB of data memory 
						// Starts from address 0x10010000

bool pseudo = false; // for decoding MARS pseudo instructions

void decodeInst(unsigned int instWord);

void emitError(char *s)
{
	cout << s;
	exit(0);
}

string decodeReg(unsigned int r)
{
	switch(r){
	case 0: return "zero";
	case 1: return "at";
	case 2: return "v0";
	case 3 : return "v1";
	case 4: return "a0";
	case 5: return "a1";
	case 6: return "a2";
	case 7: return "a3";
	case 8 : return "t0";
	case 9: return "t1";
	case 10: return "t2";
	case 11: return "t3";
	case 12: return "t4";
	case 13: return "t5";
	case 14 : return "t6";
	case 15 : return "t7";
	case 16 : return "s0";
	case 17: return "s1";
	case 18 : return "s2";
	case 19 : return "s3";
	case 20: return "s4";
	case 21 : return "s5";
	case 22: return "s6";
	case 23: return "s7";
	case 24: return "t8";
	case 25: return "t9";
	case 26: return "k0";
	case 27: return "k1";
	case 28: return "gp";
	case 29: return "sp";
	case 30: return "fp";
	case 31: return "ra";

	default: return "unknown reg";

	}
}

// Making them global is better for more modularized/procedural programming
unsigned int rd, rs, rt, func, shamt, imm, opcode;
unsigned int address;
int sImm,zImm,bAdd;	// sign extension and zero extension and branch address
static unsigned int pc = 0x00400000 ;

void i_format(unsigned int instWord) {
	// I-Format
	rt    	= (instWord>>16) & 0x1f;
	rs    	= (instWord>>21) & 0x1f;
	imm	  	= (instWord & 0x0000FFFF);
	
	switch(opcode) {
		case 0x23:	//lw
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs] + sImm;
				cout << "0x" << hex << pc << "\tlw\t$" << decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as other instructions are not implemented 
			break;

		case 0x2B:	//sw
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs] + sImm;
				cout << "0x" << hex << pc << "\tsw\t$" << decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				memory[address - 0x10010000]= regs[rt];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;
		case 0x08:	//addi
			sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
			
			if(rt == 1) {
				pseudo = true;
			}
			else {
				cout << "0x" << hex << pc << "\taddi\t$" <<decodeReg(rt) << ", $" << decodeReg(rs) << ", " <<dec<< sImm << "\n" ;
			}
			
			regs[rt]=regs[rs]+sImm;
			break;

			case 0x09:	//addiu
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				
				if(rs == 0) // pseudo : li
				{
					cout << "0x" << hex << pc << "\tli\t$" <<decodeReg(rt) << ", " << dec << sImm << "\n" ;
				}
				else
				{
					cout << "0x" << hex << pc << "\taddiu\t$" <<decodeReg(rt) << ", $" << decodeReg(rs) << ", " << dec<<sImm << "\n" ;
				}
				
		//
				regs[rt]=regs[rs]+sImm;
			break;

			case 0x0C:	//andi
				zImm 	= 0x00000000 | imm;	// sign extending the immediate field
				cout << "0x" << hex << pc << "\tandi\t$" <<decodeReg(rt) << ", $" << decodeReg(rs) << ",  0x" << hex<<zImm << "\n" ;
		//
				regs[rt]=regs[rs]&zImm;
			break;

			case 0x0D:	//ori
				zImm 	= 0x00000000 | imm;	// sign extending the immediate field
				// cout << "Imm: 0x" << hex << imm << " zImm : 0x" << hex << zImm << " ored : 0x" <<hex << (regs[rs] | zImm) << endl;
				regs[rt]=regs[rs] | zImm;
				if(pseudo) {
					pseudo = false;
					cout << "0x" << hex << pc << "\tla\t$" << decodeReg(rt) << ",  0x" << hex<< regs[rt] << "\n" ;
				} else {
					cout << "0x" << hex << pc << "\tori\t$" << decodeReg(rt) << ", $" << decodeReg(rs) << ",  0x" << hex<< zImm << "\n" ;
				}
			break;

			case 0x0E:	//xori
				zImm 	= 0x00000000 | imm;	// sign extending the immediate field
				cout << "0x" << hex << pc << "\txori\t$" << decodeReg(rt) << ", $" << decodeReg(rs) << ",  0x" << hex<<zImm << "\n" ;
		//
			regs[rt]=regs[rs]^zImm;
			break;

			case 0x28:	//sb
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				cout << "0x" << hex << pc << "\tsb\t$" << decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				memory[address - 0x10010000]=regs[rt];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

			case 0x29:	//sh
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				cout << "0x" << hex << pc << "\tsh\t$"<< decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
			memory[address - 0x10010000]=regs[rt];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

		case 0x20:	//lb
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				cout << "0x" << hex << pc << "\tlb\t$"<< decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				regs[rt]	= memory[address - 0x10010000]&0xFF;		// uncommenting this would crash the program as othe instructions are not implemented 
			break;


		case 0x21:	                  //lh
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				cout << "0x" << hex << pc << "\tlh\t$"<< decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				regs[rt]	= memory[address - 0x10010000]&0xFFFF;		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

		case 0x04:	//beq
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				bAdd		= (imm & 0x4000) ? (0xFFFC0000 | (imm*4)): (0x00000000 | (imm*4) );
				if(regs[rs]==regs[rt]) pc=pc+4+bAdd;
				cout << "0x" << hex << pc << "\tbeq\t$" << decodeReg(rs) << ", $" << decodeReg(rt) << ", " <<dec<< sImm << "\n" ;
				//regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

		case 0x05:	//bne
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				bAdd		= (imm &0x4000)? ( 0xFFFC0000 | (imm*4) ): ( 0x00000000 | (imm*4) );
				if(regs[rs]!=regs[rt]) pc=pc+4+bAdd;
				cout << "0x" << hex << pc << "\tbne\t$" << decodeReg(rs) << ", $" << decodeReg(rt) << ", " << dec<<sImm << "\n" ;
				//regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

		case 0x0A:	//slti
			sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				cout << "0x" << hex << pc << "\tslti\t$" << decodeReg(rt) << ", $" << decodeReg(rs) << ", " <<dec<< sImm << "\n" ;
		//
			regs[rd]=(regs[rs]<sImm)? 1:0;
			break;

		case 0x0F:	//lui
			sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
			
			if(rt == 1) {
				pseudo = true;
			} 
			else {
				cout << "0x" << hex << pc << "\tlui\t$" <<decodeReg(rt)<< ",  0x" << hex<<sImm << "\n" ;
			}
			
			regs[rt]=imm*65536; //regs[rt]={imm,16'b0)
			break;

		default:
			cout << "0x" << hex << pc << "\tUnkown I-Format Instruction \n";
	
	}
	
	cout << "$" << decodeReg(rt) << " : 0x" << hex << regs[rt] << endl;
}

void r_format(unsigned int instWord)
{
	//R-Format
	func  = instWord & 0x3F;
	shamt = (instWord>>6) & 0x1f;
	rd    = (instWord>>11) & 0x1f;
	rt    = (instWord>>16) & 0x1f;
	rs    = (instWord>>21) & 0x1f;
	string rS,rT,rD;			
	
	switch(func){
		//add		
		case 0x20: 
			cout << "0x" << hex << pc << "\tadd\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
		
			regs[rd]=regs[rs]+regs[rt];	
			break;

		//addu
		case 0x21: 
			if(rs == 0) // pseudo : move
			{
				cout << "0x" << hex << pc << "\tmove\t$" <<decodeReg(rd)<< ", $" << decodeReg(rt)<< "\n" ;
			}
			else
			{
				cout << "0x" << hex << pc << "\taddu\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
			}
			
		
			regs[rd]=regs[rs]+regs[rt];	
			break;
	
		//sub
		case 0x22: 
			if(pseudo) {
				pseudo = false;
				cout << "0x" << hex << pc << "\tsubi\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", " <<dec<< regs[rt] << "\n" ;			
			}
			else {
				cout << "0x" << hex << pc << "\tsub\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;			
			}
			
			regs[rd]=regs[rs]-regs[rt];	
			break;

		//and
		case 0x24: cout << "0x" << hex << pc << "\tand\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
		
			regs[rd]=regs[rs]&regs[rt];	
			break;

				//or
		case 0x25: cout << "0x" << hex << pc << "\tor\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
		
			regs[rd]=regs[rs]|regs[rt];	
			break;

				//xor
		case 0x26: cout << "0x" << hex << pc << "\txor\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
		
			regs[rd]=regs[rs]^regs[rt];	
			break;

				//slt
		case 0x2A: cout << "0x" << hex << pc << "\tslt\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
		
			regs[rd]=(regs[rs]<regs[rt])? 1:0;	
			break;

				//srl
		case 0x02: cout << "0x" << hex << pc << "\tsrl\t$" <<decodeReg(rd) << ", $" << decodeReg(rt) << ", " <<dec<< shamt << "\n" ;
		
			regs[rd]=regs[rt]>>shamt;	
			break;

				//sll
		case 0x00: cout << "0x" << hex << pc << "\tsll\t$" <<decodeReg(rd) << ", $" << decodeReg(rt) << ", " <<dec<< shamt << "\n" ;
		
			regs[rd]=regs[rt]<<shamt;	
			break;


				//jr
		case 0x08: cout << "0x" << hex << pc << "\tjr\t$" << decodeReg(rs) << "\n" ;
		
			pc=regs[rs];	
			break;

				//syscall
		case 0x0C: cout << "0x" << hex << pc << "\tsyscall\n" ;
			switch(regs[2]){
			case 1:
			case 4:
				cout<<endl;
				cout<<"\t"<<dec<<regs[4]<<endl;break;
			case 10: cout<<"The program ends"<<endl;break;		//exit(0);
			}
			break;

		default: cout << "Unkown R-Format Instruction \n";
	}

	cout << "$" << decodeReg(rt) << " : 0x" << hex << regs[rt] << endl;
}

void j_format(unsigned int instWord) {
	// J-Format (J & Jal)

	unsigned int Address  	= (instWord & 0x03FFFFFF);
	unsigned int Nadd;
	unsigned int add_2;


	switch(opcode) {
		
		case 0x02:	//j
			
		Nadd= ((pc+4) & 0xF0000000);  //Nadd= pc+4[31:28]
		add_2=(Nadd | (Address*4) );	//{ PC[31:28], address, 2’b0 }

		cout << "0x" << hex << pc << "\tj\t"<<"0x" << hex<< add_2 << "\n" ;
		pc=add_2;
		break;

		case 0x03:	//jAL
			
		Nadd= ((pc+4) & 0xF0000000);  //Nimm= pc+4[31:28]
		add_2=(Nadd | (Address*4) );	//{ PC[31:28], address, 2’b0 }

		cout << "0x" << hex << pc << "\tjal\t" <<"0x"<< hex<< add_2 << "\n" ;
		regs[31]=pc;
		pc=add_2;
		break;

		default:
		cout << "0x" << hex << pc << "\tUnkown J-Format Instruction \n";
				}
}

void decodeInst(unsigned int instWord)
{
	opcode = instWord >> 26;
	
	cout << "<-----" << opcode <<"\n";
	
	if(0 == (opcode))
		r_format(instWord);
 	else if( 	0!= opcode && 
			2!=opcode && 
			3!=opcode && 
			16!=opcode && 
			17!=opcode && 
			18!=opcode && 
			19!=opcode
		) 
		i_format(instWord);
 	else if(opcode==2 || opcode==3)
		j_format(instWord);
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