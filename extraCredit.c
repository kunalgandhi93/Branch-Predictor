#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TABLE_SIZE 32768
#define LOCAL_REG 32768

long int getIndex(long int);

int main(int argc, char const *argv[])
{
	FILE *fp;
	char *token; 
	char buf[20];
	char val[1];
	unsigned long int  indx, lindx, address,lowerlimit, upperlimit, max;
	int Gpredmiss = 0, Lpredmiss = 0,branches = 0;
	int global = 0, state_Gshare = 0, state_2lvlocal = 0;
	unsigned long int pHT1 [TABLE_SIZE];
	unsigned long int pHT2 [TABLE_SIZE];
	unsigned long int meta [TABLE_SIZE];
	unsigned long int local[TABLE_SIZE];

	memset(pHT1, 0, TABLE_SIZE * sizeof(long int));
	memset(pHT2, 0, TABLE_SIZE * sizeof(long int));
	memset(meta, 0, TABLE_SIZE * sizeof(long int));
	memset(local, 0, LOCAL_REG * sizeof(long int));

	lowerlimit = (pow(2, atoi(argv[1])) / 2);
	upperlimit = lowerlimit - 1;
	max = (lowerlimit * 2) - 1;

	fp = fopen("branch_trace_gcc.txt", "r");

	if (fp)
	{
		while (fgets(buf, 20, fp) != NULL)
		{
			token = strtok(buf, " ");
			address = atol(token);
			token = strtok(NULL, " ");
			strcpy(val,token);
			branches++;
			indx = (getIndex(address) ^ getIndex(global));

			if(pHT1[indx] < lowerlimit && (strncasecmp(val,"N",1) == 0))
    		{
    			state_Gshare++;
    			if(pHT1[indx] != 0)
    				pHT1[indx] -= 1;
    		}
    		else if(pHT1[indx] > upperlimit && (strncasecmp(val,"T",1) == 0))
    		{
    			state_Gshare++;
    			if(pHT1[indx] != max)
    				pHT1[indx] += 1;
    		}
    		else
    		{
    			if(pHT1[indx] > upperlimit && (strncasecmp(val,"N",1) == 0))
    				pHT1[indx] -= 1;
    			else if (pHT1[indx] < lowerlimit && (strncasecmp(val,"T",1) == 0))
    				pHT1[indx] += 1;

                if(meta[getIndex(address)] == 0 || meta[getIndex(address)] == 3)
                    Gpredmiss++;
    		}

    		if(strncasecmp(val,"N",1) == 0)
    			global = (global << 1);
    		else	
    			global = ((global << 1) | 0x01);

    		// 2lvlLocal
    		lindx = address & (LOCAL_REG - 1);
			indx = getIndex(local[lindx]);

			if(pHT2[indx] < lowerlimit && (strncasecmp(val,"N",1) == 0))
    		{
    			state_2lvlocal++;
    			if(pHT2[indx] != 0)
    				pHT2[indx] -= 1;
    		}
    		else if(pHT2[indx] > upperlimit && (strncasecmp(val,"T",1) == 0))
    		{
    			state_2lvlocal++;
    			if(pHT2[indx] != max)
    				pHT2[indx] += 1;
    		}
    		else
    		{
    			if(pHT2[indx] > upperlimit && (strncasecmp(val,"N",1) == 0))
    				pHT2[indx] -= 1;
    			else if (pHT2[indx] < lowerlimit && (strncasecmp(val,"T",1) == 0))
    				pHT2[indx] += 1;

                if(meta[getIndex(address)] == 0 || meta[getIndex(address)] == 3)
                    Lpredmiss++;

    		}

    		if(strncasecmp(val,"N",1) == 0)
    			local[lindx] = (local[lindx] << 1);
    		else	
    			local[lindx] = ((local[lindx] << 1) | 0x01);

            if(state_Gshare == 1 && state_2lvlocal == 0)
            {
                meta[getIndex(address)] = 1;
            }
            else if(state_Gshare == 0 && state_2lvlocal == 1)
            {
                meta[getIndex(address)] = 2;
            }
            else if(state_Gshare == 1 && state_2lvlocal == 1)
            {
                meta[getIndex(address)] = 3;
            }
            else
            {
                meta[getIndex(address)] = 0;
            }

            memset(buf, 0 , 20);
            state_Gshare = state_2lvlocal = 0;
    	}
    }

	fclose(fp);	

    printf("Total Miss = %d\n", Gpredmiss + Lpredmiss);
    printf("Miss rate = %f\n", ((Gpredmiss + Lpredmiss) / (float) branches) * 100);	
    printf("Hit rate = %f\n", 100 - ((Gpredmiss + Lpredmiss) / (float) branches) * 100);
	
	return 0;
}

long int getIndex(long int address)
{
	return address & (TABLE_SIZE - 1);
}