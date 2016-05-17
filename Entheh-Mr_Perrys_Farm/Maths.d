module Maths;

int divRoundDown(int a, int b) {
	int c = (a >> 31) & (b - 1);
	return (a - c) / b + c;
}

int modUnsigned(int a, int b) {
	int c = a % b;
	return c + ((c >> 31) & b);
}

int diffWrapped(int a, int b, int modulo) {
	int h = modulo/2;
	return (a - b + h).modUnsigned(modulo) - h;
}
