
#include <stdio.h>
typedef struct HelloWorld
{
    int* array;
} HelloWorld; 

int main()
{
    HelloWorld helloWorld; 
    helloWorld.array[32];
    for(int i = 0; i < 32; i++)
        helloWorld.array[i] = i;
    
    for(int i = 0; i < 32; i++)
        printf("%d\n", helloWorld.array[i]);
}
