#include <stdio.h>
#include <math.h>
#include <sys/time.h>

double get_wtime(void){
	struct timeval t;
	gettimeofday(&t, NULL);
	return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}

double f(double x){
	return log(x)*sqrt(x);
}

int main(int argc, char *argv[]){
	double a = 1.0;
	double b = 4.0;
	unsigned long const n = 1e9;
	const double dx = (b-a)/n;
	double S = 0;
	double t0 = get_wtime();

	for (unsigned long i = 0; i < n; i++) {
		double xi = a + (i + 0.5)*dx;
		S += f(xi);
	}

	S *= dx;
	double t1 = get_wtime();
	printf("Time=%lf seconds, Result=%.8f\n", t1-t0, S);
	return 0;
}
