#include <stdio.h>
int target_func(int x) { return x * 2; }
int main() {
    target_func(5); // run #1
    target_func(10); // run #2
    return 0;
}