#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

long int getIndex(long int);
void twolvlGshare(int);
void twolvlLocal(int);

int predmiss = 0, predhit = 0;
char *token; 
char buf[50];
char val[1];
unsigned long int address = 0, indx;
FILE *fp;
int lowerlimit, upperlimit, max;

int main(int argc, char const *argv[])
{
	lowerlimit = (pow(2, atoi(argv[2])) / 2);
	upperlimit = lowerlimit - 1;
	max = (lowerlimit * 2) - 1;

	if(strcmp(argv[1],"3") == 0)
		twolvlGshare(atoi(argv[3]));
	else if(strcmp(argv[1],"4") == 0)
		twolvlLocal(atoi(argv[3]));

    printf("Hit = %d\n", predhit);
    printf("Miss = %d\n", predmiss);
    printf("Miss rate = %f\n", ((float)predmiss/(predmiss+predhit)) * 100);
    printf("Hit rate = %f\n", ((float)predhit/(predmiss+predhit)) * 100);

	return 0;
}

void twolvlGshare(int opt)
{
	int global = 0;

	unsigned long int pHT [1024];
	
	memset(pHT, 0, 1024 * sizeof(long int));

	if(opt == 1)
		fp = fopen("drystone_trace.txt", "r");
	else
		fp = fopen("linpack_trace.txt", "r");

	if (fp)
	{
		while (fgets(buf, 50, fp) != NULL)
		{
			token = strtok(buf, " ");
			address = strtol(token, NULL, 16);
			token = strtok(NULL, " ");
		//	printf("Token is %s\n",token);
			strcpy(val,token);
			indx = (getIndex(address) ^ getIndex(global));

			if(pHT[indx] < lowerlimit && (strncasecmp(val,"0",1) == 0))
    		{
    			predhit++;
    			if(pHT[indx] != 0)
    				pHT[indx] -= 1;
    		}
    		else if(pHT[indx] > upperlimit && (strncasecmp(val,"1",1) == 0))
    		{
    			predhit++;

    			if(pHT[indx] != max)
    				pHT[indx] += 1;
    		}
    		else
    		{
    			predmiss++;

    			if(pHT[indx] > upperlimit && (strncasecmp(val,"0",1) == 0))
    				pHT[indx] -= 1;
    			else if (pHT[indx] < lowerlimit && (strncasecmp(val,"1",1) == 0))
    				pHT[indx] += 1;
    		}

    		memset(buf, 0 , 50);

    		if(strncasecmp(val,"N",1) == 0)
    			global = (global << 1);
    		else	
    			global = ((global << 1) | 0x01);
    	}	
    }

	fclose(fp);
}

void twolvlLocal(int opt)
{
	unsigned long int local[1024];
	unsigned long int pHT[128];
	unsigned long int lindx;
	
	memset(pHT, 0, 1024 * sizeof(long int));
	memset(local, 0, 128 * sizeof(long int));

	if(opt == 1)
		fp = fopen("drystone_trace.txt", "r");
	else
		fp = fopen("linpack_trace.txt", "r");


	if (fp)
	{
		while (fgets(buf, 20, fp) != NULL)
		{
			token = strtok(buf, " ");
			address = strtol(token, NULL, 16);
			token = strtok(NULL, " ");
			strcpy(val,token);
			lindx = address & 0x7F ;
			indx = getIndex(local[lindx]);

			if(pHT[indx] < lowerlimit && (strncasecmp(val,"0",1) == 0))
    		{
    			predhit++;
    			if(pHT[indx] != 0)
    				pHT[indx] -= 1;
    		}
    		else if(pHT[indx] > upperlimit && (strncasecmp(val,"1",1) == 0))
    		{
    			predhit++;

    			if(pHT[indx] != max)
    				pHT[indx] += 1;
    		}
    		else
    		{
    			predmiss++;

    			if(pHT[indx] > upperlimit && (strncasecmp(val,"0",1) == 0))
    				pHT[indx] -= 1;
    			else if (pHT[indx] < lowerlimit && (strncasecmp(val,"1",1) == 0))
    				pHT[indx] += 1;
    		}

    		memset(buf, 0 , 20);

    		if(strncasecmp(val,"N",1) == 0)
    			local[lindx] = (local[lindx] << 1);
    		else	
    			local[lindx] = ((local[lindx] << 1) | 0x01);
    	}	
    }

	fclose(fp);
}

long int getIndex(long int address)
{
	return address & 0x3FF ;
}