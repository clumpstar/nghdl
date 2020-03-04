/* This C Code for ATTINY series (specifically ATTINY85) was developed by ASHUTOSH JHA

   Latest edit - 3:22 AM, 4/3/2020

   NOTE :- The function MapToRam and output are linked to the VHDL code of ATTINY85
   		   by "ghdl_access.vhdl" file.	*/ 

#include<stdio.h>
#include<math.h>
#define size 8192		//8kb ram size for attiny 85

int debugMode=1;
int PB0,PB1,PB2,PB3,PB4,PB5;
char chg_Pc=0;
struct memory			//Structure to store RAM and other registers
{
	unsigned char data;
}ram[size],reg[32],SREG[8],PORTB,DDRB;

/* SREG MAP :- 

| C | Z | N | V | S | H | T | I |
  0   1   2   3   4   5   6   7 

*/

struct BinArrays
{
    int arr[8];
}bin[3];

int * get_ptr0() {
  return &PB0;
}

int * get_ptr1() {
  return &PB1;
}

int * get_ptr2() {
  return &PB2;
}

int * get_ptr3() {
  return &PB3;
}

int * get_ptr4() {
  return &PB4;
}

int * get_ptr5() {
  return &PB5;
}


void ClearBins(int binSel)
{
    int i;
        for(i=0;i<8;i++)
    	{
        	bin[binSel].arr[i]=0;
    	}
}

/*void Change_PC(char op, char val)
{
	if(op==45 && pc < val)		// ASCI of '-' is 45
		printf("\n*****PC value went negative*****\n");
	else if(op==43 && pc==0xFF)		// ASCI of '+' is 43
		printf("\n*****PC value went above RAM size*****\n");
	else
		{
			if(op==43)		// ASCI of '+' is 43	
				pc += val;
			else if(op==45)		// ASCI of '-' is 45
				pc -= val;
		}
}*/

void SetRam(int lb, int ub,char val)
{
	int i;
	for(i=lb;i<ub;i++)
		ram[i].data = val;
}

void PrintSREG()
{
	printf("\nStatus Register:- \n");
	printf("I: %d ,T: %d ,H: %d ,S: %d ,V: %d ,N: %d ,Z: %d ,C: %d \n",SREG[7].data,SREG[6].data,SREG[5].data,SREG[4].data,SREG[3].data,SREG[2].data,SREG[1].data,SREG[0].data);

}

void PrintRam(int lb, int ub)
{
	int i=0;
	unsigned char b1,b2,b3,b4;
	printf("\n***********RAM*************\n");
	for(i=lb;i<ub;i+=4)
		{
			b1=ram[i+0x2].data,b2=ram[i+0x3].data,b3=ram[i].data,b4=ram[i+0x1].data;
			printf("instruction %X: %X%X%X%X\n",i,b1,b2,b3,b4);
		}
	printf("\n************************\n");
}

void PrintReg(int lb, int ub)
{
	int i;
	printf("\n***********Register File*************\n");
	for(i=lb;i<ub;i++)
		printf("R[%d]: %X\n",i,reg[i].data);
	printf("\n************************\n");
}

int Rjump_Cal(char b2,char b3,char b4)
{
	int bin[16]={0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},i;
	i=8;
	while(b2!=0 && i<12)
	{
		bin[i] = b2%2;
		i++;
		b2 /= 2;
	}
	i=4;
	while(b3!=0 && i<8)
	{
		bin[i] = b3%2;
		i++;
		b3 /= 2;
	}
	i=0;
	while(b4!=0 && i<4)
	{
		bin[i] = b4%2;
		i++;
		b4 /= 2;
	}
    
	for(i=0;i<16;i++)
		   bin[i] = !bin[i];

	int a=0;
	for(i=0;i<16;i++)
	{
		a += bin[i]*pow(2,i);
	}
	return a;
}

void Hex2Bin(int binSel,int hex)			//Function to convert hex number to binary array
{
        int i=0,a,b,t=hex;

	    while(hex!=0 && i<8)
	    {
	        bin[binSel].arr[i] = hex % 2;
	        i++;
	        hex /= 2;
	    }
	    if(t > 15)
	    {
	        int t0=bin[binSel].arr[0],t1=bin[binSel].arr[1],t2=bin[binSel].arr[2],t3=bin[binSel].arr[3];
	        for(i=0;i<4;i++)
	        {
	            bin[binSel].arr[i]=bin[binSel].arr[4+i];
	        }

	        bin[binSel].arr[4]=t0;
	        bin[binSel].arr[5]=t1;
	        bin[binSel].arr[6]=t2;
	        bin[binSel].arr[7]=t3;
	    }
}

void TwosComp(int binSel)			//Function to get 2's complement
{

    int i,t,carry=0;
    for(i=0;i<8;i++)
    bin[binSel].arr[i] = !bin[binSel].arr[i];

    for(i=0;i<8;i++)
    {
        if(i==0)
            {
                t = carry + bin[binSel].arr[i] + 1;
                bin[binSel].arr[i] = bin[binSel].arr[i] ^ carry ^ 1;
            }
        else
            {
                t = carry + bin[binSel].arr[i];
                bin[binSel].arr[i] = bin[binSel].arr[i] ^ carry;
            }
        if(t<=1)
            carry = 0;
        else
            carry = 1;
    }
    
}

void Bin_Add(int a, int b, int c,int select,int withCarry)			//Function to perform binary addition
{
    int i,t;
    for(i=0;i<8;i++)
	    {
	    	if(i==0 && withCarry == 0)
	    	{
	    		t = bin[b].arr[i] + bin[a].arr[i];
	        	bin[c].arr[i] = bin[a].arr[i] ^ bin[b].arr[i];	
	    	}
	    	else
	    	{
	    		t = bin[b].arr[i] + bin[a].arr[i] + SREG[0].data;
	        	bin[c].arr[i] = bin[a].arr[i] ^ bin[b].arr[i] ^ SREG[0].data;
	    	}
	        
	        if(t<=1)
	        {
	            SREG[0].data = 0;
	            //printf("\nCarry bit is set\n");
	        }
	        else
	        {
	        	SREG[0].data = 1;
	        	//printf("\nCarry bit is set\n");
	        	if(i==3)
	        		{
	        			SREG[5].data = 1;
	        			//printf("\nHalf Carry bit is set\n");
	        		}
	        	else if(i==7)
	        		{
	        			SREG[2].data = 0;
	        			//printf("\nNegative bit is reset\n");
	        		}
	        }
	    }
	    
	int t0=bin[c].arr[0],t1=bin[c].arr[1],t2=bin[c].arr[2],t3=bin[c].arr[3];
	for(i=0;i<4;i++)
	{
	    bin[c].arr[i]=bin[c].arr[4+i];
	}

	bin[c].arr[4]=t0;
	bin[c].arr[5]=t1;
	bin[c].arr[6]=t2;
	bin[c].arr[7]=t3;
	    
	    if(select == 1)
	    {
	        SREG[5].data = !SREG[5].data;
	        SREG[2].data = !SREG[0].data;
	        SREG[0].data = !SREG[0].data;
	    }
}



void SetPins(char val)			//Function to set/reset the I/O pins
{
	int bin[6]={0,0,0,0,0,0},i=0; 
    while(val!=0)
    {
        bin[i] = val % 2;
        i++;
        val /= 2;
    }

    PB0 = bin[0];
    PB1 = bin[1];
    PB2 = bin[2];
    PB3 = bin[3];
    PB4 = bin[4];
    PB5 = bin[5];

}

void MapToRam(int flag)				//Function to map the external hex file contents into this C code
{
	int i=0,filesize;
	SetRam(0,size,0x0);
	if(flag==1)
	{
		FILE *fptr;
		unsigned char c;
		fptr = fopen("hex.txt", "r");

		fseek(fptr, 0L, SEEK_END);
	    filesize = ftell(fptr);

		rewind(fptr);
		c = fgetc(fptr); 
	    while (c != EOF && i<filesize) 
	    {
	    	// to skip newline character in file
	    	if(c == '\n')
	    		c = fgetc(fptr);
	    	// to skip ":" character in file
	    	else if(c == ':')
	    		c = fgetc(fptr);
	    	else if(c >= 127)
	    		c = 0;

	    	// the ascii eqivalent of char c is converted to hex
	    	if(c>=48 && c<=57)
	        	c-=48;
	    	else if(c>=65 && c<=70)
	       		c-=55;
	    	else if(c>=97 && c<=102)
	        	c-=87;
	        ram[i].data = c;
	        i++;
	        c = fgetc(fptr); 
	    }
	    fclose(fptr);
	}
	for(i=0;i<8;i++)
		SREG[i].data = 0;

	PrintRam(0,filesize+5);
}

void UpdateSreg()			//Function to update Zero and Signed bit of SREG
{
	int i,t=0;
	for(i=0;i<8;i++)
	{
	    if(bin[2].arr[i]==0)
	        	t++;
	}
	if(t == 8)
	{
	    SREG[1].data = 1;
	    //printf("\nZero bit is set\n");	
	}
	else
	{
	    SREG[1].data = 0;
	    //printf("\nZero bit is set\n");	
	}

	SREG[4].data = SREG[2].data ^ SREG[3].data;
	//Signed bit is exor of Negative and Overflow bits
	if(SREG[4].data == 1)
	{
		//printf("\nSigned bit is set\n");
	}
	else
	{
		//printf("\nSigned bit is reset\n");
	}

}

void Compute(char pc)			//Function that performs main computation based on current instruction
{
	int i,j,t;
	unsigned b1=ram[pc+0x2].data,b2=ram[pc+0x3].data,b3=ram[pc].data,b4=ram[pc+0x1].data;
	printf("instruction:%X%X%X%X\n",b1,b2,b3,b4);

	if(b1==0x0 && b2>=12 && b2<=15)					//ADD
	{
		printf("ADD instruction decoded\n");
		int a=reg[b3+16].data,b=reg[b4+16].data;
		PrintReg(15,32);

		ClearBins(0);
		Hex2Bin(0,a); 
		ClearBins(1);
		Hex2Bin(1,b);
		ClearBins(2);

		Bin_Add(0,1,2,0,0);

	    UpdateSreg();

		reg[b3+16].data += reg[b4+16].data;

		printf("\nAfter Operation - \n");
		PrintReg(15,32);
		chg_Pc += 0x4;
	}

/************************************************************************************************/	

	else if(b1==0x1 && b2>=12 && b2<=15)			//ADC
	{
		printf("ADC instruction decoded\n");
		int a=reg[b3+16].data,b=reg[b4+16].data;
		PrintReg(15,32);

		ClearBins(0);
		Hex2Bin(0,a); 
		ClearBins(1);
		Hex2Bin(1,b);
		ClearBins(2);

		Bin_Add(0,1,2,0,1);

	    UpdateSreg();

		reg[b3+16].data += reg[b4+16].data + SREG[0].data;

		printf("\nAfter Operation - \n");
		PrintReg(15,32);
		chg_Pc += 0x4;
	}

/************************************************************************************************/

	else if(b1==0x1 && b2 >= 8 && b2 <= 11)			//SUB
	{
		printf("SUB instruction decoded\n");
		int a=reg[b3+16].data,b=reg[b4+16].data;
		PrintReg(15,32);
		ClearBins(0);
		Hex2Bin(0,a); 
		ClearBins(1);
		Hex2Bin(1,b);
		TwosComp(1);
		ClearBins(2);

		Bin_Add(0,1,2,1,0);

		UpdateSreg();

		reg[b3+16].data -= reg[b4+16].data;
		printf("\nAfter Operation - \n");
		PrintReg(15,32);
		chg_Pc += 0x4;
	}

/************************************************************************************************/

	else if(b1==0x5)								//SUBI
	{
		printf("SUBI instruction decoded\n");
		int a=b2*16 + b4,b=reg[b3+16].data;
		PrintReg(15,32);

		ClearBins(0);
		Hex2Bin(0,a); 
		ClearBins(1);
		Hex2Bin(1,b);
		TwosComp(0);
		ClearBins(2);

		Bin_Add(1,0,2,1,1);

		UpdateSreg();

		reg[b3+16].data -= a;
		printf("\nAfter Operation - \n");
		PrintReg(15,32);
		chg_Pc += 0x4;

	}

/************************************************************************************************/

	else if(b1==0x4)								//SBCI
	{
		printf("SBCI instruction decoded\n");
		int a=b2*16 + b4,b=reg[b3+16].data;
		PrintReg(15,32);

		ClearBins(0);
		Hex2Bin(0,a); 
		ClearBins(1);
		Hex2Bin(1,b);
		TwosComp(0);
		ClearBins(2);

		Bin_Add(1,0,2,1,1);

		UpdateSreg();

		reg[b3+16].data -= a - SREG[0].data;
		printf("\nAfter Operation - \n");
		PrintReg(15,32);
		chg_Pc += 0x4;
	}

/************************************************************************************************/

	else if(b1==0xE)								//LDI
	{
		printf("LDI instruction decoded\n");
		reg[b3+16].data = b2*16 + b4;
		chg_Pc += 0x4;
	}

/************************************************************************************************/	

	else if(b1==0xB && b2==0xB)						//OUT
	{
		printf("OUT instruction decoded\n");
		if(b4==0x8)									//Setting PORTB out pins
			SetPins(reg[b3+16].data);				//Setting DDRB
		chg_Pc += 0x4;
	}

/************************************************************************************************/	

	else if(b1==0xC)								//RJMP
	{
		int bin[16]={0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},carr;
		int temp[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    	int temp2[16]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    	char jump;
		printf("RJMP instruction decoded");

		i=8;
		while(b2!=0 && i<12)
		{
			bin[i] = b2%2;
			i++;
			b2 /= 2;
		}
		i=4;
		while(b3!=0 && i<8)
		{
			bin[i] = b3%2;
			i++;
			b3 /= 2;
		}
		i=0;
		while(b4!=0 && i<4)
		{
			bin[i] = b4%2;
			i++;
			b4 /= 2;
		}
	    
		for(i=0;i<16;i++)
		{
			bin[i] = !bin[i];
			temp[i] = bin[i];
		}

		t=0;
		carr=0;
		for(i=0;i<16;i++)
	    	{
		        t = temp[i] + temp2[i] + carr;
		        bin[i] = temp[i] ^ temp2[i] ^ carr;
		        if(t<=1)
		        carr = 0;
		        else
		        carr = 1;
		    }

		j=0;
		for(i=0;i<16;i++)
		{
			j += bin[i]*pow(2,i);
		}

		jump = j;

		printf("\nJumping back: %X instructions\n",jump-1);
		chg_Pc += -2*(jump-1);
	}

/************************************************************************************************/

	else if(b1==0xf && b2>=4 && b2<=7)				//BRNE
	{
		int brne[8],carr=0;
		printf("BRNE instruction decoded");
		if(SREG[1].data==0)
		{
			ClearBins(0);
		    ClearBins(1);
		    ClearBins(2);
		    Hex2Bin(0,b2);
		    Hex2Bin(1,b3);
		    Hex2Bin(2,b4);

		    brne[6]=bin[0].arr[1];
		    brne[5]=bin[0].arr[0];
		    for(i=3;i>=0;i--)
		    	brne[i+1]=bin[1].arr[i];
		    brne[0]=bin[2].arr[3];

		    for(i=0;i<7;i++)
	    	brne[i] = !brne[i];

	    	ClearBins(0);
	    	bin[0].arr[0]=1;
	    	for(i=0;i<8;i++)
	    		bin[1].arr[i]=brne[i];

	    	for(i=0;i<8;i++)
	    	{
		        t = bin[1].arr[i] + bin[0].arr[i] + carr;
		        brne[i] = bin[1].arr[i] ^ bin[0].arr[i] ^ carr;
		        if(t<=1)
		        carr = 0;
		        else
		        carr = 1;
		    }

		    j=0;
		    for(i=0;i<8;i++)
			{
				j += brne[i]*pow(2,i);
			}

			printf("\nJumping back: %d instructions\n",j);
			chg_Pc += -4*j;
		}
		else
			chg_Pc += 0x4;

	}

/************************************************************************************************/

	else if(b1==0x0 && b2==0x0)						//NOP
	{
		printf("NOP instruction decoded");
		chg_Pc += 0x4;
	}
}

void output(int flag)			//Functoin to compute output for current instruction
{
	if(flag == 1)
	{
		printf("\nPC: %X\n",chg_Pc);
		Compute(chg_Pc);
		PrintSREG();
	}
}