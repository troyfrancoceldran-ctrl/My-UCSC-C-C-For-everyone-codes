#include <stdio.h>

#define SIZE 5 // Define SIZE as a macro constant

// I. SORTING FUNCTIONS
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void print_array(int how_many, int data[], char *str){
    int i;
    printf("%s", str);

    for (i = 0; i < how_many; i++){
        printf("%d\t", data[i]);
    }
}

void bubble(int data[], int how_many){
    int i, j;
    int go_on;

    for(i = 0; i < how_many - 1; i++){ // Outer loop runs n-1 times
        print_array(how_many, data, "\ninside bubble\n");
        printf("i = %d, input any integer to continue: ", i);
        scanf("%d", &go_on); 
        
        // Inner loop: compares adjacent elements (j) and (j+1)
        for(j = 0; j < how_many - 1 - i; j++){
            if(data[j] > data[j+1]) {
                swap(&data[j], &data[j+1]);
            }
        }
    }
}

// II. MAIN FUNCTION
int main(void) {
    // REMOVED 'const int SIZE = 5;'
    int grades[SIZE] = {78, 67, 92, 83, 88};

    print_array(SIZE, grades, "My Grades \n");
    printf("\n\n");
    bubble(grades, SIZE);
    print_array(SIZE, grades, "My sorted grades\n");
    printf("\n\n");
    return 0;
}