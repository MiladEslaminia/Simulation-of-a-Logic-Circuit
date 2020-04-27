#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define mline 5000
#define mname 50
typedef struct gate gate;
typedef struct input input;    //a struct for deploying a link-list of inputs for each gate
struct input{
	char in[mname];
	int value;
	input *next;
};
struct gate{              //an struct for deploying a link-list of gates
	char kind[mname];
	char name[mname];
	char out[mname];
	input in;
	int visit;
	int output_gate;
	gate *next;
};
//***************************************************************************

input* search_in(input* head,char in[mname]);
void add_in(input *head,input* new_in);
void free_memory(gate* head_gate);
void output(gate* head_gate);
void output_calc(gate* head_gate,int flag);
void circuit_builder(gate *head_gate);
void make_gate(gate* head,char string[mline]);
void cut_gate(gate* prev);
void delete_gate(gate* prev);
void paste_gate(gate *head,gate* new_in);
gate* search_INPORT(gate* head,char name[mname]);
gate* search_by_out(gate* head,char name[mname]);
gate* search_by_name(gate* head,char name[mname]);
gate* search_by_kind(gate* head,char name[mname]);
input* make_in(input* head);
char* make2(char name[mname]);
void make(char in[mname],char *ptr);

//******************************************************************************
void main(){
clrscr();
char *ptr;
FILE *file;
char string[mline],*null=NULL;
gate dummy_gate;
input dummy_in;
input *head_in;
gate *head_gate;                //dummies are for working with link-list easily
head_gate=&dummy_gate;
head_in=&(dummy_gate.in);
make(dummy_gate.kind,null);
make(dummy_gate.name,null);
make(dummy_gate.out,null);
make(head_in->in,null);
head_in->value=-1;
head_in->next=NULL;
dummy_gate.next=NULL;
if((file=fopen("\\input.dat","r"))==NULL){   //opening input.dat file that contain circuit details
	printf("the file can not open");
	getch();
	exit(1);
	}
do{
	fgets(string,mline,file);
	make_gate(head_gate,string);
}while(!feof(file));        //checks to see if EOF has occured
circuit_builder(head_gate);
output_calc(head_gate,0);
output(head_gate);
free_memory(head_gate);  //free allocated memory after termination
fclose(file);
}
/*******************************************************************************/
char* make2(char name[mname]){     //for copying strings
	char* out;
	int i=0;
	while(name[i]!='\n'&&name[i]!=NULL){
		*(out+i)=name[i];
		i++;
	}
	*(out+i)=NULL;
	return out;
}
//*******************************************************************
void make(char in[mname],char *ptr){
	int i;
	for(i=0;i<mname;i++)
		in[i]=*(ptr+i);
	return;
}
//*******************************************************************
input* search_in(input* head,char in[mname]){
	input* prev,*next;
	prev=head;
	next=prev->next;
	while(next){
		if(strcmp(next->in,in)==0||strcmp(next->in,in)==10||strcmp(next->in,in)==-10)
			return prev;
		prev=next;
		next=prev->next;
	}
	return NULL;
}
//************************************************************
void add_in(input *head,input* new_in){
	new_in->next=head->next;
	head->next=new_in;
}
//**************************************************************
void delete_in(input* prev){
	input* del=prev->next;
	prev->next=del->next;
	free(del);
}
//**************************************************************************
input* make_in(input* head){
	char *ptr;
	ptr=strtok(NULL," ");
	while(ptr!=NULL){
		input *new_in=(input*)malloc(sizeof(input));
		if(!new_in){
			printf("error in memory");
			return NULL;
		}
		make(new_in->in,ptr);
		new_in->value=0;
		add_in(head,new_in);
		ptr=strtok(NULL," ");
	}
return head->next;
}
//**********************************************************************
gate* search_by_name(gate* head,char name[mname]){
	gate* prev,*next;
	prev=head;
	next=prev->next;
	while(next){
		if(strcmp(next->name,name)==0)
			return prev;
		prev=next;
		next=prev->next;
	}
	return NULL;
}
//****************************************************************
gate* search_by_kind(gate* head,char name[mname]){
	gate* prev,*next;
	prev=head;
	next=prev->next;
	while(next){
		if(strcmp(next->kind,name)==0)
			return prev;
		prev=next;
		next=prev->next;
	}
	return NULL;
}
//*****************************************************************
gate* search_by_out(gate* head,char name[mname]){
	gate* prev,*next;
	prev=head;
	next=prev->next;
	while(next){
		if(strcmp(next->out,name)==0)
			return prev;
		prev=next;
		next=prev->next;
	}
	return NULL;
}
//*********************************************************
gate* search_INPORT(gate* head,char name[mname]){
	gate* prev,*next;
	prev=head;
	next=prev->next;
	while(next){
		if((strcmp(next->kind,"INPORT")==0)
			&&(strcmp(next->name,name)==0))
			return prev;
		prev=next;
		next=prev->next;
	}
	return NULL;
}
//*************************************************************
void paste_gate(gate *head,gate* new_in){
	new_in->next=head->next;
	head->next=new_in;
}
//*************************************************************
void delete_gate(gate* prev){
	gate* del=prev->next;
	prev->next=del->next;
	free(del);
}
//******************************************************************
void cut_gate(gate* prev){
	gate* del=prev->next;
	prev->next=del->next;
}
//***************************************************************
void make_gate(gate* head,char string[mline]){  //make a new struct for parsed gate and add it to link-list
	char *ptr;
	ptr=strtok(string," ");
	gate *new_gate=(gate*)malloc(sizeof(gate));
	if(!new_gate){
		printf("error in memory");
		return;
	}
	make(new_gate->kind,ptr);
	ptr=strtok(NULL," ");
	char *out1;
	out1=make2(ptr);
	make(new_gate->name,out1);
	ptr=strtok(NULL," ");
	out1=make2(ptr);
	make(new_gate->out,out1);
	new_gate->in=*(make_in(&(head->in)));
	new_gate->visit=0;
	paste_gate(head,new_gate);
}
//**************************************************************
void circuit_builder(gate *head_gate){   //sort the link list with DFS algorithm
	char *ptr;
	gate* prev;
	ptr="OUTPORT";
	char outport[mname];
	make(outport,ptr);
	prev=search_by_kind(head_gate,outport);
	gate *new_gate;
	gate* head;
	head=head_gate;
	new_gate=prev->next;
	cut_gate(prev);
	paste_gate(head_gate,new_gate);
	new_gate->visit=1;
	loop_while:
	;	while(strcmp(new_gate->kind,"INPORT")!=0){
	loop:
	;	prev=search_by_out(head,new_gate->name); //donbale gate akharim(onike outesh mosavi name outporte)
		if(prev==NULL){
			break;
		}
		if(prev->next->visit==1){
			head=prev->next;
			goto loop;
		}
		else if(prev->next->visit==0){
			new_gate=prev->next;
			cut_gate(prev);    //prev->next cut mishavad
			paste_gate(head_gate,new_gate);   //prev->next ke haman new_gate ast be aval ezafe mishavad
			new_gate->visit=1;
		}
	}
	if(strcmp(new_gate->kind,"OUTPORT")==0){
		return;
	}
	else{
		new_gate=new_gate->next;
		goto loop_while;
	}
}
//************************************************************
void output_calc(gate* head_gate,int flag){    //calculate outport for each changes
	gate* head,*prev,*new_head;
	head=head_gate;
	while(head->next){
		new_head=head_gate;
		if((strcmp(head->next->kind,"INPORT")==0)&&(flag==0)){
			head->next->output_gate=0;
		}
		else{
			if(strcmp(head->next->kind,"XNOR")==0){
			prev=search_by_out(new_head,head->next->name);
			head->next->output_gate=prev->next->output_gate;
				while(1){
					new_head=prev->next;
					prev=search_by_out(new_head,head->next->name);
					if(!prev) break;
					head->next->output_gate=((prev->next->output_gate)^(head->next->output_gate));
				}
				head->next->output_gate=!head->next->output_gate;
			}
			else if(strcmp(head->next->kind,"NOT")==0){
			prev=search_by_out(new_head,head->next->name);
			head->next->output_gate=!prev->next->output_gate;
			}
			else if(strcmp(head->next->kind,"AND")==0){
			prev=search_by_out(new_head,head->next->name);
			head->next->output_gate=prev->next->output_gate;
				while(1){
					new_head=prev->next;
					prev=search_by_out(new_head,head->next->name);
					if(!prev) break;
					head->next->output_gate=(prev->next->output_gate)&(head->next->output_gate);
				}
			}
			else if(strcmp(head->next->kind,"NAND")==0){
			prev=search_by_out(new_head,head->next->name);
			head->next->output_gate=prev->next->output_gate;
				while(1){
					new_head=prev->next;
					prev=search_by_out(new_head,head->next->name);
					if(!prev) break;
					head->next->output_gate=((prev->next->output_gate)&(head->next->output_gate));
				}
				head->next->output_gate=!head->next->output_gate;
			}
			else if(strcmp(head->next->kind,"OR")==0){
			prev=search_by_out(new_head,head->next->name);
			head->next->output_gate=prev->next->output_gate;
				while(1){
					new_head=prev->next;
					prev=search_by_out(new_head,head->next->name);
					if(!prev) break;
					head->next->output_gate=(prev->next->output_gate)|(head->next->output_gate);
				}
			}
			else if(strcmp(head->next->kind,"NOR")==0){
			prev=search_by_out(new_head,head->next->name);
			head->next->output_gate=prev->next->output_gate;
				while(1){
					new_head=prev->next;
					prev=search_by_out(new_head,head->next->name);
					if(!prev) break;
					head->next->output_gate=((prev->next->output_gate)|(head->next->output_gate));
				}
				head->next->output_gate=!head->next->output_gate;
			}
			else if(strcmp(head->next->kind,"XOR")==0){
			prev=search_by_out(new_head,head->next->name);
			head->next->output_gate=prev->next->output_gate;
				while(1){
					new_head=prev->next;
					prev=search_by_out(new_head,head->next->name);
					if(!prev) break;
					head->next->output_gate=(prev->next->output_gate)^(head->next->output_gate);
				}
			}
			else if(strcmp(head->next->kind,"OUTPORT")==0){
				prev=search_by_out(new_head,head->next->name);
				head->next->output_gate=prev->next->output_gate;
			}
			else if(flag==0){
			printf("error in datetype\n");
			getch();
			exit(1);}
		}
		head=head->next;
	}
}
//*********************************************************************
void output(gate* head_gate){      //output of the program calculates changes in inports
	gate *head,*prev,*prev_gate;
	input *prev_in;
	head=head_gate;
	char string[20],name[mname];
	char* ptr;
	int temp_output;
	while(1){
		gets(string);
		ptr=strtok(string," ");
		char *out1;
		out1=make2(ptr);
		make(name,out1);
		if(strcmp(ptr,"quit")==0)
			return;
		prev=search_INPORT(head,name);
		if(!prev){
			printf("error in data, try again\n");
		}
		else{
			strtok(NULL," ");
			ptr=strtok(NULL," ");
			prev->next->output_gate=atoi(ptr);
			if(prev->next->output_gate>1){
				printf("error in data, try again\n");
			}
			else{
				output_calc(head_gate,1);
				prev=search_by_kind(head_gate,"OUTPORT");
				char out[mname];
				out1=make2(prev->next->name);
				make(out,out1);
				printf("%s = %d\n",out,prev->next->output_gate);
			}
		}
	}
}
//************************************************************************
void free_memory(gate* head_gate){
	while(head_gate->next->next!=NULL){
		delete_gate(head_gate->next);
	}
	delete_gate(head_gate);
	free(head_gate);
}
//*************************************************************
//milad eslaminia 810185259 //