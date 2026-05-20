#include <iostream>
#include <string>
using namespace std;

template <class Summable>
Summable sumArray(Summable data[], int size) {
    Summable total = data[0];
    for (int i = 1; i < size; i++) {
        total += data[i];
    }
    return total;
}

struct Label {
    string text;

    Label& operator+=(const Label& other) {
        text = text + "-" + other.text;
        return *this;
    }
};

int main() {
    Label items[3];
    items[0].text = "A";
    items[1].text = "B";
    items[2].text = "C";

    Label result = sumArray(items, 3);
    cout << result.text;
    return 0;
}
