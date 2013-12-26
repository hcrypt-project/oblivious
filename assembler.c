/*
 * assembler.c
 *
 *  Created on: 13.01.2011
 *      Author: micha
 */

// Assembler for the hcrypt machine. Generates encrypted machine code under a libScarab public key 

// This is a quick and dirty conversion from the Java prototype

//public class Assembler
//{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <integer-fhe.h>

struct node_t
{
	char *s;
	int i;
	struct node_t *p;
};

void putsymbol(char *symbol,int org,int unique,int sourceline);
struct node_t *findsymbol(struct node_t *symbols,char *symbol);
void printlist(struct node_t *symbols);
void freelist(struct node_t *symbols);

int ok=1;
int numsymbols=0;

struct node_t *symbols,*node,**alloc;
fhe_pk_t pk;
fhe_sk_t sk;
mpz_t cipher;

	int main(int argc, char **argv)
	{
		int org=0;
		char *line;
		int i,j;
		int sourceline=0;

		if(argc!=3)
		{
			printf("usage: %s <in> <out> <publickey>\n",argv[0]);
			exit(0);
		}
		fhe_pk_init(pk);
		fhe_pk_loadkey(pk,argv[3]);
		//fhe_pk_print(pk);

//		puts("keygen");
//		fhe_keygen(pk,sk);

		symbols=NULL;
		alloc=&symbols;

		FILE *r=NULL;
		FILE *w=NULL;
		//BufferedReader r=new BufferedReader(new FileReader("/tmp/test.asm"));
		r=fopen(argv[1],"r");
		if(r==NULL)
		{
			puts("source error");
			return 8;
		}
		//BufferedWriter w=new BufferedWriter(new FileWriter("/tmp/test.obj"));
		w=fopen(argv[2],"w");
		if(w==NULL)
		{
			puts("target error");
			return 8;
		}

		//System.out.println("PASS 1");
		puts("pass 1");

		line=(char*)malloc(1024);

		//HashMap<String,Integer> labels=new HashMap<String,Integer>();

		while(!feof(r))
		{
			char *token, *tokens[100];
			//String label=null;
			char *label;
			int numtokens;
			//String line=r.readLine();
			++sourceline;
			fgets(line,100,r);


			if(*line==0)
				break;

			if(strstr(line,"INITAC")||strstr(line,"INITPC"))
				continue;

			//StringTokenizer st=new StringTokenizer(line," \t\n;");

			numtokens=0;
			token=strtok(line," \n\t");
			while(token!=NULL)
			{
				tokens[numtokens]=token;
				token=strtok(NULL," \n\t");
				++numtokens;
			}

			label=tokens[0];

			if(numtokens==0)
				continue;

			if(numtokens==2)
			{
				if(!(*line==' '||*line=='\t'))
				{
					//++label;
					//labels.put(label, new Integer(org));
					//printf("put symbol %s=%d\n",label,org);
					putsymbol(label,org,1,sourceline);
				}
			}
			else if(numtokens>2)
			{
				//++label;
				//labels.put(label, new Integer(org));
				//printf("put symbol %s=%d\n",label,org);
				putsymbol(label,org,1,sourceline);
			}
			org++;
		}
		//printlist(symbols);
		printf("  %d symbols\n  %d command words\n",numsymbols,org);


		//XXX
		//System.out.println("PASS 2");
		puts("pass 2");
		rewind(r);

		sourceline=0;
		//r=new BufferedReader(new FileReader("test.asm"));

		while(!feof(r))
		{
			char *token, *tokens[100];
			//String label=null;
			char *label,*opcode,*operand;
			int numtokens,symboladdress,cmd;
			char line0;
			struct node_t *search;
			//String label=null;

			++sourceline;
			fgets(line,100,r);
			if(feof(r))
				break;

			if(*line==0)
				break;

			line0=*line;
			numtokens=0;
			token=strtok(line," \n\t");
			while(token!=NULL)
			{
				tokens[numtokens]=token;
				token=strtok(NULL," \n\t");
				++numtokens;
			}

			//StringTokenizer st=new StringTokenizer(line," \n\t;");
			if(numtokens==0)
				continue;

			if(!strcmp(tokens[0],"INITAC"))
			{
				search=findsymbol(symbols,tokens[1]);

				if(search!=NULL)
				{
					symboladdress=search->i;
				}
				else
				{
					symboladdress=atoi(tokens[1]);

					if(symboladdress==0&&*tokens[1]!='0')
					{
						ok=0;
						printf("undef'd symbol %s in line %d\n",tokens[1],sourceline);
					}
				}
				for(i=1;i<256;i*=2)
				{
					if(symboladdress&i)
						fhe_encrypt(cipher,pk,1);
					else
						fhe_encrypt(cipher,pk,0);

					gmp_fprintf(w,"%Zd\n",cipher);
				}
				continue;
			}

			if(!strcmp(tokens[0],"INITPC"))
			{
				search=findsymbol(symbols,tokens[1]);

				if(search!=NULL)
				{
					symboladdress=search->i;
				}
				else
				{
					symboladdress=atoi(tokens[1]);

					if(symboladdress==0&&*tokens[1]!='0')
					{
						ok=0;
						printf("undef'd symbol %s in line %d\n",tokens[1],sourceline);
					}
				}

				for(i=1;i<256;i*=2)
				{
					if(symboladdress&i)
						fhe_encrypt(cipher,pk,1);
					else
						fhe_encrypt(cipher,pk,0);

					gmp_fprintf(w,"%Zd\n",cipher);
				}

				continue;
			}



			label="";
			opcode="";
			operand="";

			if(line0==' '||line0=='\t')
			{
				opcode=tokens[0];
				if(numtokens==2)
					operand=tokens[1];
			}
			else
			{
				label=tokens[0];
				opcode=tokens[1];
				if(numtokens>2)
					operand=tokens[2];
			}

			cmd=0;

			if(!strcmp(opcode,".BD"))
				cmd=100;
			if(!strcmp(opcode,"STa"))
				cmd=15;
			if(!strcmp(opcode,"L"))
				cmd=14;
			if(!strcmp(opcode,"ROL"))
				cmd=13;
			if(!strcmp(opcode,"ROR"))
				cmd=12;
			if(!strncmp(opcode,"ADD",3))
				cmd=11;
			if(!strcmp(opcode,"CLC"))
				cmd=10;
			if(!strcmp(opcode,"SEC"))
				cmd=9;
			if(!strncmp(opcode,"XOR",3))
				cmd=8;
			if(!strncmp(opcode,"AND",3))
				cmd=7;
			if(!strncmp(opcode,"OR",2))
				cmd=6;
			if(!strcmp(opcode,"BEQ"))
				cmd=5;
			if(!strcmp(opcode,"J"))
				cmd=4;
			if(!strcmp(opcode,"La"))
				cmd=3;
			if(!strcmp(opcode,"BMI"))
				cmd=2;
			if(!strncmp(opcode,"CMP",3))
				cmd=1;

			if(opcode[strlen(opcode)-1]=='a')
				cmd+=16;

			if(*operand==0)
				operand="0";

			search=findsymbol(symbols,operand);
			if(search!=NULL)
			{
				symboladdress=search->i;
			}
			else
			{
				symboladdress=atoi(operand);

				if(symboladdress==0&&*operand!='0'&&cmd<100)
				{
					ok=0;
					printf("undef'd symbol %s in line %d\n",operand,sourceline);
				}
			}

			//printf("%s,%s,%s (%d)\n",*label?label:"-",opcode,operand,symboladdress);

			//System.out.println(label+","+opcode+","+operand+"("+lookup+")");

			//int[] operation=null;



			/*
			operation=Function.encode(5, cmd);

			int[] argument=Function.encode(8, Integer.parseInt(lookup!=null?lookup:operand));

			for(int i=0;i<8;i++)
			{
				System.out.print(hex(argument[i])+" ");
				w.write(hex(argument[i])+" ");
			}
			for(int i=0;i<5;i++)
			{
				System.out.print(hex(operation[i])+" ");
				w.write(hex(operation[i])+" ");
			}
			System.out.println("\t\t"+line);
			w.write("\n");
			*/
			//printf("%d %d -> ",cmd,symboladdress);

			if(cmd<100)
			{
				for(i=1;i<256;i*=2)
				{
					if(symboladdress&i)
						fhe_encrypt(cipher,pk,1);
					else
						fhe_encrypt(cipher,pk,0);

					gmp_fprintf(w,"%Zd\n",cipher);
				}

				for(i=1;i<32;i*=2)
				{
					if(cmd&i)
						fhe_encrypt(cipher,pk,1);
					else
						fhe_encrypt(cipher,pk,0);

					gmp_fprintf(w,"%Zd\n",cipher);
				}
			}
			else
			{
				if(cmd==100) //.BD binary data
				{
					if(strlen(operand)!=13)
					{
						printf("binary dataword length mismatch in line %d\n",sourceline);
					}
					for(i=0;i<13;i++)
					{
						if(operand[i]!='0'&&operand[i]!='1')
						{
							printf("binary dataword syntax error in line %d\n",sourceline);
						}
						if(operand[i]=='0')
							fhe_encrypt(cipher,pk,0);
						else
							fhe_encrypt(cipher,pk,1);

						gmp_fprintf(w,"%Zd\n",cipher);
					}
				}
			}
			//printf(".\n");
		}

		//XXX
		fclose(w);
		fclose(r);
		freelist(symbols);
		fhe_pk_clear(pk);
		mpz_clear(cipher);

		printf("%s\n",ok?"ok":"errors");

		return 0;
	}

	void putsymbol(char *symbol,int org,int unique,int line)
	{
		struct node_t *node;

		if(unique)
			if((node=findsymbol(symbols,symbol))!=NULL)
			{
				printf("redef'd symbol %s in line %d\n",symbol,line);
				ok=0;
				return;
			}
		++numsymbols;

		*alloc=(struct node_t*)malloc(sizeof(struct node_t));
		node=*alloc;
		node->s=malloc(strlen(symbol)+1);
		strcpy(node->s,symbol);
		node->i=org;
		node->p=NULL;
		alloc=&(node->p);
	}

	struct node_t *findsymbol(struct node_t *symbols,char *symbol)
	{
		struct node_t *node;

		node=symbols;
		while(node!=NULL)
		{
			if(!strcmp(node->s,symbol))
				return node;
			node=node->p;
		}
		return NULL;
	}

	void printlist(struct node_t *symbols)
	{
		struct node_t *node;

		node=symbols;
		//printf("inspect node %08x\n",node);fflush(stdout);
		while(node!=NULL)
		{
			//printf("next=%08x\n",node->p);fflush(stdout);
			printf("%s=%d\n",node->s,node->i);
			node=node->p;
		}
	}

	void freelist(struct node_t *symbols)
	{
		struct node_t *node;

		node=symbols;
		while(node!=NULL)
		{
			symbols=node->p;
			free(node);
			node=symbols;
		}
	}

	//	static String hex(int i)
//	{
//		String s=Integer.toHexString(i);
//		if(s.length()==1)
//			s="0"+s;
//
//		return s;
//	}

//}

