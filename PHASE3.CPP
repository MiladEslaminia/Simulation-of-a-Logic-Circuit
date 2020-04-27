#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
struct gate             //gate data type to define a gate easily
	{
	char type[25];
	char name[25];
	char out[25];
	char inport1[25];
	char inport2[25];
	int in1;
	int in2;
	};

void simulator(gate gate,int selector);   //calculate hte gate
int gate_parser(gate *gate);              //parse the data of gate
void in_parser(gate gate,char input[],int *in1,int *in2);      //parse inputs of gates
void main()
	{
	char input[20];
	gate gate1;
	gate1.in1=0;               //inputs initialization
	gate1.in2=0;
	int in1=0,in2=0;
	int selector;
	clrscr();
	selector=gate_parser(&gate1);
	gets(input);

	while(strcmp(input,"quit"))        //condition of exit
		{
		in_parser(gate1,input,&in1,&in2);     //runtime loop ends with input=quit
		gate1.in1=in1;
		gate1.in2=in2;
		simulator(gate1,selector);
		gets(input);
		}
	}
int gate_parser(gate *gate)
	{
	char input[100];
	char input1[100];
	char input2[100];
	char input3[100];
	char *p;
	gets(input);
	strcpy(gate->type,strtok(input," "));
	if(!strcmp(gate->type,"NOT"))               //check the gate type
		{
		strcpy(gate->name,strtok(NULL," "));    //use strcpy to copy on string to another
		strcpy(gate->out,strtok(NULL," "));
		strcpy(gate->inport1,strtok(NULL," "));
		gets(input1);
		strtok(input1," ");
		p=strtok(NULL," ");
		if(!(strcmp(p,(gate->name))))
			strcpy(gate->inport1,p);
		p=strtok(NULL," ");
		if(!(strcmp(p,(gate->name))))
			strcpy(gate->inport2,p);
		gets(input3);
		strtok(input3," ");
		strcpy(gate->out,strtok(NULL," "));
		return 7;
		}
	else
		{
		strcpy(gate->name,strtok(NULL," "));
		strcpy(gate->out,strtok(NULL," "));
		strcpy(gate->inport1,strtok(NULL," "));
		strcpy(gate->inport2,strtok(NULL," "));
		gets(input1);
		strtok(input1," ");
		p=strtok(NULL," ");
		if(!(strcmp(p,(gate->name))))
			strcpy(gate->inport1,p);
		gets(input2);
		strtok(input2," ");
		p=strtok(NULL," ");
		if(!(strcmp(p,(gate->name))))
		strcpy(gate->inport2,p);
		gets(input3);
		strtok(input3," ");
		strcpy(gate->out,strtok(NULL," "));   //switch between different kinds of type
		if(!(strcmp(gate->type,"XOR")))		   //selector will hold the value of gate type
			return 1;
		if(!(strcmp(gate->type,"XNOR")))
			return 2;
		if(!(strcmp(gate->type,"OR")))
			return 3;
		if(!(strcmp(gate->type,"NOR")))
			return 4;
		if(!(strcmp(gate->type,"AND")))
			return 5;
		if(!(strcmp(gate->type,"NAND")))
			return 6;
	  }
	return 0;
	}
void in_parser(gate gate2,char input[],int *in1,int *in2)
	{
	char *p;
	p=strtok(input,"=");                        //changes any output other than 0 to 1
	if(!(strcmp(p,(gate2.inport1))))
		if(atoi(strtok(NULL," ")))              //evaluate the gate
			{
			*in1=1;
			return;
			}
		else
			{
			*in1=0;
			return;
			}
	if(!(strcmp(p,(gate2.inport2))))
		if(atoi(strtok(NULL," ")))
			{
			*in2=1;
			return;
			}
		else
			{
			*in2=0;
			return;
			}

	}
void simulator(gate gate,int selector)
	{
	switch (selector)             //swtich to the proper gate
		{
		case 0:
			printf("Invalid gate type \n");
			break;
		case 1:
			printf("%s=%d \n",gate.out,((gate.in1) ^ (gate.in2)));  //logical operations
			break;
		case 2:
			printf("%s=%d \n",gate.out,!((gate.in1) ^ (gate.in2)));
			break;
		case 3:
			printf("%s=%d \n",gate.out,((gate.in1) | (gate.in2)));
			break;
		case 4:
			printf("%s=%d \n",gate.out,!((gate.in1) | (gate.in2)));
			break;
		case 5:
			printf("%s=%d \n",gate.out,((gate.in1) & (gate.in2)));
			break;
		case 6:
			printf("%s=%d \n",gate.out,!((gate.in1) & (gate.in2)));
			break;
		case 7:
			if(gate.in1==1)                       //logical not
				printf("%s=0 \n",gate.out);
			else
				printf("%s=1 \n",gate.out);
			break;
	   }
	   return;
	}


