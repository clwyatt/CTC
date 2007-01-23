#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
int flag;
long from = atoi(argv[1]);

printf("%d\n", from);
	
while(1) 
{
long i;
flag = 1;
	for (i = 2;i< from/2; i++)
	{
	if( from % i == 0)
		{
		from ++;
		flag = 0; 
		break;
		}
	}	
	if(flag==1)	{
	printf("this primenumber is %d", from );
	return 0;
	}
	else {
	printf("%d\n", from);
	}
}	
return 1;
}
