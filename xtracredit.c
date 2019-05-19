//#define DEBUG

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define STRONGLY_NOT_TAKEN 0
#define STRONGLY_TAKEN 1
#define TABLE_SIZE 32768
#define MASK_BITS 0x3FF
#define MASK_7BITS 0x7F


int main(int argc, char *argv[]){
	
	long int missLocalPred = 0;
	long int missGsharePred= 0;
	long int correctLocalPred = 0;
	long int correctGsharePred = 0;
	long int correctPred = 0;
	long int missPred = 0;
	long int totalBranches = 0;
	int PHT[TABLE_SIZE], PHT1[TABLE_SIZE],
		LHR[TABLE_SIZE], META[TABLE_SIZE];
	int counter;
	int globalFlag = 0;
	unsigned int address,LHR_address,PHT_address, PC_address;
	char *id;
	unsigned int GHR_address = 0x00000000;
	int state1 = 0,state2 = 0;
	counter = pow(2,atoi(argv[1]));
	FILE *traceFile;
	traceFile = fopen("branch_trace_gcc.txt","r");
	if(traceFile == NULL){
		printf("Unable to open trace file \n");
		return 0;
	}

	for(int i = 0; i < TABLE_SIZE; i++){
		
		PHT[i] = 00;
		PHT1[i] = 00;
		META[i] = 00;
		LHR[i] = 00;
	} 
	char buffer[20];
	while(fgets(buffer,20, traceFile) != NULL){
		char *p;
		p = strtok(buffer," ");
		//address = 0;
		PC_address = (unsigned long)atol(p);
		PC_address = PHT_address = LHR_address = PC_address%TABLE_SIZE;
		//PHT_address = (PC_address & MASK_BITS);
		//LHR_address = (PC_address & MASK_BITS);
		address = LHR[LHR_address];
		//PC_address = PC_address & MASK_BITS;
		id = strtok(NULL, " ");
		
		#ifdef DEBUG
			printf("%ld\n", address);
			printf("%s\n", id);
		#endif
		PHT_address = PHT_address ^ GHR_address;
		totalBranches++;
		
		switch(*id){
			
			case 'T':
				if(PHT1[PHT_address] >= (counter/2)){
					correctGsharePred++;
					if(PHT1[PHT_address] >= (counter-1))
					{
						PHT1[PHT_address] = (counter-1);
					}
					else
						PHT1[PHT_address] = PHT1[PHT_address] + 1;
					state1++;
					#ifdef DEBUG
						printf("T:Correct\n");
					#endif
					
				}
				else{
					#ifdef DEBUG
						printf("T:Miss\n");
					#endif
					PHT1[PHT_address] = PHT1[PHT_address] + 1;
					if (META[PC_address] >= 2)
						missGsharePred++;
				}
				GHR_address = (GHR_address << 1);
				GHR_address = GHR_address | 0x00000001;
				GHR_address = GHR_address & 0x00007FFF; 

				break;
			case 'N':
			
				if(PHT1[PHT_address] < (counter/2)){
					correctGsharePred++;
					if(PHT1[PHT_address] <= STRONGLY_NOT_TAKEN){
						PHT1[PHT_address] = STRONGLY_NOT_TAKEN;
					}
					else
						PHT1[PHT_address] = PHT1[PHT_address] - 1;;
					#ifdef DEBUG
						printf("N:Correct\n");
					#endif
					
				}
				else{
					#ifdef DEBUG
						printf("N:Miss\n");
					#endif
					PHT1[PHT_address] = PHT1[PHT_address] - 1;
					if (META[PC_address] >= 2)
						missGsharePred++;
				}
				GHR_address = (GHR_address << 1);
				GHR_address = GHR_address | 0x00000000;
				GHR_address = GHR_address & 0x00007FFF; 
				break;
			default:	
				printf("ERROR in reading trace file\n");
				break;
		}

		switch(*id){
			
			case 'T':
			
				if(PHT[address] >= (counter/2)){
					correctLocalPred++;
					if(PHT[address] >= (counter-1))
					{
						PHT[address] = (counter-1);
					}
					else
						PHT[address] = PHT[address] + 1;
					state2++;
					#ifdef DEBUG
						printf("T:Correct\n");
					#endif
					
				}
				else{
					#ifdef DEBUG
						printf("T:Miss\n");
					#endif
					PHT[address] = PHT[address] + 1;
					if (META[PC_address] < 2)
						missLocalPred++;
				}
				LHR[LHR_address] = (LHR[LHR_address] << 1);
				LHR[LHR_address] = LHR[LHR_address] | 0x00000001;
				LHR[LHR_address] = LHR[LHR_address] & 0x00007FFF; 

				break;
			case 'N':
			
				if(PHT[address] < (counter/2)){
					correctLocalPred++;
					if(PHT[address] <= STRONGLY_NOT_TAKEN){
						PHT[address] = STRONGLY_NOT_TAKEN;
					}
					else
						PHT[address] = PHT[address] - 1;;
					#ifdef DEBUG
						printf("N:Correct\n");
					#endif
					
				}
				else{
					#ifdef DEBUG
						printf("N:Miss\n");
					#endif
					PHT[address] = PHT[address] - 1;
					if (META[PC_address] < 2)
						missLocalPred++;
				}
				LHR[LHR_address] = (LHR[LHR_address] << 1);
				LHR[LHR_address] = LHR[LHR_address] | 0x00000000;
				LHR[LHR_address] = LHR[LHR_address] & 0x00007FFF;
				break;
			default:	
				printf("ERROR in reading trace file\n");
				break;
		}

		if((PHT1[PHT_address] < 2 && PHT[address] >= 2) &&(state2 == 1 && state1 == 0))
		{
			META[PC_address] = META[PC_address] + 1;
			if (META[PC_address] >= 3)
			{
				META[PC_address] = 3;
			}
		}
		if ((PHT1[PHT_address] >= 2 && PHT[address] < 2) &&(state2 == 0 && state1 == 1))
		{
			META[PC_address] = META[PC_address] - 1;
			if (META[PC_address] <= 0)
			{
				META[PC_address] = 0;
			}
		}
	}

	missPred = missLocalPred + missGsharePred;
	state1 = state2 = 0;
	fclose(traceFile);
	printf("Miss prediction 	= %ld\n", missPred);
	//printf("Correct prediction  	= %ld\n", correctPrediction);
	//printf("Total - (Miss + Predict)%ld\n", totalBranches-(missPrediction+correctPrediction));
	printf("Total Branches 		= %ld\n", totalBranches);
	printf("Missrate		= %.4f\n", (float)missPred/totalBranches*100);
	//printf("Correct prediction	= %.4f\n", (float)correctPrediction/totalBranches*100);
}
