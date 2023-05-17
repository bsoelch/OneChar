#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

static const int  FILE_SEEK_ERROR =-2;

typedef enum{
	OP_PLUS,
	OP_MINUS,

	OP_MULT,
	OP_DIV,
	OP_MOD,

	OP_POW,

	OP_GT,
	OP_LT,
	OP_EQ,
	OP_AND,
	OP_OR,

	OP_BRACKET,
	OP_SAVE
}Operator;

int opLevel[]={
		[OP_AND]=1,[OP_OR]=1,
		[OP_GT]=2,[OP_LT]=2,[OP_EQ]=2,
		[OP_PLUS]=3,[OP_MINUS]=3,
		[OP_MULT]=4,[OP_DIV]=4,[OP_MOD]=4,
		[OP_POW]=5,
		[OP_SAVE]=6,
};

int64_t mem[1024];
int64_t valStack[1024];
int64_t valCount=0;
size_t valCap=1024;
bool nextVal=true;
bool hadSpace=false;
Operator opStack[1024];
size_t opCount=0;
size_t opCap=1024;
size_t ipStack[1024];
size_t ipCount=0;
size_t ipCap=1024;
int64_t loopCount=0;
int64_t procCount=0;

bool comment=false;
bool stringMode=false;
bool escapeMode=false;

void evaluateOps(int nextLevel){
	while(opCount>0){
		if(opStack[opCount-1]!=OP_BRACKET&&opLevel[opStack[opCount-1]]>=nextLevel){
			switch(opStack[--opCount]){
			case OP_AND:
				if(valCount<2){fputs("not enough arguments for '&'",stderr);exit(1);};
				valCount--;
				valStack[valCount-1]&=valStack[valCount];
				break;
			case OP_OR:
				if(valCount<2){fputs("not enough arguments for '>'",stderr);exit(1);};
				valCount--;
				valStack[valCount-1]|=valStack[valCount];
				break;
			case OP_GT:
				if(valCount<2){fputs("not enough arguments for '>'",stderr);exit(1);};
				valCount--;
				valStack[valCount-1]=valStack[valCount-1]>valStack[valCount]?1:0;
				break;
			case OP_LT:
				if(valCount<2){fputs("not enough arguments for '<'",stderr);exit(1);};
				valCount--;
				valStack[valCount-1]=valStack[valCount-1]<valStack[valCount]?1:0;
				break;
			case OP_EQ:
				if(valCount<2){fputs("not enough arguments for '='",stderr);exit(1);};
				valCount--;
				valStack[valCount-1]=valStack[valCount-1]==valStack[valCount]?1:0;
				break;
			case OP_PLUS:
				if(valCount<2){fputs("not enough arguments for '+'",stderr);exit(1);};
				valCount--;
				valStack[valCount-1]+=valStack[valCount];
				break;
			case OP_MINUS:
				if(valCount<2){fputs("not enough arguments for '-'",stderr);exit(1);};
				valCount--;
				valStack[valCount-1]-=valStack[valCount];
				break;
			case OP_MULT:
				if(valCount<2){fputs("not enough arguments for '*'",stderr);exit(1);};
				valCount--;
				valStack[valCount-1]*=valStack[valCount];
				break;
			case OP_DIV:
				if(valCount<2){fputs("not enough arguments for '/'",stderr);exit(1);};
				valCount--;
				valStack[valCount-1]/=valStack[valCount];
				break;
			case OP_MOD:
				if(valCount<2){fputs("not enough arguments for '%'",stderr);exit(1);};
				valCount--;
				valStack[valCount-1]%=valStack[valCount];
				break;
			case OP_POW:
				if(valCount<2){fputs("not enough arguments for '^'",stderr);exit(1);};
				valCount--;
				int64_t pow=1,a=valStack[valCount-1],e=valStack[valCount];
				if(e<0){
					pow=0;
				}else{
					while(e!=0){
						if(e&1){
							pow*=a;
						}
						a*=a;
						e>>=1;
					}
				}
				valStack[valCount-1]=pow;
				break;
			case OP_SAVE:
				if(valCount<2){fputs("not enough arguments for '$'",stderr);exit(1);};
				valCount--;
				mem[valStack[valCount]]=valStack[valCount-1];
				valCount--;
				break;
			case OP_BRACKET:
				fputs("unreachable",stderr);exit(1);
				break;
			}
		}else{
			break;
		}
	}
}

void runProgram(char* chars,size_t size){//unsused characters: `
	for(size_t ip=0;ip<size;ip++){
	  if(comment){
	    if(chars[ip]=='\\'||chars[ip]=='\n')
	      comment=false;
	    continue;
	  }
		if(stringMode){
			if(escapeMode){
				escapeMode=false;
				switch(chars[ip]){
				case '"':
					valStack[valCount++]='"';
					break;
				case '\\':
					valStack[valCount++]='\\';
					break;
				case 'n':
					valStack[valCount++]='\n';
					break;
				case 't':
					valStack[valCount++]='\t';
					break;
				case 'r':
					valStack[valCount++]='\r';
					break;
				default:
					fprintf(stderr,"unsupported escape sequence: \\%c\n",chars[ip]);exit(1);
				}
			}else if(chars[ip]=='\\'){
				escapeMode=true;
			}else if(chars[ip]=='"'){
				stringMode=false;
				int64_t tmp=valCount-ipStack[--ipCount];
				valStack[valCount++]=tmp;
			}else{
				valStack[valCount++]=chars[ip];
			}
		}else if(procCount>0){
			if(chars[ip]=='{'){
				procCount++;
			}else if(chars[ip]=='}'){
				procCount--;
			}
		}else if(loopCount>0){
			if(chars[ip]==']'){
				loopCount++;
			}else if(chars[ip]==']'){
				loopCount--;
			}
		}else{
			switch(chars[ip]){
			case '0':case '1':case '2':case '3':case '4':
			case '5':case '6':case '7':case '8':case '9':
				hadSpace=false;
				if(nextVal){
					nextVal=false;
					valStack[valCount++]=chars[ip]-'0';
				}else{
					valStack[valCount-1]=10*valStack[valCount-1]+(chars[ip]-'0');
				}
				break;
			case '+':
				hadSpace=false;
				evaluateOps(opLevel[OP_PLUS]);
				opStack[opCount++]=OP_PLUS;
				nextVal=true;
				break;
			case '-':
				hadSpace=false;
				evaluateOps(opLevel[OP_MINUS]);
				opStack[opCount++]=OP_MINUS;
				nextVal=true;
				break;
			case '*':
				hadSpace=false;
				hadSpace=false;
				evaluateOps(opLevel[OP_MULT]);
				opStack[opCount++]=OP_MULT;
				nextVal=true;
				break;
			case '/':
				hadSpace=false;
				evaluateOps(opLevel[OP_DIV]);
				opStack[opCount++]=OP_DIV;
				nextVal=true;
				break;
			case '%':
				hadSpace=false;
				evaluateOps(opLevel[OP_MOD]);
				opStack[opCount++]=OP_MOD;
				nextVal=true;
				break;
			case '^':
				evaluateOps(opLevel[OP_POW]+1);
				opStack[opCount++]=OP_POW;
				nextVal=true;
				break;
			case '>':
				evaluateOps(opLevel[OP_GT]);
				opStack[opCount++]=OP_GT;
				nextVal=true;
				break;
			case '<':
				evaluateOps(opLevel[OP_LT]);
				opStack[opCount++]=OP_LT;
				nextVal=true;
				break;
			case '=':
				evaluateOps(opLevel[OP_EQ]);
				opStack[opCount++]=OP_EQ;
				nextVal=true;
				break;
			case '&':
				evaluateOps(opLevel[OP_AND]);
				opStack[opCount++]=OP_AND;
				nextVal=true;
				break;
			case '|':
				hadSpace=false;
				evaluateOps(opLevel[OP_OR]);
				opStack[opCount++]=OP_OR;
				nextVal=true;
				break;
			case '!':
				hadSpace=false;
				valStack[valCount-1]=valStack[valCount-1]==0?1:0;
				nextVal=true;
				break;
			case '~':
				hadSpace=false;
				valStack[valCount-1]=~valStack[valCount-1];
				nextVal=true;
				break;
			case '(':
				hadSpace=false;
				if(!nextVal){//evaluate unfinished integers
					evaluateOps(0);
					nextVal=true;
				}
				opStack[opCount++]=OP_BRACKET;
				nextVal=true;
				break;
			case ')':
				hadSpace=false;
				evaluateOps(0);
				if(opStack[opCount-1]!=OP_BRACKET){
					fputs("unfinished expression in bracket",stderr);exit(1);
				}
				opCount--;
				nextVal=true;
				break;
			case '@':
				valStack[valCount-1]=mem[valStack[valCount-1]];
				nextVal=true;
				break;
			case '$':;
				evaluateOps(opLevel[OP_SAVE]);
				opStack[opCount++]=OP_SAVE;
				nextVal=true;
				break;
			case '#':;
				int64_t id=valStack[valCount-1];
				valCount--;
				if(id<=0){
					id=1-id;
					if(id>valCount){
						fprintf(stderr,"stack index out of bounds %"PRId64"\n",id);exit(1);
					}
					valStack[valCount-id]=valStack[valCount-1];
					nextVal=true;
				}else{
					if(id>valCount){
						fprintf(stderr,"stack index out of bounds %"PRId64"\n",id);exit(1);
					}
					valStack[valCount]=valStack[valCount-id];
					valCount++;
					nextVal=true;
				}
				break;
			case '[':
				evaluateOps(0);
				if(valStack[valCount-1]!=0){
					ipStack[ipCount++]=ip;
				}else{
					loopCount=1;
				}
				valCount--;
				nextVal=true;
				break;
			case ']':
				evaluateOps(0);
				if(valStack[valCount-1]!=0){
					ip=ipStack[ipCount-1];
				}else{
					ipCount--;
				}
				valCount--;
				nextVal=true;
				break;
			case ':'://duplicate top value
				hadSpace=false;
				evaluateOps(0);
				valStack[valCount]=valStack[valCount-1];
				valCount++;
				nextVal=true;
				break;
			case '.'://drop last element
				hadSpace=false;
				evaluateOps(0);
				valCount--;
				nextVal=true;
				break;
			case '{':
				hadSpace=false;
				evaluateOps(0);
				valStack[valCount++]=ip;
				nextVal=true;
				procCount=1;
				break;
			case '}':
				if(ipCount<=0){
					fputs("unexpected return '}'",stderr);exit(1);
				}
				ip=ipStack[--ipCount];//return
				break;
			case '?':;
				uint64_t to=valStack[--valCount];
				ipStack[ipCount++]=ip;
				ip=to;
				break;
			case ';':
				evaluateOps(0);
				printf("%"PRId64"\n",valStack[--valCount]);
				nextVal=true;
				break;
			case ',':
				evaluateOps(0);
				printf("%c",(char)valStack[--valCount]);
				nextVal=true;
				break;
			case '\'':
				hadSpace=false;
				evaluateOps(0);
				valStack[valCount++]=getchar();
				nextVal=true;
				break;
			case '"':
				hadSpace=false;
				ipStack[ipCount++]=valCount;
				stringMode=true;
				break;
			case '\\':
			  comment=true;
			  break;
			default:
				if(!hadSpace){
					evaluateOps(0);
					nextVal=true;
					hadSpace=true;
				}
				break;
			}
		}
	}
	evaluateOps(0);

	puts("\n------------------");
	printf("%"PRId64":",valCount);
	while(valCount>0){
		printf("%"PRId64" ",valStack[--valCount]);
	}
	puts("");
}

/* Copy from StackOverflow
 * fp is assumed to be non null
 * */
long int fsize(FILE *fp){
    long int prev=ftell(fp);
    if(prev==-1||fseek(fp, 0L, SEEK_END)!=0){
		return FILE_SEEK_ERROR;
    }
    long int sz=ftell(fp);
    //go back to where we were
    if(fseek(fp,prev,SEEK_SET)!=0){
		return FILE_SEEK_ERROR;
    }
    return sz;
}
int main(int numArgs,char** args) {
	bool loadFile;
	char *code=NULL;
	if(numArgs==2){
		loadFile=false;
		code = args[1];
	}else if(numArgs>2&&(strcmp(args[1],"-f")==0)){
		loadFile=true;
		code = args[2];
	}
	if(code==NULL){
		printf("usage: [Filename]\n or -f [Filename]");
		return EXIT_FAILURE;
	}
	long int size;
	if(loadFile){
		FILE *file = fopen(code, "r");
		if(file!=NULL){
			size=fsize(file);
			if(size==FILE_SEEK_ERROR){
				printf("IO Error while detecting file-size\n");
				return EXIT_FAILURE;
			}else if(size<0){//XXX recover form undetected file size (if seek worked)
				printf("IO Error while detecting file-size\n");
				return EXIT_FAILURE;
			}else{
				code = malloc((size+1)*sizeof(char));
				if(code==NULL){
					printf("Memory Error\n");
					return EXIT_FAILURE;
				}
				size=fread(code,sizeof(char),size,file);
				if(size<0){
					printf("IO Error while reading file\n");
					free(code);
					return EXIT_FAILURE;
				}
				fclose(file);//file no longer needed (whole contents are buffered)
			}
			if(code==NULL){
				printf("Memory Error\n");
				return EXIT_FAILURE;
			}
		}else{
			printf("IO Error while opening File: %s\n",code);
			return EXIT_FAILURE;
		}
		code[size]='\0';//null-terminate string (for printf)
	}else{
		size=strlen(code);
	}
	//print buffer
	runProgram(code,size);
	if(loadFile){
		free(code);
	}
	return EXIT_SUCCESS;
}

