#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("In square.c\n");
    int num;
    // Read the integer from standard input
    scanf("%d", &num);
    // Calculate square and print the result
    printf("%d\n", num * num);
    return 0;
}
