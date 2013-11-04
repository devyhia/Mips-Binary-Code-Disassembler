/*
        This is just a skeleton. It DOES NOT implement all the requirements. 
        It only recognized the "ADD" and the "LW" instructions and prints 
        "Unkown Instruction" for all other instructions!
*/



#include <iostream>
#include <fstream>
#include <sstream>
#include "stdlib.h"
#include <string>

using namespace std;

int regs[32]={0};
char memory[8*1024];    // only 8KB of data memory 
                        // Starts from address 0x10010000

stringstream exec_output;
stringstream decode_output;



bool pseudo = false; // for decoding MARS pseudo instructions
int *pseudo_changes;

string *labels;
int lbl_cnt = 0; // labels count
int lbl_idx = 0; // label index

int decode_idx = 0;

unsigned int *instructions;
int cnt;
int pc_idx = 0; // program counter index


// Making them global is better for more modularized/procedural programming
unsigned int rd, rs, rt, func, shamt, imm, opcode;
unsigned int address;
int sImm,zImm,bAdd;     // sign extension and zero extension and branch address
const unsigned int pc_start = 0x00400000 ;

void decodeInst(unsigned int instWord);

int pc()
{
	return pc_start + (pc_idx * 4);
}

void set_pc_by_address(int address)
{
	pc_idx = (address - pc_start) / 4;
}

int get_pc_by_address(int address)
{
	return (address - pc_start) / 4;
}

string get_label_by_address(int address) {
	lbl_idx = get_pc_by_address(address);
	
	if(labels[lbl_idx] == "") {
		stringstream s;
		s << "label" << ++lbl_cnt;
		labels[lbl_idx] = s.str();
	}
	
	return labels[lbl_idx];
}

string get_label_by_immediate(int immediate) { 
	lbl_idx = decode_idx + immediate + 1; // +1 because ( pc is += 4 | one step ahead )

	if(labels[lbl_idx] == "") {
		stringstream s;
		s << "label" << ++lbl_cnt;
		labels[lbl_idx] = s.str();
	}
	
	return labels[lbl_idx];
}

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

void decode_i_format(unsigned int instWord) {
	// I-Format
	rt    	= (instWord>>16) & 0x1f;
	rs    	= (instWord>>21) & 0x1f;
	imm	  	= (instWord & 0x0000FFFF);
	
	switch(opcode) 
	{
		case 0x23:	//lw
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs] + sImm;
				decode_output << "lw\t$" << decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
			break;

		case 0x2B:	//sw
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs] + sImm;
				decode_output << "sw\t$" << decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
			break;
		case 0x08:	//addi
			sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
			
			if(rt == 1) {
				pseudo = true;
				pseudo_changes[decode_idx] = 2;
			}
			else {
				decode_output << "addi\t$" <<decodeReg(rt) << ", $" << decodeReg(rs) << ", " <<dec<< sImm << "\n" ;
			}
			
			regs[rt]=regs[rs]+sImm;
			break;

			case 0x09:	//addiu
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				
				if(rs == 0) // pseudo : li
				{
					pseudo_changes[decode_idx] = 1;
					decode_output << "li\t$" <<decodeReg(rt) << ", " << dec << sImm << "\n" ;
				}
				else
				{
					decode_output << "addiu\t$" <<decodeReg(rt) << ", $" << decodeReg(rs) << ", " << dec<<sImm << "\n" ;
				}
				
				regs[rt]=regs[rs]+sImm;
			break;

			case 0x0C:	//andi
				zImm 	= 0x00000000 | imm;	// sign extending the immediate field
				decode_output << "andi\t$" <<decodeReg(rt) << ", $" << decodeReg(rs) << ",  0x" << hex<<zImm << "\n" ;

				regs[rt]=regs[rs]&zImm;
			break;

			case 0x0D:	//ori
				zImm 	= 0x00000000 | imm;	// sign extending the immediate field
				// cout << "Imm: 0x" << hex << imm << " zImm : 0x" << hex << zImm << " ored : 0x" <<hex << (regs[rs] | zImm) << endl;
				regs[rt]=regs[rs] | zImm;
				if(pseudo) {
					pseudo = false;
					decode_output << "la\t$" << decodeReg(rt) << ",  0x" << hex<< regs[rt] << "\n" ;
				} else {
					decode_output << "ori\t$" << decodeReg(rt) << ", $" << decodeReg(rs) << ",  0x" << hex<< zImm << "\n" ;
				}
			break;

			case 0x0E:	//xori
				zImm 	= 0x00000000 | imm;	// sign extending the immediate field
				decode_output << "xori\t$" << decodeReg(rt) << ", $" << decodeReg(rs) << ",  0x" << hex<<zImm << "\n" ;

				regs[rt]=regs[rs]^zImm;
			break;

			case 0x28:	//sb
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				decode_output << "sb\t$" << decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
			break;

			case 0x29:	//sh
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				decode_output << "sh\t$"<< decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
			break;

		case 0x20:	//lb
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				decode_output << "lb\t$"<< decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
			break;


		case 0x21:	                  //lh
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				decode_output << "lh\t$"<< decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
			break;

		case 0x04:	//beq
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				bAdd		= (imm & 0x4000) ? (0xFFFC0000 | (imm*4)): (0x00000000 | (imm*4) );

				decode_output << "beq\t$" << decodeReg(rs) << ", $" << decodeReg(rt) << ", " << get_label_by_immediate(sImm) << "\n" ;
				//regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

		case 0x05:	//bne
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				bAdd		= (imm &0x4000)? ( 0xFFFC0000 | (imm*4) ): ( 0x00000000 | (imm*4) );

				decode_output << "bne\t$" << decodeReg(rs) << ", $" << decodeReg(rt) << ", " << get_label_by_immediate(sImm) << "\n" ;
				//regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

		case 0x0A:	//slti
			sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
			decode_output << "slti\t$" << decodeReg(rt) << ", $" << decodeReg(rs) << ", " <<dec<< sImm << "\n" ;
			
			regs[rd]=(regs[rs]<sImm)? 1:0;
			break;

		case 0x0F:	//lui
			sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
			
			if(rt == 1) {
				pseudo = true;
				pseudo_changes[decode_idx] = 2;
			} 
			else {
				decode_output << "lui\t$" <<decodeReg(rt)<< ",  0x" << hex<<sImm << "\n" ;
			}
			
			regs[rt]=imm*65536; //regs[rt]={imm,16'b0)
			break;

		default:
			decode_output << "Unkown I-Format Instruction \n";
	}
}

void decode_r_format(unsigned int instWord)
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
                        decode_output << "add\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                
                        regs[rd]=regs[rs]+regs[rt];     
                        break;

                //addu
                case 0x21: 
                        if(rs == 0) // pseudo : move
                        {
								pseudo_changes[decode_idx] = 1;
                                decode_output << "move\t$" <<decodeReg(rd)<< ", $" << decodeReg(rt)<< "\n" ;
                        }
                        else
                        {
                                decode_output << "addu\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                        }
                        
                
                        regs[rd]=regs[rs]+regs[rt];     
                        break;
        
                //sub
                case 0x22: 
                        if(pseudo) {
                                pseudo = false;
                                decode_output << "subi\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", " <<dec<< regs[rt] << "\n" ;                    
                        }
                        else {
                                decode_output << "sub\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;                     
                        }
                        
                        regs[rd]=regs[rs]-regs[rt];     
                        break;

                //and
                case 0x24: decode_output << "and\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                
                        regs[rd]=regs[rs]&regs[rt];     
                        break;

                                //or
                case 0x25: decode_output << "or\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                
                        regs[rd]=regs[rs]|regs[rt];     
                        break;

                                //xor
                case 0x26: decode_output << "xor\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                
                        regs[rd]=regs[rs]^regs[rt];     
                        break;

                                //slt
                case 0x2A: decode_output << "slt\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                
                        regs[rd]=(regs[rs]<regs[rt])? 1:0;      
                        break;

                                //srl
                case 0x02: decode_output << "srl\t$" <<decodeReg(rd) << ", $" << decodeReg(rt) << ", " <<dec<< shamt << "\n" ;
                
                        regs[rd]=regs[rt]>>shamt;       
                        break;

                                //sll
                case 0x00: decode_output << "sll\t$" <<decodeReg(rd) << ", $" << decodeReg(rt) << ", " <<dec<< shamt << "\n" ;
                
                        regs[rd]=regs[rt]<<shamt;       
                        break;


                                //jr
                case 0x08: decode_output << "jr\t$" << decodeReg(rs) << "\n" ;
                
                        
						set_pc_by_address(regs[rs]);    
                        break;

                                //syscall
                case 0x0C: 
					decode_output << "syscall\n" ;
					break;
				}
}

void decode_j_format(unsigned int instWord) {
        // J-Format (J & Jal)

        unsigned int Address    = (instWord & 0x03FFFFFF);
        unsigned int Nadd;
        unsigned int add_2;


        switch(opcode) {
                
                case 0x02:      //j
                        
                Nadd= ((pc()+4) & 0xF0000000);  //Nadd= pc+4[31:28]
                add_2=(Nadd | (Address*4) );    //{ PC[31:28], address, 2�b0 }
				
				decode_output << "j\t"<< get_label_by_address(add_2) << "\n" ;
                break;

                case 0x03:      //jAL
                        
                Nadd= ((pc()+4) & 0xF0000000);  //Nimm= pc+4[31:28]
                add_2=(Nadd | (Address*4) );    //{ PC[31:28], address, 2�b0 }
				
                decode_output << "jal\t" << get_label_by_address(add_2) << "\n" ;
                break;

                default:
                decode_output << "Unkown J-Format Instruction \n";
			}
}

void decodeInst(unsigned int instWord)
{
        opcode = instWord >> 26;
        if(0 == (opcode))
                decode_r_format(instWord);
        else if(        0!= opcode && 
                        2!=opcode && 
                        3!=opcode && 
                        16!=opcode && 
                        17!=opcode && 
                        18!=opcode && 
                        19!=opcode
                ) 
                decode_i_format(instWord);
        else if(opcode==2 || opcode==3)
                decode_j_format(instWord);
        else {
                decode_output << "Opcode: "<< opcode << " - Unkown Instruction \n";
        }
}

void execute_i_format(unsigned int instWord) {
	// I-Format
	rt    	= (instWord>>16) & 0x1f;
	rs    	= (instWord>>21) & 0x1f;
	imm	  	= (instWord & 0x0000FFFF);
	
	switch(opcode) 
	{
		case 0x23:	//lw
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs] + sImm;
				cout << "0x" << hex << pc() << "\tlw\t$" << decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				regs[rt]	= *(int*)&memory[address - 0x10010000];		// uncommenting this would crash the program as other instructions are not implemented 
			break;

		case 0x2B:	//sw
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs] + sImm;
				cout << "0x" << hex << pc() << "\tsw\t$" << decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				*(int*)&memory[address - 0x10010000]= regs[rt];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;
		case 0x08:	//addi
			sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
			
			cout << "0x" << hex << pc() << "\taddi\t$" <<decodeReg(rt) << ", $" << decodeReg(rs) << ", " <<dec<< sImm << "\n" ;
			
			regs[rt]=regs[rs]+sImm;
			break;

			case 0x09:	//addiu
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				cout << "0x" << hex << pc() << "\taddiu\t$" <<decodeReg(rt) << ", $" << decodeReg(rs) << ", " << dec<<sImm << "\n" ;
				regs[rt]=regs[rs]+sImm;
			break;

			case 0x0C:	//andi
				zImm 	= 0x00000000 | imm;	// sign extending the immediate field
				cout << "0x" << hex << pc() << "\tandi\t$" <<decodeReg(rt) << ", $" << decodeReg(rs) << ",  0x" << hex<<zImm << "\n" ;
				regs[rt]=regs[rs]&zImm;
			break;

			case 0x0D:	//ori
				zImm 	= 0x00000000 | imm;	// sign extending the immediate field
				// cout << "Imm: 0x" << hex << imm << " zImm : 0x" << hex << zImm << " ored : 0x" <<hex << (regs[rs] | zImm) << endl;
				regs[rt]=regs[rs] | zImm;
				cout << "0x" << hex << pc() << "\tori\t$" << decodeReg(rt) << ", $" << decodeReg(rs) << ",  0x" << hex<< zImm << "\n" ;
			break;

			case 0x0E:	//xori
				zImm 	= 0x00000000 | imm;	// sign extending the immediate field
				cout << "0x" << hex << pc() << "\txori\t$" << decodeReg(rt) << ", $" << decodeReg(rs) << ",  0x" << hex<<zImm << "\n" ;
				regs[rt]=regs[rs]^zImm;
			break;

			case 0x28:	//sb
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				cout << "0x" << hex << pc() << "\tsb\t$" << decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				memory[address - 0x10010000]=regs[rt];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

			case 0x29:	//sh
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				cout << "0x" << hex << pc() << "\tsh\t$"<< decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				*(short*)&memory[address - 0x10010000]=regs[rt];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

		case 0x20:	//lb
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				cout << "0x" << hex << pc() << "\tlb\t$"<< decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				regs[rt]	= memory[address - 0x10010000]&0xFF;		// uncommenting this would crash the program as othe instructions are not implemented 
			break;


		case 0x21:	      //lh
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				address 	= regs[rs]	+ sImm;
				cout << "0x" << hex << pc() << "\tlh\t$"<< decodeReg(rt) << "," << sImm << "($" << decodeReg(rs) << ")\n";
				regs[rt]	= *(short*)&memory[address - 0x10010000]&0xFFFF;		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

		case 0x04:	//beq
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				bAdd		= (imm & 0x4000) ? (0xFFFC0000 | (imm*4)): (0x00000000 | (imm*4) );
				cout << "0x" << hex << pc() << "\tbeq\t$" << decodeReg(rs) << ", $" << decodeReg(rt) << ", " <<dec<< sImm << "\n";
				if(regs[rs]==regs[rt]) pc_idx += sImm;
				//regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

		case 0x05:	//bne
				sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
				bAdd		= (imm &0x4000)? ( 0xFFFC0000 | (imm*4) ): ( 0x00000000 | (imm*4) );
				cout << "0x" << hex << pc() << "\tbne\t$" << decodeReg(rs) << ", $" << decodeReg(rt) << ", " << dec<<sImm << "\n" ;
				if(regs[rs]!=regs[rt]) pc_idx += sImm;
				//regs[rt]	= memory[address - 0x10010000];		// uncommenting this would crash the program as othe instructions are not implemented 
			break;

		case 0x0A:	//slti
			sImm 		= (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
			cout << "0x" << hex << pc() << "\tslti\t$" << decodeReg(rt) << ", $" << decodeReg(rs) << ", " <<dec<< sImm << "\n" ;
			
			regs[rd]=(regs[rs]<sImm)? 1:0;
			break;

		case 0x0F:	//lui
			sImm = (imm & 0x8000) ? (0xFFFF0000 | imm): imm;	// sign extending the immediate field
			
			cout << "0x" << hex << pc() << "\tlui\t$" <<decodeReg(rt)<< ",  0x" << hex<<sImm << "\n" ;
			
			regs[rt]=imm*65536; //regs[rt]={imm,16'b0)
			break;

		default:
			cout << "0x" << hex << pc() << "\tUnkown I-Format Instruction \n";
	
	}
	
	cout << "[TRACE]\t\t" << "$" << decodeReg(rt) << " : 0x" << hex << regs[rt] << endl;
}

void execute_r_format(unsigned int instWord)
{
        //R-Format
        func  = instWord & 0x3F;
        shamt = (instWord>>6) & 0x1f;
        rd    = (instWord>>11) & 0x1f;
        rt    = (instWord>>16) & 0x1f;
        rs    = (instWord>>21) & 0x1f;
        string rS,rT,rD;    
        
		int idx = 0;
        
        switch(func){
                //add           
                case 0x20: 
                        cout << "0x" << hex << pc() << "\tadd\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                
                        regs[rd]=regs[rs]+regs[rt];     
                        break;

                //addu
                case 0x21: 
                        cout << "0x" << hex << pc() << "\taddu\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                        regs[rd]=regs[rs]+regs[rt];     
                        break;
        
                //sub
                case 0x22:                        
                        cout << "0x" << hex << pc() << "\tsub\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;                     
                        regs[rd]=regs[rs]-regs[rt];     
                        break;

                //and
                case 0x24: 
					cout << "0x" << hex << pc() << "\tand\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                    regs[rd]=regs[rs]&regs[rt];     
                    break;

                //or
                case 0x25: cout << "0x" << hex << pc() << "\tor\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                
                        regs[rd]=regs[rs]|regs[rt];     
                        break;

                                //xor
                case 0x26: cout << "0x" << hex << pc() << "\txor\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                
                        regs[rd]=regs[rs]^regs[rt];     
                        break;

                                //slt
                case 0x2A: cout << "0x" << hex << pc() << "\tslt\t$" <<decodeReg(rd)<< ", $" << decodeReg(rs) << ", $" << decodeReg(rt)<< "\n" ;
                
                        regs[rd]=(regs[rs]<regs[rt])? 1:0;      
                        break;

                                //srl
                case 0x02: cout << "0x" << hex << pc() << "\tsrl\t$" <<decodeReg(rd) << ", $" << decodeReg(rt) << ", " <<dec<< shamt << "\n" ;
                
                        regs[rd]=regs[rt]>>shamt;       
                        break;

                                //sll
                case 0x00: cout << "0x" << hex << pc() << "\tsll\t$" <<decodeReg(rd) << ", $" << decodeReg(rt) << ", " <<dec<< shamt << "\n" ;
                
                        regs[rd]=regs[rt]<<shamt;       
                        break;


                                //jr
                case 0x08: 
						cout << "0x" << hex << pc() << "\tjr\t$" << decodeReg(rs) << "\n" ;
                
						set_pc_by_address(regs[rs]);
                        break;

                                //syscall
                case 0x0C: 
					cout << "0x" << hex << pc() << "\tsyscall\n" ;
                        switch(regs[2]){
                        case 1:
                            exec_output << dec<<regs[4]<<endl;
							break;
                        case 4:
						idx = regs[4] - 0x10010000;
							while(memory[idx] != char(0)) {
                            	exec_output << (char)memory[idx++];
							}
							exec_output << endl;
							break;
						case 10:
							exec_output << "The End Of The Program"<<endl;
							break;
						}
                        break;

                default: 
					cout << "Unkown R-Format Instruction \n";
					break;
				
        }

       cout << "[TRACE]\t\t" << "$" << decodeReg(rd) << " : 0x" << hex << regs[rd] << endl;
}

void execute_j_format(unsigned int instWord) {
        // J-Format (J & Jal)
        unsigned int Address    = (instWord & 0x03FFFFFF);
        unsigned int Nadd;
        unsigned int add_2;

        switch(opcode) {
                
                case 0x02:      //j
                        
                Nadd= ((pc()+4) & 0xF0000000);  //Nadd= pc+4[31:28]
                add_2=(Nadd | (Address*4) );    //{ PC[31:28], address, 2�b0 }

                cout << "0x" << hex << pc() << "\tj\t"<<"0x" << hex<< add_2 << "\n" ;
                set_pc_by_address(add_2 - 4);
                break;

                case 0x03:      //jAL
 
                Nadd= ((pc()+4) & 0xF0000000);  //Nimm= pc+4[31:28]
                add_2=(Nadd | (Address*4) );    //{ PC[31:28], address, 2�b0 }

                cout << "0x" << hex << pc() << "\tjal\t" <<"0x"<< hex<< add_2 << "\n" ;
                regs[31]=pc();
                set_pc_by_address(add_2 - 4);
                break;

                default:
                cout << "0x" << hex << pc() << "\tUnkown J-Format Instruction \n";
           }
}

void executeInst()
{
	unsigned int instWord = instructions[pc_idx];
	opcode = instWord >> 26;
        
        cout << "[OPCODE]\t" << opcode <<"\n";
		cout << "[INSTRUCTION]\t";
        if(0 == (opcode))
                execute_r_format(instWord);
        else if(        0!= opcode && 
                        2!=opcode && 
                        3!=opcode && 
                        16!=opcode && 
                        17!=opcode && 
                        18!=opcode && 
                        19!=opcode
                ) 
                execute_i_format(instWord);
        else if(opcode==2 || opcode==3)
                execute_j_format(instWord);
        else {
                cout << "Opcode: "<< opcode << " - Unkown Instruction \n";
        }
		cout << endl;
}

int get_counting_coefficient(int pos) {
	int coef = 0;
	for(int i = 0; i < pos; i++) {
		if(pseudo_changes[i] > 1)
			coef -= pseudo_changes[i] - 1; 
	}
	return coef;
}

int main(int argc, char *argv[]){
	unsigned int instWord=0;
	ifstream inFile;
	ofstream outFile;
	
    bool decode_only = false;
    
	if(argc<2) emitError("use: mipsim <machine_code_file_name>\n");
	
	if(string(argv[1]) == "-h" || string(argv[1]) == "--help")
	{
		cout << string(65, '-') << endl << "Welcome to the MIPS Simulator!" << endl << string(65, '-') << endl;
		cout << "You can use the application in the two following formats: " << endl;
		cout << "* mipsim <machine_code_file_name>" << endl;
		cout << "* mipsim <machine_code_file_name> <data_binary_file_name>" << endl << string(65, '-') << endl;
		return 0;
	}
	
	if(argc == 3) // a dump memory ( the .data section of the application )
	{
		inFile.open(argv[2], ios::in | ios::binary);
		if(inFile.is_open())
		{
			for(int i = 0; inFile.read ((char *)& memory[i], 1); i++) {}
			inFile.close();
		}
		else {
			cout << "Couldn't access Data Binary file" << endl;
			return 0;
		}
	}
	inFile.open(argv[1], ios::in | ios::binary | ios::ate);
	
	if(inFile.is_open())
	{
		cnt = inFile.tellg() / 4;
		inFile.seekg(0);
		
		instructions = new unsigned int[cnt]();
		labels = new string[cnt]();
		labels[0] = "main"; // set the first of code to be the main function
		cout << "count : " << cnt << endl;
		pseudo_changes = new int[cnt]();
		
		decode_idx = 0;
		
		cout << "/// Decoding the binary file" << endl;
		
		while(true){
			if(!inFile.read ((char *)& instWord, 4)) break;
			decodeInst(instWord);
			instructions[decode_idx++] = instWord;
		}
		
		// avoid double-counting of pseudo instructions
		for(int i = 0; i < cnt ; i++) { 
			if(labels[i] != "") {
				int coef = get_counting_coefficient(i);
				if(coef < 0) {
					labels[i+coef] = labels[i];
					labels[i] = "";
				}
			}
		}

		decode_idx = 0;
		string line;
		for(;getline(decode_output, line); decode_idx++)
			cout << labels[decode_idx] << (labels[decode_idx] != "" ? ":" : "") << "\t" << line << endl;
		
        for(int i=0;i<32;i++) // clear the registers for execution
            regs[i] = 0;
        
        cout << endl << "/// [TRACE] Executing the binary file" << endl;
        pc_idx = 0;
        while(pc_idx < cnt) {
            executeInst();
            pc_idx++;
        }
            
        cout << endl << "/// Program Output" << endl;
        cout << exec_output.str() << endl;
        
        cout << endl << "/// Registers " << endl;
        for(int i=0;i<32;i++)
            cout<<dec<<i<<": \t0x"<<hex<< regs[i]<<"\n";
        
		// delete labels;
		// 	delete instructions;
	} else emitError("Cannot access input file\n");

	return 0;
}