#include <iostream>
using namespace std;

int sumInts(int data[], int size) {
    int total = 0;
    for (int i = 0; i < size; i++) {
        total = total + data[i];
    }
    return total;
}

template <class T>
T sumArray(T data[], int size) {
    T total = 0;
    for (int i = 0; i < size; i++) {
        total = total + data[i];
    }
    return total;
}

int main() {
    int values[4] = {2, 4, 6, 8};

    int original = sumInts(values, 4);
    int templated = sumArray(values, 4);

    cout << original + templated;
    return 0;
}
