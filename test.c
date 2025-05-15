
#include <stdio.h>
typedef struct HelloWorld
{
    int* array;
} HelloWorld; 


#define alloc_buffer(array, buffer)\
    array = buffer; \

int main()
{
    HelloWorld helloWorld; 
    int buffer[] = {1, 2, 4};
    alloc_buffer(helloWorld.array, buffer); 
    for(int i = 0; i < 3; i++)
        printf("%d\n", helloWorld.array[i]);

    return 0; 
}

