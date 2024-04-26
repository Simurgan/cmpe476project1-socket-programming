#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("In cube.c\n");
    int num;
    // Read the integer from standard input
    scanf("%d", &num);
    // Calculate cube and print the result
    printf("%d\n", num * num * num);
    return 0;
}