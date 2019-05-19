#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* Calculates hit and miss rate of branch prediction*/
int predmiss = 0, predhit = 0;

char *token; 
char buf[20];
char val[1];

/*Stores PC address and index value for Pattern History Table*/
long int address = 0, indx;

/*pointer to file*/
FILE *fp;

int lowerlimit, upperlimit, max;

/************************************************************************
* Function name: onelevel
* Argument: No arguments
* Return Type: void
* Description: Analyses prediction by extracting index from PC address 
               and looking it into pattern history table
************************************************************************/
void onelevel()
{
    int *pHT = (int*) malloc(1024 * sizeof(int));

    memset(pHT, 0, 1024 * sizeof(int));

    fp = fopen("branch_trace_gcc.txt", "r");

    if (fp)
    {
        while (fgets(buf, 20, fp) != NULL)
        {
            token = strtok(buf, " ");
            address = atol(token);
            token = strtok(NULL, " ");
            strcpy(val,token);
            indx = getIndex(address);
        
            if(pHT[indx] < lowerlimit && (strncasecmp(val,"N",1) == 0))
            {
                predhit++;
                if(pHT[indx] != 0)
                    pHT[indx] -= 1;
            }
            else if(pHT[indx] > upperlimit && (strncasecmp(val,"T",1) == 0))
            {
                predhit++;

                if(pHT[indx] != max)
                    pHT[indx] += 1;
            }
            else
            {
                predmiss++;

                if(pHT[indx] > upperlimit && (strncasecmp(val,"N",1) == 0))
                    pHT[indx] -= 1;
                else if (pHT[indx] < lowerlimit && (strncasecmp(val,"T",1) == 0))
                    pHT[indx] += 1;
            }

            memset(buf, 0 , 20);
        }
    }

    free(pHT);
    fclose(fp);
}


/************************************************************************
* Function name: twolvlGlobal
* Argument: No arguments
* Return Type: void
* Description: Analyses prediction by extracting index from Global 
               History Register and looking it into pattern history table
************************************************************************/
void twolvlGlobal()
{

    int *pHT = (int*) malloc(1024 * sizeof(int));
    
    memset(pHT, 0, 1024 * sizeof(int));

    fp = fopen("branch_trace_gcc.txt", "r");


    if (fp)
    {
        while (fgets(buf, 20, fp) != NULL)
        {
            token = strtok(buf, " ");
            token = strtok(NULL, " ");
            strcpy(val,token);
            indx = getIndex(address);

            if(pHT[indx] < lowerlimit && (strncasecmp(val,"N",1) == 0))
            {
                predhit++;
                if(pHT[indx] != 0)
                    pHT[indx] -= 1;
            }
            else if(pHT[indx] > upperlimit && (strncasecmp(val,"T",1) == 0))
            {
                predhit++;

                if(pHT[indx] != max)
                    pHT[indx] += 1;
            }
            else
            {
                predmiss++;

                if(pHT[indx] > upperlimit && (strncasecmp(val,"N",1) == 0))
                    pHT[indx] -= 1;
                else if (pHT[indx] < lowerlimit && (strncasecmp(val,"T",1) == 0))
                    pHT[indx] += 1;
            }

            memset(buf, 0 , 20);

            if(strncasecmp(val,"N",1) == 0)
                address = (address << 1);
            else    
                address = ((address << 1) | 0x01);
        }   
    }

    free(pHT);
    fclose(fp);
}

/************************************************************************
* Function name: twolvlGshare
* Argument: No arguments
* Return Type: void
* Description: Analyses prediction by extracting index from PC address 
               XOR with GHR and looking it into pattern history table
************************************************************************/
void twolvlGshare()
{
    int global = 0;

    int *pHT = (int*) malloc(1024 * sizeof(int));
    
    memset(pHT, 0, 1024 * sizeof(int));

    fp = fopen("branch_trace_gcc.txt", "r");


    if (fp)
    {
        while (fgets(buf, 20, fp) != NULL)
        {
            token = strtok(buf, " ");
            address = atol(token);
            token = strtok(NULL, " ");
            strcpy(val,token);
            indx = (getIndex(address) ^ getIndex(global));

            if(pHT[indx] < lowerlimit && (strncasecmp(val,"N",1) == 0))
            {
                predhit++;
                if(pHT[indx] != 0)
                    pHT[indx] -= 1;
            }
            else if(pHT[indx] > upperlimit && (strncasecmp(val,"T",1) == 0))
            {
                predhit++;

                if(pHT[indx] != max)
                    pHT[indx] += 1;
            }
            else
            {
                predmiss++;

                if(pHT[indx] > upperlimit && (strncasecmp(val,"N",1) == 0))
                    pHT[indx] -= 1;
                else if (pHT[indx] < lowerlimit && (strncasecmp(val,"T",1) == 0))
                    pHT[indx] += 1;
            }

            memset(buf, 0 , 20);

            if(strncasecmp(val,"N",1) == 0)
                global = (global << 1);
            else    
                global = ((global << 1) | 0x01);
        }   
    }

    free(pHT);
    fclose(fp);
}

/************************************************************************
* Function name: teolvlLocal
* Argument: No arguments
* Return Type: void
* Description: Analyses prediction by extracting index from local History 
               table with help of PC address and then looking it into 
               pattern history table
************************************************************************/
void twolvlLocal()
{
    int *local = (int*) malloc(128 * sizeof(int));
    int *pHT = (int*) malloc(1024 * sizeof(int));
    int lindx;
    
    memset(pHT, 0, 1024 * sizeof(int));
    memset(local, 0, 128 * sizeof(int));

    fp = fopen("branch_trace_gcc.txt", "r");


    if (fp)
    {
        while (fgets(buf, 20, fp) != NULL)
        {
            token = strtok(buf, " ");
            address = atol(token);
            token = strtok(NULL, " ");
            strcpy(val,token);
            lindx = address & 0x7F ;
            indx = getIndex(local[lindx]);

            if(pHT[indx] < lowerlimit && (strncasecmp(val,"N",1) == 0))
            {
                predhit++;
                if(pHT[indx] != 0)
                    pHT[indx] -= 1;
            }
            else if(pHT[indx] > upperlimit && (strncasecmp(val,"T",1) == 0))
            {
                predhit++;

                if(pHT[indx] != max)
                    pHT[indx] += 1;
            }
            else
            {
                predmiss++;

                if(pHT[indx] > upperlimit && (strncasecmp(val,"N",1) == 0))
                    pHT[indx] -= 1;
                else if (pHT[indx] < lowerlimit && (strncasecmp(val,"T",1) == 0))
                    pHT[indx] += 1;
            }

            memset(buf, 0 , 20);

            if(strncasecmp(val,"N",1) == 0)
                local[lindx] = (local[lindx] << 1);
            else    
                local[lindx] = ((local[lindx] << 1) | 0x01);
        }   
    }

    free(pHT);
    free(local);
    fclose(fp);
}

/************************************************************************
* Function name: getindex
* Argument: PC address
* Return Type: long int
* Description: Extracts required number of bit value for indexing purpose
************************************************************************/
long int getIndex(long int address)
{
    return address & 0x3FF ;
}

/************************************************************************
* Function name: main
* Argument: Predictor model, bit counter.
* Return Type: int
************************************************************************/
int main(int argc, char const *argv[])
{
	lowerlimit = (pow(2, atoi(argv[2])) / 2);
	upperlimit = lowerlimit - 1;
	max = (lowerlimit * 2) - 1;

	if(strcmp(argv[1],"1") == 0)
		onelevel();
	else if(strcmp(argv[1],"2") == 0)
		twolvlGlobal();
	else if(strcmp(argv[1],"3") == 0)
		twolvlGshare();
	else if(strcmp(argv[1],"4") == 0)
		twolvlLocal();

    printf("Hit = %d\n", predhit);
    printf("Miss = %d\n", predmiss);
    printf("Miss rate = %f\n", ((float)predmiss/(predmiss+predhit)) * 100);
    printf("Hit rate = %f\n", ((float)predhit/(predmiss+predhit)) * 100);

	return 0;
}