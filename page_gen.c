#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

char* my_itoa(int num, char* str)
{
	if(str == NULL)
		return NULL;

	sprintf(str, "%d", num);

	return str;
}

void big_file()
{
	FILE* fp;
	fp = fopen("accesses_2.txt","w");

	srand((long)time(NULL));

 	int size, num;
	char space[1] = "\n";
	char snum[4];
	
	
	while(size < 5489856)
	{
		num = (rand()%(999999 - 1000) + 0);
		my_itoa(num, snum);
		fprintf(fp,space,"%s");
		size++;
	}

	fclose(fp);
}

void small_file()
{
	FILE* fp;
	fp = fopen("accesses_1.txt","w");

	srand((long)time(NULL));

 	int size, num;
	char space[1] = "\n";
	char snum[2];
	
	
	while(size < 5489856)
	{
		num = (rand()%(99 - 0) + 0);
		my_itoa(num, snum);
		fprintf(fp,space,"%s");
		size++;
	}
	fclose(fp);
}

int main(int argc, char* argv[])
{
	small_file();
	big_file();

	return 0;
}


