int target_func(int n) {
    if (n <= 0) return 100;
    return target_func(n - 1) + 1;
}
int main() {
    target_func(3); 
    return 0;
}