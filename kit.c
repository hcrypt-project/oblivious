#include "kit.h"
#include "crypto.h"

int main_(int argc,char **argv)
{
	int i;
	extern int optind;
	fhe_pk_t pk;
	fhe_sk_t sk;

	puts("MLEE homomorphic encryption toolkit");

	//i=getopt(argc,argv,"gb");

	if(i==-1||i==63) //keine option, falsche oder '?'
		;//usage();

	if(i=='g')
	{
		char *pkname,*skname;

		if(optind>=argc)
			;//usage();

		printf("  generating smart-keypair as {%s.sk,%s.pk}\n",argv[optind],argv[optind]);

		pkname=(char*)malloc(strlen(argv[optind]+4));
		skname=(char*)malloc(strlen(argv[optind]+4));
		sprintf(pkname,"%s.pk",argv[optind]);
		sprintf(skname,"%s.sk",argv[optind]);

		puts("init");
		fhe_sk_init(sk);
		fhe_pk_init(pk);

		puts("generate");
		fhe_keygen(pk, sk);

		puts("store");
		fhe_pk_store(pk,pkname);
		fhe_sk_store(sk,skname);

		fhe_pk_clear(pk);
		fhe_sk_clear(sk);
	}
	if(i=='b')
	{
		//benchmark();
	}
	return 0;
}

void fhe_lib_init(fhe_sk_t *sk2)
{
}
/** I/O ***/
void fhe_pk_store(fhe_pk_t pk,char *s)
{
	FILE *f;
	f=fopen(s,"w");
	fprintf(f,"mlee public key (p,alpha,{c[i],B[i]}\n");

	gmp_fprintf(f,"%Zd\n",pk->p);
	gmp_fprintf(f,"%Zd\n",pk->alpha);

	for (int i = 0; i < FHE_S1; i++)
	{
		gmp_fprintf(f,"%Zd\n%Zd\n", pk->c[i], pk->B[i]);
	}
	fclose(f);
}
int fhe_pk_load(fhe_pk_t pk,char *filename)
{
	FILE *f;
	//static fhe_pk_t pk;
	char line[16384];
	int i;

	//fhe_pk_init(pk);
	f=fopen(filename,"r");
	if(f==NULL)
		return 0;

	fgets(line,16384,f); //header line

	fgets(line,16384,f); //p
	mpz_set_str(pk->p,line,10);
	fgets(line,16384,f); //alpha
	mpz_set_str(pk->alpha,line,10);

	for(i=0;i<FHE_S1;i++)
	{
		fgets(line,16384,f);
		mpz_set_str(pk->c[i],line,10);
		fgets(line,16384,f);
		mpz_set_str(pk->B[i],line,10);
	}
	fclose(f);

	return 1;
}
void fhe_sk_store(fhe_sk_t sk,char *filename)
{
	FILE *f;

	f=fopen(filename,"w");
	fprintf(f,"mlee secret key (p,B)\n");
	gmp_fprintf(f,"%Zd\n", sk->p);
	gmp_fprintf(f,"%Zd\n", sk->B);
	fclose(f);
}
int fhe_sk_load(fhe_sk_t sk,char *filename)
{
	FILE *f;
	//static fhe_sk_t sk;
	char line[16384];

	//fhe_sk_init(sk);
	f=fopen(filename,"r");
	if(f==NULL)
		return 0;
	fgets(line,16384,f); //header line

	fgets(line,16384,f); //p
	mpz_set_str(sk->p,line,10);
	fgets(line,16384,f); //B
	mpz_set_str(sk->B,line,10);
	fclose(f);

	return 1;
}

void fhe_int_init(fhe_int_t i)
{
	mpz_init(i->mpz);
	i->noise=1;
}

void fhe_int_clear(fhe_int_t i)
{
	mpz_clear(i->mpz);
}

void fhe_int_set(fhe_int_t to,fhe_int_t from)
{
	mpz_set(to->mpz,from->mpz);
	to->noise=from->noise;
}

void
fhe_int_add(fhe_int_t res, fhe_int_t a, fhe_int_t b, fhe_pk_t pk)
{
	/*if(((a->noise+1)>NOISE_THRESHOLD) || ((b->noise+1) > NOISE_THRESHOLD))
	{
		if(a->noise > b->noise)
		{
			fhe_recrypt(a->mpz,pk);
			a->noise=NOISE_RESET;
			res->noise=NOISE_RESET+1;
		}
		else
		{
			fhe_recrypt(b->mpz,pk);
			b->noise=NOISE_RESET;
			res->noise=NOISE_RESET+1;
		}
	}*/
	crypto_add(res->mpz,a->mpz,b->mpz,pk);
	res->noise=a->noise>b->noise?a->noise:b->noise;
}

void
fhe_int_mul(fhe_int_t res, fhe_int_t a, fhe_int_t b, fhe_pk_t pk)
{
	//printf("M(%d,%d=",a->noise,b->noise);
	if((a->noise + b->noise) > NOISE_THRESHOLD)
	{
		if(a->noise > b->noise)
		{
			fhe_recrypt(a->mpz,pk);
			a->noise=NOISE_RESET;

			if(b->noise>3)
			{
				fhe_recrypt(b->mpz,pk);
				b->noise=NOISE_RESET;
			}
		}
		else
		{
			fhe_recrypt(b->mpz,pk);
			b->noise=NOISE_RESET;

			if(a->noise>3)
			{
				fhe_recrypt(a->mpz,pk);
				a->noise=NOISE_RESET;
			}

		}
	}
	crypto_mul(res->mpz,a->mpz,b->mpz,pk);
	res->noise=a->noise+b->noise;
	//printf("%d) ",res->noise);
}

void fhe_int_xor(fhe_int_t res,fhe_int_t a,fhe_int_t b,fhe_pk_t pk)
{
	fhe_int_add(res,a,b,pk);
}

void fhe_int_or(fhe_int_t res,fhe_int_t a,fhe_int_t b,fhe_pk_t pk)
{
	if((a->noise + b->noise) > NOISE_THRESHOLD)
	{
		if(a->noise > b->noise)
		{
			fhe_recrypt(a->mpz,pk);
			a->noise=NOISE_RESET;
		}
		else
		{
			fhe_recrypt(b->mpz,pk);
			b->noise=NOISE_RESET;
		}
	}
	crypto_or(res->mpz,a->mpz,b->mpz,pk);
	res->noise=a->noise+b->noise;
}

void fhe_int_and(fhe_int_t res,fhe_int_t a,fhe_int_t b,fhe_pk_t pk)
{
	fhe_int_mul(res,a,b,pk);
}

void fhe_int_not(fhe_int_t res,fhe_int_t a,fhe_pk_t pk)
{
	crypto_not(res->mpz,a->mpz,pk);
}

void fhe_int_encrypt(fhe_int_t res,fhe_pk_t pk,int m)
{
	fhe_encrypt(res->mpz,pk,m);
	res->noise=1;
}
