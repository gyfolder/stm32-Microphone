
#include "MyMath.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "stm32f1xx_hal.h"
Complex ComplexMake(Real re, Real im){
    Complex complex;
    complex.re = re; complex.im = im;
    return complex;
}
Real cAbsolute(Complex a){
    return sqrt(a.re * a.re + a.im * a.im);
}
Real cArgument(Complex a){
    return atan2(a.im, a.re);
}
Complex cAdd(Complex a, Complex b){
    return ComplexMake(a.re + b.re, a.im + b.im);
}
Complex cSubtract(Complex a, Complex b){
    return ComplexMake(a.re - b.re, a.im - b.im);
}
Complex cMultiply(Complex a, Complex b){
    return ComplexMake(a.re * b.re - a.im * b.im, a.re * b.im + b.re * a.im);
}
Complex cDivide(Complex a, Complex b){
    Real abs = cAbsolute(b);
    abs *= abs;
    if (abs < 1.e-30) assert(0);
    return ComplexMake((a.re * b.re + a.im * b.im)/abs, (-a.re * b.im + b.re * a.im)/abs);
}
Complex cNegate(Complex a){
    return ComplexMake(-a.re, -a.im);
}
Complex cPolarToComplex(Real length, Real angle){
    return ComplexMake(length * cos(angle), length * sin(angle));
}
vecRef vecAlloc(unsigned long long length) {
    vecRef vector = (vecRef)malloc(sizeof(vec) * 1);
    vector->length = length;
    vector->arr = (Complex*)calloc(length, sizeof(Complex));
    return vector;
}
void vecRelease(vecRef* arr) {
    if (*arr)
    {
        if ((*arr)->arr) free((*arr)->arr);
        free(*arr);
    }
}

void vadd(vecRef a, vecRef b, vecRef* output) {
    assert(a->length == b->length);
    (*output)->length = a->length;
    for (int i = 0; i < a->length; i++) {
        (*output)->arr[i] = cAdd(a->arr[i], b->arr[i]);
    }
}

void vsubtract(vecRef a, vecRef b, vecRef* output) {
    assert(a->length == b->length);
    (*output)->length = a->length;
    for (int i = 0; i < a->length; i++) {
        (*output)->arr[i] = cSubtract(a->arr[i], b->arr[i]);
    }
}

void vmultiply(vecRef a, vecRef b, vecRef* output) {
    assert(a->length == b->length);
    (*output)->length = a->length;
    for (int i = 0; i < a->length; i++) {
        (*output)->arr[i] = cMultiply(a->arr[i], b->arr[i]);
    }
}

void vdivide(vecRef a, vecRef b, vecRef* output) {
    assert(a->length == b->length);
    (*output)->length = a->length;
    for (int i = 0; i < a->length; i++) {
        (*output)->arr[i] = cDivide(a->arr[i], b->arr[i]);
    }
}

void vaddScalar(vecRef a, Complex c, vecRef* output) {
    (*output)->length = a->length;
    for (int i = 0; i < a->length; i++) {
        (*output)->arr[i] = cAdd(a->arr[i], c);
    }
}

void vsubScalar(vecRef a, Complex c, vecRef* output) {
    (*output)->length = a->length;
    for (int i = 0; i < a->length; i++) {
        (*output)->arr[i] = cSubtract(a->arr[i], c);
    }
}

void vmulScalar(vecRef a, Complex c, vecRef* output) {
    (*output)->length = a->length;
    for (int i = 0; i < a->length; i++) {
        (*output)->arr[i] = cMultiply(a->arr[i], c);
    }
}

void vdivScalar(vecRef a, Complex c, vecRef* output) {
    (*output)->length = a->length;
    for (int i = 0; i < a->length; i++) {
        (*output)->arr[i] = cDivide(a->arr[i], c);
    }
}
void fft(const vecRef input, vecRef *output){
    assert(input->length >= 2);
    unsigned long long N = input->length;
    /*if (checkPowerOfTwo(N) != 1)
    {
        N = 1 << (int)(round(log2(N))+1);
    }*/
    int rounds = (int)log2(N);
    
    // If output is null, we make it
    // If output is not null and if it is not power of 2, we reallocate it
    // Else, use output
    if (*output == NULL || (*output)->length != N)
    {
        vecRelease(output);
        *output = vecAlloc(N);
    }
    
    permutate(rounds, input, output);
	//vecRef tmp = vecAlloc(N);
	//memcpy(tmp->arr, (*output)->arr, sizeof(Complex) * tmp->length);
	for (int q = rounds - 1; q >= 0; q--)
	{
		//vecSwap(output, &tmp);
		computeFFT(q, *output, output);
	}
	//vecRelease(&tmp);
    vdivScalar(*output, ComplexMake(sqrt(N), 0.0), output);
}
void ifft(const vecRef input, vecRef *output){
    assert(input->length >= 2);
    unsigned long long N = input->length;
    if (checkPowerOfTwo(N) != 1)
    {
        N = 1 << (int)(round(log2(N))+1);
    }
    int rounds = (int)log2(N);
    
    // If output is null, we make it
    // If output is not null and if it is not power of 2, we reallocate it
    // Else, use output
    if (*output == NULL || (*output)->length != N)
    {
        vecRelease(output);
        *output = vecAlloc(N);
    }
    
    permutate(rounds, input, output);
	//vecRef tmp = vecAlloc(N);
	//memcpy(tmp->arr, (*output)->arr, sizeof(Complex) * tmp->length);
	for (int q = rounds - 1; q >= 0; q--)
	{
		//vecSwap(output, &tmp);
		computeFFT(q, *output, output);
	}
	//vecRelease(&tmp);
    vdivScalar(*output, ComplexMake(sqrt(N), 0.0), output);
}
void dft(const vecRef input, vecRef *output){
    assert(input->length >= 2);
    unsigned long long N = input->length;
    
    // If output is null, we make it
    // If output is not null and if it is not power of 2, we reallocate it
    // Else, use output
    if (*output == NULL || (*output)->length != N)
    {
        vecRelease(output);
        *output = vecAlloc(N);
    }
    
    for (unsigned long long i = 0; i < N; i++)
    {
        Complex tmp = ComplexMake(0, 0);
        for (unsigned long long j = 0; j < N; j++)
        {
            Complex wj = cPolarToComplex(1.0, -(2.0*M_PI*i*j)/N);
            tmp = cAdd(tmp, cMultiply(wj, input->arr[j]));
        }
        (*output)->arr[i] = tmp;
    }
    vdivScalar(*output, ComplexMake(sqrt(N), 0.0), output);
}
void idft(const vecRef input, vecRef *output){
    assert(input->length >= 2);
    unsigned long long N = input->length;
    
    // If output is null, we make it
    // If output is not null and if it is not power of 2, we reallocate it
    // Else, use output
    if (*output == NULL || (*output)->length != N)
    {
        vecRelease(output);
        *output = vecAlloc(N);
    }
    
    for (unsigned long long i = 0; i < N; i++)
    {
        Complex tmp = ComplexMake(0, 0);
        for (unsigned long long j = 0; j < N; j++)
        {
            Complex wj = cPolarToComplex(1.0, (2.0*M_PI*i*j)/N);
            tmp = cAdd(tmp, cMultiply(wj, input->arr[j]));
        }
        (*output)->arr[i] = tmp;
    }
    vdivScalar(*output, ComplexMake(sqrt(N), 0.0), output);
}
void vecSwap(vecRef *a, vecRef *b){
    vecRef tmp = *a;
    *a = *b;
    *b = tmp;
}
int checkPowerOfTwo(const unsigned long long n){
    unsigned long long k = n;
    while(k > 1)
    {
        if (k % 2 == 1) return 0;
        k /= 2;
    }
    return 1;
}
// http://stackoverflow.com/a/34236981
unsigned long long reverse(const unsigned long long n, const unsigned long long k){
    unsigned long long r, i;
    for (r = 0, i = 0; i < k; ++i)
        r |= ((n >> i) & 1) << (k - i - 1);
    return r;
}
void permutate(const int q, const vecRef input, vecRef *output){
    for (unsigned long long i = 0; i < (1 << q); i++)
    {
        (*output)->arr[i] = input->arr[reverse(i, q)];
    }
}
void compute(const int q, int sign, const vecRef input, vecRef *output){
    const unsigned long long countBlock = 1 << q;
    const unsigned long long lenBlock = (*output)->length / countBlock;
    if (sign >= 0) sign = 1;
    if (sign < 0) sign = -1;
    
    for (unsigned long long baseIdx = 0; baseIdx < lenBlock/2; baseIdx++)
    {
        const Complex w = cPolarToComplex(1.0, sign * (M_PI * baseIdx)/(lenBlock/2));
        for (unsigned long long block = 0; block < countBlock; block++)
        {
            const unsigned long long idx = baseIdx + block * lenBlock;
            const Complex a = input->arr[idx];
            const Complex b = input->arr[idx + lenBlock/2];
            const Complex wB = cMultiply(w, b);
            // Upper part
            (*output)->arr[idx] = cAdd(a, wB);
            // Lower part
            (*output)->arr[idx + lenBlock/2] = cSubtract(a, wB);
        }
    }
}
void computeFFT(const int q, const vecRef input, vecRef *output){
    compute(q, -1, input, output);
}
void computeIFFT(const int q, const vecRef input, vecRef *output){
    compute(q, 1, input, output);
}