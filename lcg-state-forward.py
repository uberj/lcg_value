
#define MODMULT(a, b, c, m, s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m
# /* MODMULT computes s*b mod m, provided that m=a*b+c and 0<=c<m */

# void modMult_(int *s1, int *s2) {
	# int q;
        # MODMULT(53668, 40014, 12211, 2147483563L, *s1);
        # MODMULT(52774, 40692, 3791, 2147483399L, *s2);
# }

# void lcg_value(double *z, int *s1, int *s2) {
        # modMult_(s1, s2);

        # *z = *s1 - *s2;
        # if (*z < 1)
                # *z += 2147483562;
        # *z *= 4.656613e-10;
# }
def modmult(a, b, c, m, s):
    q1 = s // a
    s = b * (s - a * q) - c * q
    if s1 < 0:
        s = s + m
    return s

def lcg_value(z, s1, s2):
    #define MODMULT(a, b, c, m, s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m
    s1 = modmult(53668, 40014, 12211, 2147483563L, s1)
    s2 = modmult(52774, 40692, 3791, 2147483399L, s2)
    z = s1 - s2;
    if (z < 1)
        z += 2147483562;
    z *= 4.656613e-10;
    return s1, s2

def main():
    s1 = sys.argv[1]
    s2 = sys.argv[2]
    total = sys.argv[3]

    for i in xrange(total):
        lcg_value()

if __name__ == "__main__":
    main()
