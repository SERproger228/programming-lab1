#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "dynamic_array.h"

#include <stddef.h>

typedef struct Polynomial {
    DynamicArray coefficients;
} Polynomial;

Polynomial createPolynomial(size_t degree, const TypeInfo* type);
void freePolynomial(Polynomial* polynomial);

size_t getPolynomialDegree(const Polynomial* polynomial);

void setPolynomialCoefficient(Polynomial* polynomial, size_t power, const void* value);
const void* getPolynomialCoefficient(const Polynomial* polynomial, size_t power);

Polynomial addPolynomials(const Polynomial* left, const Polynomial* right);
Polynomial multiplyPolynomialByScalar(const Polynomial* polynomial, const void* scalar);
Polynomial multiplyPolynomials(const Polynomial* left, const Polynomial* right);
void evaluatePolynomialAt(const Polynomial* polynomial, const void* point, void* result);
Polynomial composePolynomials(const Polynomial* outer, const Polynomial* inner);

void printPolynomial(const Polynomial* polynomial);

#endif
