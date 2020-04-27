#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <graphics.h>
#include <stdlib.h>
#include <dos.h>
#define max_line 1000
#define max_name 100
//*******************************************************
struct input{
	char name[max_line];
	int value;
	input *prev;
	input *next;
	};
struct gate{
	char type[max_line];       //linked list of gates
	int delay;
	char name[max_line];
	char out[max_line];
	input *f_in;
	int output;
	gate *prev;
	gate *next;
	int visit;
	};
struct event{                 //for creating todo list
	char name[max_line];
	int time;
	int value;
	event *next;
	event *prev;
	};
int maxx;
int maxy;
int y=30;
//************************************************************
gate* circuit_parser(char *in_line,gate *head);
event* circuit_calc(gate* head,event* head_event,int clock);
input* add_in(char *p,input* head_in);
void free_all(gate *head);
gate* search_by_name(gate* head,char name[]);
gate* search_by_out(gate* head,char name[]);
gate* search_by_type(gate* head,char name[]);
gate * circuit_builder(gate *head);
event* add_event(event* head,char temp[],int time,int value);
event* check_todo_list(gate* head,event* head_event,int clock);
void init();
void draw(int set,int clock,int part,int cclock);
int init_graph(int cycles,gate *head_gate);
//*******************************MAIN_PROGRAM*********************************
void main(){                                    //opening the circuit info file
	clrscr();
	gate *temp,*temp1;
	FILE *circuit=fopen("\\circuit.in","r");
	if(circuit==NULL)
		{
		printf("Unable to open circuit.in \n");    //promt a message in case
		exit(1);
		}
	gate *head=NULL;
	char in_line[max_line];
	int i;
	while(fgets(in_line,max_line,circuit))    //parse the circuit.in
		{
		i=strlen(in_line);
		in_line[i-1]=NULL;
		head=circuit_parser(in_line,head);
		}
	fclose(circuit);
	int cycles;
	printf("Enter number of cycles: \n");   //read the number of cycles to be shown on each page
	scanf("%d",&cycles);
	FILE *data=fopen("\\data.in","r");     //open data.in file containing inport changes
	if(data==NULL)
		{
		printf("Unable to open data.in \n");
		exit(1);
		}
	event* head_event=NULL;
	char *dline;
	int time=0;
	int value;
	char name[max_name];
	fgets(dline,max_line,data);
	while(!feof(data))          //parse the data.in and add an event to todo list for each change
		{
		dline++;
		time=atoi(dline)+time;
		while((fgets(dline,max_line,data)) && (*dline != '#'))
			{
			strcpy(name,strtok(dline,"="));
			value=atoi(strtok(NULL,"="));
			head_event=add_event(head_event,name,time,value);
			}
		}
	fclose(data);
	int clock=0;
	gate* gate1;
	head=circuit_builder(head);       //construct the circuit using dfs
	init();
	maxx=getmaxx();                      //invoke graphics
	maxy=getmaxy();
	int part=init_graph(cycles,head);    //draw the lines of the graph
	int cclock=0;
	while(head_event !=NULL)      //start the clock of the circuit
		{
		head_event=check_todo_list(head,head_event,clock);
		head_event=circuit_calc(head,head_event,clock);
		head_event=check_todo_list(head,head_event,clock);
		if((cclock * part)+50 >= maxx)      //if reached the end start from the begining
			{
			cleardevice();
			init_graph(cycles,head);
			cclock=0;
			}
		y=30;
		temp=head;
		temp1=head;
		while(temp)                    //search inports and draw the value
			{
			temp=search_by_type(temp1,"INPORT");
			if(temp==NULL)
				break;
			draw(temp->output,clock,part,cclock);
			y=y+30;
			temp1=temp->next;
			}
		temp=search_by_type(head,"OUTPORT");    //draw outport  value
		draw(temp->output,clock,part,cclock);
		clock++;
		cclock++;
		delay(1000);            //delay of each cycle
		}
	getch();
	closegraph();
	free_all(head);            //free all allocated memory
}

//******************************************************************
gate* circuit_parser(char *in_line,gate *head)     //parse the info of the gate and creat a linked list of gates
	{
	char* p;
	gate *newGate=(gate*)malloc(sizeof(gate));
	strcpy(newGate->type,strtok(in_line," "));
	if(strcmp("OUTPORT",newGate->type) && strcmp("INPORT",newGate->type))
		{
		p=strtok(NULL," ");
		p++;
		newGate->delay=atoi(p);
		strcpy(newGate->name,strtok(NULL," "));
		strcpy(newGate->out,strtok(NULL," "));
		}
	else  if(strcmp("INPORT",newGate->type)==0)
		{
		strcpy(newGate->name,strtok(NULL," "));
		strcpy(newGate->out,strtok(NULL," "));
		newGate->delay=0;
		}
   else
		{
		strcpy(newGate->name,strtok(NULL," "));
		strcpy(newGate->out,"FINAL_OUTPUT");
		newGate->delay=0;
		}
	newGate->output=0;
	newGate->visit=0;
	input *head_in=NULL;
	p=strtok(NULL," ");
	while(p)
		{
		head_in=add_in(p,head_in);
		p=strtok(NULL," ");
		}
	newGate->f_in=head_in;
	if(head==NULL)
		{
		head=newGate;
		newGate->next=NULL;
		newGate->prev=NULL;
		return head;
		}
	else
		{
		newGate->next=head;
		head->prev=newGate;
		head=newGate;
		return head;
		}
	}
//*********************************************************************
input* add_in(char *p,input* head_in)          //add a new input for a gate
	{
	input *newIN=(input*)malloc(sizeof(input));
	strcpy(newIN->name,p);
	newIN->value=0;
	if(head_in==NULL)
		{
		head_in=newIN;
		newIN->next=NULL;
		newIN->prev=NULL;
		return head_in;
		}
	else
		{
		head_in->prev=newIN;
		newIN->next=head_in;
		newIN->prev=NULL;
		head_in=newIN;
		return head_in;
		}
   }
//*************************************************************
void free_all(gate *head)         //free all allocated memory
	{
	gate *temp3,*temp4;
	input *temp1,*temp2;
	temp3=head;
	while(temp3)
		{
		temp1=temp3->f_in;
		while(temp1)
			{
			temp2=temp1->next;
			free(temp1);
			temp1=temp2;
			}
		temp4=temp3->next;
		free(temp3);
		temp3=temp4;
		}
	}
//****************************************************
event* add_event(event* head,char temp[],int time,int value)    //add a new event to todo list
	{
	event* newEvent=(event*)malloc(sizeof(event));
	strcpy(newEvent->name,temp);
	newEvent->value=value;
	newEvent->time=time;
	if(head==NULL)
		{
		head=newEvent;
		newEvent->next=NULL;
		newEvent->prev=NULL;
		return head;
		}
	 else
		{
		head->prev=newEvent;
		newEvent->next=head;
		newEvent->prev=NULL;
		head=newEvent;
		return head;
		}
 }
//****************************************************
event* check_todo_list(gate* head,event* head_event,int clock)
	{
	event *temp1,*temp3,*temp2=head_event;   //check whole of the todo list and do events which their time is equal to clock
	temp1=head_event;
	gate* gate1;
	if((head_event->time)==clock)
		{
		gate1=search_by_name(head,temp1->name);
		gate1->output=head_event->value;
		head_event->next->prev=NULL;
		temp2=head_event->next;
		free(head_event);
		temp1=temp2;
		}
	while(temp1)
		{
		if(temp1->time == clock)
			{
			gate1=search_by_name(head,temp1->name);
			gate1->output=temp1->value;
			temp1->next->prev=temp1->prev;
			temp1->prev->next=temp1->next;
			temp3=temp1;
			temp1=temp1->next;
			free(temp3);
			}
		  else
			temp1=temp1->next;
		 }

	return temp2;
   }
//*************************************************************
event* circuit_calc(gate* head,event* head_event,int clock)     //calculate each gate in each cycle and add an event if the output is going to change
	{
	gate *temp=head;
	int temp_output,temp_output2;
	gate *temp1;
	while(temp)
		{
		if(strcmp(temp->type,"XNOR")== 0)
			{
			temp1=search_by_out(head,temp->name);
			temp_output=temp1->output;
			while(temp1)
				{
				temp1=search_by_out(temp1->next,temp->name);
				if(temp1)
					temp_output=(temp_output) ^ (temp1->output);
				}
			temp_output=!(temp_output);
			if(temp_output != temp->output)
				head_event=add_event(head_event,temp->name,clock+(temp->delay),temp_output);
			}
		if(strcmp(temp->type,"XOR")== 0)
			{
			temp1=search_by_out(head,temp->name);
			temp_output=temp1->output;
			while(temp1)
				{
				temp1=search_by_out(temp1->next,temp->name);
				if(temp1)
					temp_output=(temp_output) ^ (temp1->output);
				}
			if(temp_output != temp->output)
				head_event=add_event(head_event,temp->name,clock+(temp->delay),temp_output);
			}
		if(strcmp(temp->type,"OR")== 0)
			{
			temp1=search_by_out(head,temp->name);
			temp_output=temp1->output;
			while(temp1)
				{
				temp1=search_by_out(temp1->next,temp->name);
				if(temp1)
					temp_output=(temp_output) | (temp1->output);
				}
			if(temp_output != temp->output)
				head_event=add_event(head_event,temp->name,clock+(temp->delay),temp_output);
			}
		if(strcmp(temp->type,"NOR")== 0)
			{
			temp1=search_by_out(head,temp->name);
			temp_output=temp1->output;
			while(temp1)
				{
				temp1=search_by_out(temp1->next,temp->name);
				if(temp1)
					temp_output=(temp_output) | (temp1->output);
				}
			temp_output=!(temp_output);
			if(temp_output != temp->output)
				head_event=add_event(head_event,temp->name,clock+(temp->delay),temp_output);
			}
		if(strcmp(temp->type,"AND")== 0)
			{
			temp1=search_by_out(head,temp->name);
			temp_output=temp1->output;
			while(temp1)
				{
				temp1=search_by_out(temp1->next,temp->name);
				if(temp1)
					temp_output=(temp_output) & (temp1->output);
				}
			if(temp_output != temp->output)
				head_event=add_event(head_event,temp->name,clock+(temp->delay),temp_output);
			}
		if(strcmp(temp->type,"NAND")== 0)
			{
			temp1=search_by_out(head,temp->name);
			temp_output=temp1->output;
			while(temp1)
				{
				temp1=search_by_out(temp1->next,temp->name);
				if(temp1)
					temp_output=((temp_output) & (temp1->output));
				}
			temp_output=!(temp_output);
			if(temp_output != temp->output)
				head_event=add_event(head_event,temp->name,clock+(temp->delay),temp_output);
			}
		if(strcmp(temp->type,"NOT")== 0)
			{
			temp1=search_by_out(head,temp->name);
			temp_output=!(temp1->output);
			if(temp_output != temp->output)
				head_event=add_event(head_event,temp->name,clock+(temp->delay),temp_output);
			}
		if(strcmp(temp->type,"OUTPORT")== 0)
			{
			temp1=search_by_out(head,temp->name);
			temp->output=temp1->output;
			}
		temp=temp->next;
	   }
   return head_event;
   }
//******************************************************
gate * circuit_builder(gate *head)     //sort the linked list of gate with a dfs route
	{
	gate* temp,*temp1,*head_gate;
	char string[max_line]="OUTPORT";
	temp=search_by_type(head,string);
	temp->next->prev=temp->prev;
	temp->prev->next=temp->next;
	temp->prev=NULL;
	head->prev=temp;
	temp->next=head;
	temp->visit=1;
	head=temp;
	head_gate=head;
	loop_while:
	;
	while(strcmp(temp->type,"INPORT") != 0)
		{
	loop:
	;
		temp1=search_by_out(head_gate,temp->name);
		if(temp1 == NULL)
			break;
		if(temp1->visit==1)
			{
			 head_gate=temp1->next;
			 goto loop;
			 }
		else if(temp1->visit==0)
			{
			temp=temp1;
			temp->next->prev=temp->prev;
			temp->prev->next=temp->next;
			head->prev=temp1;
			temp->next=head;
			temp->prev=NULL;
			head=temp;
			head->visit=1;
			}
	   }
	   if(strcmp(temp->type,"OUTPORT")==0)
		 {
		 return head;
		 }
	   else
		{
		temp=temp->next;
		goto loop_while;
		}
	}
//**********************************************************
gate* search_by_type(gate* head,char name[]){   //find the gate with specified type
	gate* temp=head;
	while(temp)
		{
		if(strcmp(temp->type,name)==0)
			return temp;
		temp=temp->next;
		}
	return NULL;
}
//****************************************************************
gate* search_by_out(gate* head,char name[])   //find the gate with specified output name
	{
	gate* temp=head;
	while(temp)
		{
		if(strcmp(temp->out,name)==0)
			return temp;
		temp=temp->next;
		}
	return NULL;
	}
//********************************************************************
gate* search_by_name(gate* head,char name[])      //find the gate with specified name
	{
	gate* temp=head;
	while(temp)
		{
		if(strcmp(temp->name,name)==0)
			return temp;
		temp=temp->next;
		}
	return NULL;
	}
//************************************************************************
void init()                       //invoke graphics
	{
	int driver=DETECT,gmode,errorcode;
	initgraph(&driver,&gmode,"");
	errorcode=graphresult();
	if(errorcode != grOk)
		{
		printf("GRAPHICS ERROR");
		getch();
		exit(1);
		}
	}
//*************************************************************************
void draw(int set,int clock,int part,int cclock) //draw a line depends on value of set
	{
	setlinestyle(SOLID_LINE,1,5);
	if(set == 1)
		{
		setcolor(2);
		line(cclock*part+50,y-10,cclock*part+50+part,y-10);
		}
	else if(set==0)
		{
		setcolor(4);
		line(cclock*part+50,y,cclock*part+50+part,y);
		}
	char string[50];
	setcolor(2);
	itoa(clock,string,10);
	outtextxy(cclock*part+50,maxy-10,string);
	setcolor(15);
	setlinestyle(SOLID_LINE,1,1);
	line(cclock*part+50+part,maxy,cclock*part+50+part,0);
	}
//********************************************************************
int init_graph(int cycles,gate *head_gate)   //initialize the graph
	{
		gate* temp,*temp1;
		temp1=head_gate;
		setlinestyle(SOLID_LINE,1,5);
		setcolor(15);
		line(50,0,50,maxy);
		line(0,maxy-20,maxx,maxy-20);
		outtextxy(1,maxy-10,"clock");
		int parts=(maxx-50)/cycles;
		temp=head_gate;
		int y=30;
		while(temp)
			{
			temp=search_by_type(temp1,"INPORT");
			if(temp==NULL)
				break;
			outtextxy(1,y,temp->name);
			y=y+30;
			temp1=temp->next;
			}
		temp=search_by_type(head_gate,"OUTPORT");
		outtextxy(1,y,temp->name);
		return parts;
	}
//*********************************************************************
//************************MILAD ESLAMINIA 810185259********************


