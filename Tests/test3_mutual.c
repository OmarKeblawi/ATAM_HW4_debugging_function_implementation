void foo(int n);
int target_func(int n) {
    if (n > 0) foo(n - 1);
    return n;
}
void foo(int n) {
    target_func(n);
}
int main() {
    target_func(1); // target(1) calls foo(0) which calls target(0)
    return 0;
}