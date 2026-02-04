#include <unistd.h>

int print_int(int i)
{
	int x = i;
	int idx = 0;
	int arr[8] = {0};
	while (x > 0) {
		int q = x / 10;
		int r = x - 10 * q;
		arr[idx] = r + 48;
		idx++;
		x = q;
	}
	write(1, arr, 8);
	return 0;
}

int fib(int n)
{
	if (n <= 1)
		return n;
	return fib(n - 1) + fib(n - 2);
}

int main()
{
	int a = fib(35);
	return print_int(a);
}
