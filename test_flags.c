#include <stdio.h>

typedef enum {
	FIRST_FLAG = 1,
	SECOND_FLAG = 2,
	THIRD_FLAG = 4,
} flags;

int main(){
	int x = FIRST_FLAG | THIRD_FLAG;
	int c = 0;
	printf("%d\n", x);
	if(x & THIRD_FLAG){
		printf("PASSED");
	}
	return 1;
}
