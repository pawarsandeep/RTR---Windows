#include <stdio.h>

extern "C" void makeSquare(float num)
{
	printf("Shared Object Func:: The Square is: %f\n", num*num);
}
