#include<stdio.h>
#include<stdlib.h>

int f(int b){
    if(b<0)
        return 0;
    return 1;    
}

int main(){
    char c[10] = {'a', 'b','c','d', '\0', 'e', 'f', 'g','\0', 'h'};
    printf("%s", c);
    //printf("%d", f(-2));
}






