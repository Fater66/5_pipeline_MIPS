#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.
#define ADDU 1
#define SUBU 0



struct IFStruct {
    //everytime PC+4;
    bitset<32>  PC;
    bool        nop;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
         rfout.open("/Users/fater/study/CSA/Lab2/Lab2/Lab2/RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
};

class ALU//use lab0
{
public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {
        unsigned int result;
        switch(ALUOP.to_ulong())
        {
                
            case ADDU : result = oprand1.to_ulong() + oprand2.to_ulong(); break;
            case SUBU : result = oprand1.to_ulong() - oprand2.to_ulong(); break;
        }
        
        return ALUresult = bitset<32>(result);
    }
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
			imem.open("/Users/fater/study/CSA/Lab2/Code-2018/Testbenches/testcase/2-2/imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(std::stoi(line,nullptr,2));
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("/Users/fater/study/CSA/Lab2/Code-2018/Testbenches/testcase/2-2/dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(std::stoi(line,nullptr,2));
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("/Users/fater/study/CSA/Lab2/Lab2/Lab2/dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  

unsigned long shiftbits(bitset<32> inst, int start)
{
//    unsigned long ulonginst;
    return ((inst.to_ulong())>>start);
    
}

bitset<32> signextend (bitset<16> imm)
{
    string sestring;
    if (imm[15]==0){
        sestring = "0000000000000000"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    else{
        sestring = "1111111111111111"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    return (bitset<32> (sestring));
    
}

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("/Users/fater/study/CSA/Lab2/Lab2/Lab2/stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}
 

int main()
{
    
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    ALU  myALU;
    
    int cycle = 0;
    bool stall = 0;
    //instruction part
    bitset<32> instruction;
    bitset<6> opcode;
    bitset<6> funct;
    bitset<16> imm;
    
    // RF signals
    bitset<5> RReg1;
    bitset<5> RReg2;
    bitset<5> WReg;
    
    // ALU signals
    bitset<3> ALUop;
    bitset<32> ALUin1;
    bitset<32> ALUin2;
    bitset<32> signext;
    bitset<32> ALUOut;

    //control signals
    bitset<1> IType;
    bitset<1> JType;
    bitset<1> RType;
    bitset<1> IsBranch;
    bitset<1> IsLoad;
    bitset<1> IsStore;
    bitset<1> WrtEnable;
    
    // pc signals
    bitset<32> pcplusfour;
    bitset<32> jaddr;
    bitset<32> braddr;
    bitset<1> IsEq;
    
    stateStruct state,newState;
    state.IF.nop = 0;
    state.ID.nop = 1;
    state.EX.nop = 1;
    state.MEM.nop = 1;
    state.WB.nop = 1;
    state.EX.is_I_type = 0;
    newState.EX.is_I_type = 0;
    state.EX.rd_mem = 0;
    newState.EX.rd_mem= 0;
    state.EX.wrt_mem = 0;
    newState.EX.wrt_mem = 0;
    state.EX.wrt_enable = 0;
    state.MEM.rd_mem = 0;
    newState.MEM.rd_mem = 0;
    state.MEM.wrt_mem = 0;
    newState.MEM.wrt_mem = 0;
    state.MEM.wrt_enable = 0;
    newState.MEM.wrt_enable = 0;
    newState.EX.wrt_enable = 0;
    newState.IF.nop = 0;
    state.WB.wrt_enable = 0;
    newState.WB.wrt_enable = 0;
    newState.ID.nop = 1;
    newState.EX.nop = 1;
    newState.MEM.nop = 1;
    newState.WB.nop = 1;
    newState.EX.alu_op = 1;
    
    while (1) {

        

        /* --------------------- WB stage --------------------- */
        if (!state.WB.nop)
        {
            if ( state.WB.wrt_enable)
            {
                myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
                if (state.WB.Wrt_reg_addr==state.EX.Rt)
                {
                    state.EX.Read_data2 = state.WB.Wrt_data;
                    cout<<"Mem-Mem forwarding to Ex.Rt"<<endl;
                }
                else if (state.WB.Wrt_reg_addr==state.EX.Rs)
                {
                    state.EX.Read_data1 = state.WB.Wrt_data;
                    cout<<"Mem-Mem forwarding to Ex.Rs"<<endl;
                }
            }
            
        }else{
        }

        /* --------------------- MEM stage --------------------- */
      
        if (!state.MEM.nop)
        {
            newState.WB.nop = false;
            newState.WB.Wrt_data = state.MEM.ALUresult;

            if (state.MEM.wrt_mem)
            {
                if (state.WB.wrt_enable && state.WB.Wrt_reg_addr==state.MEM.Rt)
                    state.MEM.Store_data = state.WB.Wrt_data;
                cout << "State.mem.aluresult = "<<state.MEM.ALUresult << endl;
                cout << "State.mem.storedata = "<<state.MEM.Store_data << endl;
                myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
            }
            if (state.MEM.rd_mem) newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
//            else cout<<"can't read and write at the same time"<< endl;
            newState.WB.wrt_enable = state.MEM.wrt_enable;
            newState.WB.Rs = state.MEM.Rs;
            newState.WB.Rt = state.MEM.Rt;
            newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;

        }else{
            newState.WB.nop = true;
        }

        /* --------------------- EX stage --------------------- */
        if (!state.EX.nop)
        {
            newState.MEM.nop = false;
            
            ALUin1 = state.EX.Read_data1;
            

            
            ALUin2 = (state.EX.is_I_type)? signextend(state.EX.Imm): state.EX.Read_data2;
//            //forwarding data
//            if (state.WB.wrt_enable && state.WB.Wrt_reg_addr==state.EX.Rs)
//                ALUin1 = state.WB.Wrt_data;
            if (state.MEM.wrt_enable && state.MEM.Wrt_reg_addr==state.EX.Rs && !state.MEM.rd_mem)
                ALUin1 = state.MEM.ALUresult;
//            if (state.WB.wrt_enable && state.WB.Wrt_reg_addr==state.EX.Rt&&state.EX.is_I_type)
//            {
//                 state.EX.Read_data2= state.WB.Wrt_data;
//                 cout<<"Mem-Mem forwarding"<<endl;
//            }
            
            if (state.MEM.wrt_enable && state.MEM.Wrt_reg_addr==state.EX.Rt && !state.MEM.rd_mem &&!state.EX.is_I_type )
                ALUin2 = state.MEM.ALUresult;
        
            
            ALUOut = myALU.ALUOperation(state.EX.alu_op,ALUin1,ALUin2);
            cout<<"Alu1 = "<<ALUin1.to_ulong()<<",Alu2 = "<< ALUin2.to_ulong()<<endl;
            cout<<ALUOut<<endl;
            cout<<myALU.ALUOperation(1,0,10)<<endl;
            newState.MEM.ALUresult = ALUOut;
            newState.MEM.Store_data =state.EX.Read_data2;
            
            newState.MEM.Rs = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;
            newState.MEM.wrt_enable = state.EX.wrt_enable;
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            newState.MEM.wrt_mem = state.EX.wrt_mem;
            newState.MEM.rd_mem = state.EX.rd_mem;
            

        }else{
            newState.MEM.nop = true;
        }
        
        /* --------------------- ID stage --------------------- */
        // processing information from EX stage in ID stage
        if (!state.ID.nop)
        {
            newState.EX.nop = false;
            opcode = bitset<6> (shiftbits(state.ID.Instr, 26));
            bitset<32> test = bitset<32>("00010000001000001111111111111100");
            cout<<"shiftbit"<<shiftbits(test, 26)<<endl;
            RType = (opcode.to_ulong()==0)?1:0;
            IType = (opcode.to_ulong()!=0 &&opcode.to_ulong()!=4)?1:0;
            cout<<"opcode="<<opcode<<endl;
            cout<<"Itype"<<IType<<endl;
            JType = (opcode.to_ulong()==2)?1:0;
            IsBranch = (opcode.to_ulong()==4)?1:0;
            IsLoad = (opcode.to_ulong()==35)?1:0;//lw instruction
            IsStore = (opcode.to_ulong()==43)?1:0;
            WrtEnable = (IsStore.to_ulong() || IsBranch.to_ulong() || JType.to_ulong())?0:1;
            
            funct = bitset<6> (shiftbits(instruction, 0));
            RReg1 = bitset<5> (shiftbits(state.ID.Instr, 21));
            RReg2 = bitset<5> (shiftbits(state.ID.Instr, 16));
            ALUop = (opcode.to_ulong()==35 || opcode.to_ulong()==43)?(bitset<3>(string("001"))):bitset<3> (shiftbits(state.ID.Instr, 0));
            imm = bitset<16> (shiftbits(state.ID.Instr, 0));

            WReg =  (IType.to_ulong())? RReg2 : bitset<5> (shiftbits(state.ID.Instr, 11));
            
            //ReadData1,ReadData2
            newState.EX.Read_data1 = myRF.readRF(RReg1);
            newState.EX.Read_data2 = myRF.readRF(RReg2);
            //imm
            newState.EX.Imm = imm;
            //Rs,Rt
            newState.EX.Rs = RReg1;
            newState.EX.Rt = RReg2;

            //Wrt_reg_addr
            if (RType.to_ulong())//if R-type instruction
            {
                newState.EX.Wrt_reg_addr = WReg;//[15:11]
            }
            else {
                newState.EX.Wrt_reg_addr = RReg2;//Doesnt matter
//                cout<<"Not R-type Instruction"<<endl;
            }
            //ALUop Set for addu, lw, sw; unset for subu
            if (ALUop.to_ulong() != 3)
            {
                newState.EX.alu_op = 1;
                cout<<"set aluop to "<<ALUop.to_ulong()<<endl;
            }else newState.EX.alu_op = 0;
            //is_I_type
            if(IType.to_ulong())
            {
                newState.EX.is_I_type = 1;
            }else newState.EX.is_I_type = 0;
            //write_enable
            if (IsStore.to_ulong() || IsBranch.to_ulong() || JType.to_ulong())
                newState.EX.wrt_enable = 0;
            else
                newState.EX.wrt_enable = 1;
            //rd_mem wrt_mem
            newState.EX.rd_mem =(IsLoad.to_ulong())? 1:0;//lw instruction
            newState.EX.wrt_mem =(IsStore.to_ulong())? 1:0;
            
            //branch instruction
            //beq changes to bne
            signext = signextend (newState.EX.Imm);
            IsEq = (newState.EX.Read_data1 != newState.EX.Read_data2)?1:0;
            braddr = bitset<32>(
                                state.IF.PC.to_ulong()
                                + (bitset<32>((bitset<30> (shiftbits(signext,0))).to_string<char,std::string::traits_type,std::string::allocator_type>()+"00")).to_ulong() );
//            //
//            if (IsBranch.to_ulong() && IsEq.to_ulong())
//            {
//                cout<<newState.IF.PC<<endl;
//                cout<<"signextend"<<signext<<endl;
//                newState.IF.PC = braddr;
//                cout<<"跳转到:"<<braddr<<endl;
//                newState.ID.nop = 1;
//            }
//            //stall
            if (!stall)
            {
                if (!newState.EX.wrt_mem)
                {
                    stall =newState.MEM.rd_mem && (newState.MEM.Wrt_reg_addr == newState.EX.Rt||newState.MEM.Wrt_reg_addr == newState.EX.Rs);
                }
                else{
                    stall =newState.MEM.rd_mem && newState.MEM.Wrt_reg_addr == newState.EX.Rs;
                }
                cout<<"stall ="<<stall<<endl;
                if(stall){
                    newState.ID = state.ID;
                    newState.EX.nop =1;
                }
            }else {
                stall = 0;
                newState.EX.nop =0;
            }
 
            
        }else{
            newState.EX.nop = true;
        }

        
        /* --------------------- IF stage --------------------- */
        if (!state.IF.nop)
        {
                if (!stall)
                {
                    instruction = myInsMem.readInstr(state.IF.PC);//read from address pc
                    cout<<"current instruction="<<instruction<<endl;
                    //check if it is halt
                    if(instruction.to_string<char,std::string::traits_type,std::string::allocator_type>()=="11111111111111111111111111111111"){
                        newState.IF.nop = 1;
                        newState.ID.Instr =instruction;
                        newState.ID.nop = 1;
                    }
                    else
                    {
                        pcplusfour = bitset<32> (state.IF.PC.to_ulong()+4);
                        cout<<"current PC="<<pcplusfour.to_ulong()<<endl;
                        
                        newState.IF.PC = pcplusfour;
                        newState.ID.nop = false;
                        newState.ID.Instr =instruction;
                        
                        if (IsBranch.to_ulong() && IsEq.to_ulong())
                        {
                            IsBranch = 0;
                            cout<<newState.IF.PC<<endl;
                            cout<<"signextend"<<signext<<endl;
                            newState.IF.PC = braddr;
                            cout<<"jumpt to ->"<<braddr<<endl;
                            cout<<"current branch pc ="<<newState.IF.PC.to_ulong()<<endl;
//                            newState.ID.Instr =myInsMem.readInstr(braddr);
                            newState.ID.nop = 1;
                        }

                        

                        //instruction fetch

                    }
                }
                else{
                    newState.IF = state.IF;
                }
            
        }else
        {
            newState.ID.nop = 1;
        }
        
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
        cout<<"current cycle = "<< cycle<<endl;
        cout<<endl;
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */ 
        cycle++;
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}
