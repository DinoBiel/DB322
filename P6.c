#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

//Global Variables
int wordBool = 0;
int wrdct=0;
int charct=0;
int linect=0;
char last;  //last char visited
int flagW=0;   //this is a boolean to check if -w is used
int flagC=0;   //this is a boolean to check if -c is used
int flagL=0;   //this is a boolean to check if -l is used
int flagFile=0; //checks to see if a file handle is even passed in
int noDash=1;   //Checks to see if there is a '-', if there isn't then do -wcl

int totalW = 0;	//tracks total for w category
int totalC = 0; //tracks total for c category
int totalL = 0; //tracks total for l category

//prints the counts for each category
void printCounts(int lCt, int wCt, int cCt, char *arg, int arg_c){
	//this section will print the counts

if(arg_c==1){
	//do nothing
}else{
	if(flagL==1 || noDash==1){
		printf("%d ", lCt);
	}
	if(flagW==1|| noDash==1){
		printf("%d ", wCt);
	}
	if(flagC==1|| noDash==1){
		printf("%d ", cCt);
	}

	if(flagFile==1){
		printf("PID= %d ",getpid());
	}
	//if there is a file print the file handle
	if(flagFile==1){
		printf("%s \n", arg);
	}else{
		//if no file exists, just give it a new line
		printf("\n");
	}
  }
}


//Checks to see if command line has a '-', if it does flag it.
void checkDash(int arg_c, char *arg){
	char *buffer;

		//load argument into buffer
	buffer = arg;
	    
	 //Check to see if there is a Dash. If there is no Dash default to WCL
	if(buffer[0]!='-'){
		 noDash=1; //this means there is no dash.
	}else{
		noDash=0;	//this means there is a dash. once this triggers it will stop checking
	}
}



//checks the glags to see what to print.
void checkFlags(int arg_c, char *arg){
	int j; //loop controller
	char *buffer;

		//load argument into buffer
	buffer = arg;
	    
	//if there is no '.' or '-'  then we are dealing with a possible file
	if(buffer[0]!='.' && buffer[0]!='-'){
		flagFile=1;
	}

	//if we found '-' then loop until we find w,c,or l
	if(buffer[0]=='-'){
		for(j=1; buffer[j]!='\0'; j++){
			if(buffer[j]=='w'){
				flagW=1;
			}
			if(buffer[j]=='c'){
				flagC=1;
			}
			if(buffer[j]=='l'){
				flagL=1;
			}
		}
	}
}

void countFromStdIn(){
	int c;
	while((c = fgetc(stdin))!=EOF)
	{
		++charct;		//counts chars

		//this tells me to start the word check
		if(c!=' ' && c!='\n' && last!='\n'){		
				wordBool=1;
		}

		//Check to see if c is a word
		if(((c==' ' || c=='\n') && wordBool==1) && last!='\n'){ 
			wrdct++;
			wordBool=0; //reset word bool
		}
		if(c=='\n'){
			++linect;	//counts new lines
			wordBool=0;
		}
			last=c;
	}
	if((last!=' ' && last!='\n') && c==EOF){
		wrdct++;
	}
	printf("\n");	//this is for formating purposes.
}


void countFromFile(char *arg){
	FILE *fp;
	int c;
	//fopen takes in as an argument a file handle and what to do. "r" reads "w" writes
	fp = fopen(arg, "r");
	//if no file exists for what they are trying to pass in let the user know
	if(fp == NULL){
		printf ("No File found named : %s \n", arg);
		}else{
			while((c = fgetc(fp))!=EOF)
			{
				++charct;		//counts chars

				//this tells me to start the word check
				if(c!=' ' && c!='\n' && last!='\n'){		
					wordBool=1;
				}

				//Check to see if c is a word
				if(((c==' ' || c=='\n') && wordBool==1) && last!='\n'){ 
					wrdct++;
					wordBool=0; //reset word bool
				}
				if(c=='\n'){
					++linect;	//counts new lines
					wordBool=0;
				}
					last=c;
			}
		if((last!=' ' && last!='\n') && c==EOF){
			wrdct++;
		}
	}
	fclose(fp);
}

void updateTotals(){
	totalW = totalW + wrdct;
	totalC = totalC + charct;
	totalL = totalL + linect;
}

void printGrandTotals(){
	//Print Grand Totals
	if(flagL==1 || noDash==1){
		printf("%d ", totalL);
	}
	if(flagW==1 || noDash==1){
		printf("%d ", totalW);
	}
	if(flagC==1 || noDash==1){
		printf("%d ", totalC);
	}
	printf("total ");
	printf("PID=%d \n", getpid());

}


int main(int argc, char *argv[])
{
  int i;	//loop controller for arguments
  char *buffer;  //pointer to an array of characters, holds the current argument
  int fd[2];	//for the pipe
  int pid;		//process ID value
  int ret = pipe(fd);
  int values[6];	//holds all values for counts when passed between read/write

  if(ret ==-1){
		perror("Pipe creation failed");
		exit(-1);
	}

	//Check to see if there is a dash in the command line
	for(i=0; i<argc; i++){
		if(noDash==0){
			//if there is a dash there is no need to continue checking for a dash, do nothing.
		}else{
		checkDash(argc, argv[i]);
		}
	}

	//check flags for WCL if needed
	for(i=0; i<argc; i++){
		checkFlags(argc, argv[i]);
	}

	for(i=0; i<argc; i++){
		//clear all variables for next cycle
		charct=0;
		linect=0;
		wrdct=0;
		wordBool=0;


		pid = fork();

		if(pid==0){
			//child process
			//load next arg into buffer to check for command
			buffer=argv[i];

			//check to see if a file was passed in
			if(flagFile==0){
				countFromStdIn();
			}
			//if buffer[0]=='-' that means this is a -wcl command and we should ignore it
			else if(buffer[0] !='-' && buffer[0]!='.'){	
				countFromFile(argv[i]);
			}

			//update totals
			updateTotals();

			//load array to pass values
			values[0]=linect;
			values[1]=wrdct;
			values[2]=charct;
			values[3]=totalL;
			values[4]=totalW;
			values[5]=totalC;
			

			//print counts for file
			if( argc==1 || (buffer[0]!='.' && buffer[0]!='-')){
				printCounts(linect,wrdct,charct, argv[i], argc);
			}
			//write values to pipe
			write(fd[1], &values, sizeof(values));
			exit(0);
		}else{

			//wait for children to finish
			wait(NULL);
			//parent process
			read(fd[0], &values, sizeof(values));

			//update the values for parent
			linect=values[0];
			wrdct =values[1];
			charct=values[2];
			totalL=values[3];
			totalW=values[4];
			totalC=values[5];
		}	
	}
	//print grand totals from parent
	printGrandTotals();
	return 0;
}