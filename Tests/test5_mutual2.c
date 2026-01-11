void foo(int n);
int target_func(int n) {
    if (n > 0) foo(n - 1);
    return n;
}
void foo(int n) {
    target_func(n-1);
}
int main() {
    foo(5); // target(1) calls foo(0) which calls target(0)
    return 0;
}

/* what happens here?
foo -> 5
    target_func -> 4
        foo -> 3
        target_func -> 2
            foo -> 1
            target_func -> 0
            return 0
            return 1
        return 2
        return 3
    return 4
*/
