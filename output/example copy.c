#include <stdio.h>

int main() {
    int rows = 5, cols = 10;
    //ohh my god!!!!!!
    for (int i = 1; i <= rows; ++i) {
        for (int j = 1; j <= cols; ++j) {
            printf("(%d,%d) ", i, j);
        }
        printf("\n");
    }
    
    return 0;
}