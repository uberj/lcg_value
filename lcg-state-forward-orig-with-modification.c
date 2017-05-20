/* 
 * To compile: gcc lcg-state-forward.c -o lcg-state-forward  
 *
 * This program permutes the internal state of PHP 5.3's LCG (PRNG)
 *
 * For testing against my php-lcg-reverse.c which can reverse internal state
 *
 * -samy kamkar, code@samy.pl, 08/22/09
 *
 */

#include <stdio.h>

#define MODMULT(a, b, c, m, s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m
/* MODMULT computes s*b mod m, provided that m=a*b+c and 0<=c<m */

void modMult_(int *s1, int *s2) {
	int q;
        MODMULT(53668, 40014, 12211, 2147483563L, *s1);
        MODMULT(52774, 40692, 3791, 2147483399L, *s2);
}

void lcg_value(double *z, int *s1, int *s2) {
        modMult_(s1, s2);

        *z = *s1 - *s2;
        if (*z < 1)
                *z += 2147483562;
        *z *= 4.656613e-10;
}

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		printf("usage: %s <s1 [-1251222200]> <s2 [98137]> <times>\n", argv[0]);
		return -1;
	}

	// Our 's' values we're going to reverse
	int q;
	int s1 = atoi(argv[1]);
	int s2 = atoi(argv[2]);
	int total = atoi(argv[3]);
	int times = 0;
	double z;

	while (times++ != total)
	{
                lcg_value(&z, &s1, &s2);
		printf("%d: s1=%d s2=%d lcg_value=%.14f\n", times, s1, s2, z);
	}

	return 0;
}
