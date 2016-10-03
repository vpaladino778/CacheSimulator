#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/*	Vincent Paladino
 *	CSC 322
 *	Cache Simulator
 *	Homework Assignment 3
 */

typedef struct{
	int valid;
	int tag;
	int setIndex;
	int useOrder;	/*LRU - Assigned a value by an external counter*/
	int useFreq;	/*LFU - Starts at 0, incremented by 1 each use */
}line;

typedef struct{
	line *lines;
}set;

static unsigned long B(const char *s)
{
        unsigned long b = 0;
        while (*s) {
                b <<= 1;
                b += *s++ - '0';
        }
        return b;
}

//Returns a string with 'bits' amount of 1's
char *generateMask(int bits)
{
	int i;
	char* mask = malloc(sizeof(char) * bits + 1);
	char* one = "1";
	strcpy(mask,one);
	for(i = 1; i < bits; i++){
		strcat(mask,one);
	}
	
	return mask;
}

int main(int argc, char **argv)
{

				/* Cache Information */ 
	int s;		/*Number of sets */
	int e;		/*Cache Lines within each set */
	int b;		/*Block size*/
	int m;		/*Bits in Address */
	
    int C;		/*Cache size */
	
	int hT;		/*Hit Time */
	int mP;		/*Miss Penalty */
	
	int cIndex;
	int cTag;
	int cBlock;
	
	// Counters 
	double hitCount = 0;
	double missCount = 0;
	int cycles = 0;
	int usage = 0;
	
    //Loop Counters
	int i,k,q;
    
	int *addresses = (int *) malloc(sizeof(int) * 100);
	int toAdd;
	int count = 0;
	char *p = (char *) malloc(sizeof(char) * 4);	/*Replacement Policy */
	
	//Cache Parameters
	scanf("%d",&s);
	scanf("%d%d%d%s%d%d", &e, &b, &m, p, &hT, &mP);
	set *sets = malloc(sizeof(*sets) * s);
	
	//Calculate derived quantities
	int setIndexBits = log2(s);
    int blockOffsetBits = log2(b);
	int ind;
	int tagBits = m - (setIndexBits + blockOffsetBits);
	
    C = b * e * s;
	int cbe = (int) (C / b);
	
	//Allocate memory
	for(q = 0; q < s; q++){
		sets[q].lines = malloc(sizeof(line) * e);
	}
	
	//Initialize the Cache
	for( i = 0; i < s; i++){
		for( k = 0; k < e; k++){
			sets[i].lines[k].valid = 0;
			sets[i].lines[k].useFreq = 0;
		}
	}
	
	scanf("%x",&toAdd);
	while(toAdd != 0xffffffff){
		addresses[count] = toAdd;
		scanf("%x",&toAdd);
		count++;
	}
	
	if(e == 1){
		//Direct Map (One line per set)
		for(i = 0; i < count; i++){
			printf("%0*x ",2,addresses[i]);
			cTag = addresses[i] >> (setIndexBits + blockOffsetBits);
			ind = addresses[i] >> blockOffsetBits;
			char *charMask = generateMask(setIndexBits);
			int mask = (int) B(charMask);
			cIndex = mask & ind;
			
			if(sets[cIndex].lines[0].valid == 1){
				if(cIndex == sets[cIndex].lines[0].setIndex && cTag == sets[cIndex].lines[0].tag){
					printf("H\n");
					hitCount++;
				}else {
					//Evict
					printf("M\n");
					missCount++;
					sets[cIndex].lines[0].valid = 1;
					sets[cIndex].lines[0].setIndex = cIndex;
					sets[cIndex].lines[0].tag = cTag;
				}
			}else{
				printf("M\n");
				missCount++;
				sets[cIndex].lines[0].valid = 1;
				sets[cIndex].lines[0].setIndex = cIndex;
				sets[cIndex].lines[0].tag = cTag;
			}
			free(charMask);
	
		}
		
	}else if(e > 1 && e < cbe){
		//Set Associative
		for(i = 0; i < count; i++){
			printf("%0*x ",2,addresses[i]);
			cTag = addresses[i] >> (setIndexBits + blockOffsetBits);
			ind = addresses[i] >> blockOffsetBits;
			//Extract index bits
			char *charMask = generateMask(setIndexBits);
			int mask = (int) B(charMask);
			cIndex = mask & ind;
			//Extract block bits
			charMask = generateMask(setIndexBits + tagBits);
			mask = (int) B(charMask);
			cBlock = mask & addresses[i];
			
			int miss = 0;
			for(q = 0; q < e; q++){
				if(sets[cIndex].lines[q].valid == 1 && sets[cIndex].lines[q].tag == cTag){
					usage++;
					printf("H\n");
					hitCount++;
					sets[cIndex].lines[q].useFreq += 1;
					sets[cIndex].lines[q].useOrder = usage;
					miss = 0;
					break;
				}else{
					miss = 1;
				}
			}
			if(miss == 1){
				printf("M\n");
				missCount++;
				int n;
				int least = 1;
				//Evict
				if(i == 0){
					sets[cIndex].lines[least].valid = 1;
					sets[cIndex].lines[least].setIndex = cIndex;
					sets[cIndex].lines[least].useOrder = usage;
					sets[cIndex].lines[least].useFreq = 1;
					sets[cIndex].lines[least].tag = cTag;
					usage++;	
				}else if(strcmp(p,"LFU")){
					//Find candidate
					for(n = 1; n < e; n++){
						if(sets[cIndex].lines[n].useFreq < sets[cIndex].lines[least].useFreq){
							least = n;
						}
					}
					//Evict
					sets[cIndex].lines[least].valid = 1;
					sets[cIndex].lines[least].setIndex = cIndex;
					sets[cIndex].lines[least].useFreq = 1;
					sets[cIndex].lines[least].tag = cTag;
					usage++;
				}else if(strcmp(p,"LRU")){
					for(n = 0; n < e; n++){
						if(sets[cIndex].lines[n].useOrder < sets[cIndex].lines[least].useOrder){
							least = n;
						}
					}
					//Evict
					sets[cIndex].lines[least].valid = 1;
					sets[cIndex].lines[least].setIndex = cIndex;
					sets[cIndex].lines[least].useOrder = usage;
					sets[cIndex].lines[least].tag = cTag;
					usage++;
				}
			
			}

		}
	}else{
		//Fully Associative
		for(i = 0; i < count; i++){
			printf("%0*x ",2,addresses[i]);
			cTag = addresses[i] >> (setIndexBits + blockOffsetBits);
			ind = addresses[i] >> blockOffsetBits;
			//Extract block bits
			char *charMask = generateMask(setIndexBits + tagBits);
			int mask = (int) B(charMask);
			cBlock = mask & addresses[i];
			
			int miss = 0;
			for(q = 0; q < e; q++){
				if(sets[0].lines[q].valid == 1 && sets[0].lines[q].tag == cTag){
					usage++;
					printf("H\n");
					hitCount++;
					sets[0].lines[q].useFreq += 1;
					sets[0].lines[q].useOrder = usage;
					miss = 0;
					break;
				}else{
					miss = 1;
				}
			}
			if(miss == 1){
				printf("M\n");
				missCount++;
				int n;
				int least = 1;
				//Evict
				if(i == 0){
					sets[0].lines[0].valid = 1;
					sets[0].lines[0].useFreq = 1;
					sets[0].lines[0].tag = cTag;
					usage++;
				}else if(strcmp(p,"LFU") == 0){
					//Find candidate
					for(n = 0; n < e; n++){
							if(sets[0].lines[n].useFreq < sets[0].lines[least].useFreq){
								least = n;
							}
					}
					//Evict
					sets[0].lines[least].valid = 1;
					sets[0].lines[least].useFreq = 1;
					sets[0].lines[least].tag = cTag;
					usage++;
				}else if(strcmp(p,"LRU") == 0){
					for(n = 0; n < e; n++){
						if(sets[0].lines[n].useOrder < sets[0].lines[least].useOrder){		
							least = n;
						}
					}
					//Evict
					sets[0].lines[least].valid = 1;
					sets[0].lines[least].useOrder = usage;
					sets[0].lines[least].tag = cTag;
					usage++;
				}
			
			}

		}
	}
	
	int missRate =(int) ((missCount / (missCount + hitCount)) * 100);
	cycles = (hT * (hitCount + missCount) + (missCount * mP));
	
	printf("%d %d\n",missRate, cycles);

	//Free memory
	for(q = 0; q < s; q++){
		free(sets[q].lines);
	}
	free(p);
	free(addresses);
	free(sets);
	return 0;
}
