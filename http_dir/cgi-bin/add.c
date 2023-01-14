#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int add(int a, int b) {
    return a + b;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("usage: add(int a, int b) takes 2 parameters\n");
        return 1;
    }
    int v1 = atoi(argv[1]);
    int v2 = atoi(argv[2]);
    printf("cgi program add(%d, %d) = %d\n", v1, v2, add(v1, v2));
    return 0;
}