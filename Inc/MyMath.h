
typedef float Real;
typedef struct _complex{
    Real re;
    Real im;
} Complex;
typedef struct _vec {
    unsigned long long length;
    Complex* arr;
} vec;
typedef vec* vecRef;

Real cAbsolute(Complex a);
Real cArgument(Complex a);
Complex cAdd(Complex a, Complex b);
Complex cSubtract(Complex a, Complex b);
Complex cMultiply(Complex a, Complex b);
Complex cDivide(Complex a, Complex b);
Complex cNegate(Complex a);
Complex cPolarToComplex(Real length, Real angle);

Complex ComplexMake(Real re, Real im);
Real cAbsolute(Complex a);
Real cArgument(Complex a);
Complex cAdd(Complex a, Complex b);
Complex cSubtract(Complex a, Complex b);
Complex cMultiply(Complex a, Complex b);
Complex cDivide(Complex a, Complex b);
Complex cNegate(Complex a);

vecRef vecAlloc(unsigned long long length);
void   vecRelease(vecRef* arr);

void      vadd(vecRef a, vecRef b, vecRef* output);
void vsubtract(vecRef a, vecRef b, vecRef* output);
void vmultiply(vecRef a, vecRef b, vecRef* output);
void   vdivide(vecRef a, vecRef b, vecRef* output);

void vaddScalar(vecRef a, Complex c, vecRef* output);
void vsubScalar(vecRef a, Complex c, vecRef* output);
void vmulScalar(vecRef a, Complex c, vecRef* output);
void vdivScalar(vecRef a, Complex c, vecRef* output);

void fft(const vecRef input, vecRef *output);
void ifft(const vecRef input, vecRef *output);
void dft(const vecRef input, vecRef *output);
void idft(const vecRef input, vecRef *output);
void vecSwap(vecRef *a, vecRef *b);
int checkPowerOfTwo(const unsigned long long n);
unsigned long long reverse(const unsigned long long n, const unsigned long long k);
void permutate(const int q, const vecRef input, vecRef *output);
void compute(const int q, int sign, const vecRef input, vecRef *output);
void computeFFT(const int q, const vecRef input, vecRef *output);
void computeIFFT(const int q, const vecRef input, vecRef *output);