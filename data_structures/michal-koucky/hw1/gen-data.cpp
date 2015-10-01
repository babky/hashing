// Generates data

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define T unsigned long

// int BASELEN=2000000;  // 12G
int BASELEN=8000000;  // 48G

T nextrandom()
{
    return ((T)rand()) * ((T)rand()) + (T)rand() + ((T)rand()<<32) + rand();
}

T nextrandom(T A, T B)
{
   if(B<=A)return A;
   return A + (nextrandom() % (1+B-A));
}

void progression(T A, T increment, int length)
{
   for(int i=0;i<length;i++){
	printf("%lu %lu\n",A,nextrandom());
	A+=increment;
   }
}

void randomprogression(T A, T increment, T rozptyl, int length)
{
   for(int i=0;i<length;i++){
        printf("%lu %lu\n",nextrandom(A,A+rozptyl),nextrandom());
	A+=increment;
   }
}


void randominterval(T A, T B, int length)
{
   for(int i=0;i<length;i++){
	printf("%lu %lu\n",nextrandom(A,B),nextrandom());
   }
}

int main(int argc, char* argv[])
{
    unsigned int total=0;
    unsigned int length;

    int seed;
    if(argc<=1){ printf("Zadej seed jako argument. Zopakuj seed dvakrat pro 1/10 dat.\n");return 1;}
    else sscanf(argv[argc-1],"%d",&seed);
    if(argc==3)BASELEN/=10;
    srand(seed);

    total+=(length=BASELEN+nextrandom(1,BASELEN));

    randominterval(1000,100000,length);    

    total+=(length=BASELEN+nextrandom(1,BASELEN));

    randominterval(0,(-1L)-5,length);    

    total+=(length=BASELEN+nextrandom(1,BASELEN));

	T start1 = rand();
    progression(start1, 17, length * 10);
    progression(start1 + rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 4), length * (1 + rand() % 3));
    progression(start1 - rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 4), length * 2);
    progression(start1, 17, length * 10);
    progression(start1 + rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 4), length);
    progression(start1 + rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 3), length * (1 + rand() % 5));
    progression(start1 + rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 3), length * (1 + rand() % 5));
    progression(start1 + rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 3), length * (1 + rand() % 5));
    progression(start1, 17, length * 10);
    progression(start1 + rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 4), length);
    progression(start1 - rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 2), length);
    progression(start1, 17, length * 10);
    progression(start1 + rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 4), length);
    progression(start1 + rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 4), length * 2);
    progression(start1 - rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 6), length);
    progression(start1, 17, length * 10);
    progression(start1 + rand() % (length / 16 * 17) * 17, 17 * (1 + rand() % 4), length);

	T start2 = rand();
    progression(start2, 25, length * 7);
    progression(start2 + rand() % (length / 8 * 25) * 25, 25 * (1 + rand() % 4), length);
    progression(start2 - rand() % (length / 8 * 25) * 25, 25 * (1 + rand() % 4), length);

	
	T start3 = rand();
    progression(start3, 17, length * 19);
    progression(start3 + rand() % (length / 8 * 17) * 17, 17 * (1 + rand() % 3), length);
    progression(start3 - rand() % (length / 8 * 17) * 17, 17 * (1 + rand() % 3), length);
    progression(start3 - rand() % (length / 8 * 17) * 17, 17 * (1 + rand() % 3), length);
    progression(start3 - rand() % (length / 8 * 17) * 17, 17 * (1 + rand() % 3), length);
    progression(start3 - rand() % (length / 8 * 17) * 17, 17 * (1 + rand() % 4), length);
    progression(start3 - rand() % (length / 8 * 17) * 17, 17 * (1 + rand() % 3), length);
    progression(start3 - rand() % (length / 8 * 17) * 17, 17 * (1 + rand() % 2), length);

	
    total+=(length=BASELEN+nextrandom(1,BASELEN));

    progression(rand(),(-1L),length);

    total+=(length=BASELEN+nextrandom(1,BASELEN));

    randomprogression(rand(),170,10000,length);

    total+=(length=BASELEN+nextrandom(1,BASELEN));

    randominterval(0,(-1L)-5,length); 

    randominterval(0,(-1L)-5,12*BASELEN-total); 
       

    return 0;
}


