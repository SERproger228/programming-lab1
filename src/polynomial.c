#include "polynomial.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Polynomial createEmptyPolynomial(void)
{
    Polynomial polynomial;

    polynomial.coefficients.data = NULL;
    polynomial.coefficients.size = 0;
    polynomial.coefficients.type = NULL;

    return polynomial;
}

static void trimTrailingZeroCoefficients(Polynomial* polynomial)
{
    size_t newSize;
    void* newData;

    if (polynomial == NULL) {
        fprintf(stderr, "trimTrailingZeroCoefficients: polynomial is NULL\n");
        return;
    }

    if (polynomial->coefficients.type == NULL) {
        fprintf(stderr, "trimTrailingZeroCoefficients: polynomial type is NULL\n");
        return;
    }

    if (polynomial->coefficients.data == NULL) {
        fprintf(stderr, "trimTrailingZeroCoefficients: coefficients data is NULL\n");
        return;
    }

    if (polynomial->coefficients.type->isZero == NULL) {
        fprintf(stderr, "trimTrailingZeroCoefficients: type isZero function is NULL\n");
        return;
    }

    newSize = polynomial->coefficients.size;

    while (newSize > 1) {
        const void* coefficient = getPolynomialCoefficient(polynomial, newSize - 1);

        if (coefficient == NULL) {
            return;
        }

        if (!polynomial->coefficients.type->isZero(coefficient)) {
            break;
        }

        newSize--;
    }

    if (newSize == polynomial->coefficients.size) {
        return;
    }

    newData = calloc(newSize, polynomial->coefficients.type->size);

    if (newData == NULL) {
        fprintf(stderr, "trimTrailingZeroCoefficients: memory allocation failed\n");
        return;
    }

    memcpy(newData, polynomial->coefficients.data, newSize * polynomial->coefficients.type->size);
    free(polynomial->coefficients.data);
    polynomial->coefficients.data = newData;
    polynomial->coefficients.size = newSize;
}

Polynomial createPolynomial(size_t degree, const TypeInfo* type)
{
    Polynomial polynomial;

    polynomial.coefficients = createDynamicArray(degree + 1, type);

    return polynomial;
}

void freePolynomial(Polynomial* polynomial)
{
    if (polynomial == NULL) {
        return;
    }

    freeDynamicArray(&polynomial->coefficients);
}

size_t getPolynomialDegree(const Polynomial* polynomial)
{
    if (polynomial == NULL) {
        fprintf(stderr, "getPolynomialDegree: polynomial is NULL\n");
        return 0;
    }

    if (polynomial->coefficients.size == 0) {
        fprintf(stderr, "getPolynomialDegree: coefficients size is 0\n");
        return 0;
    }

    return polynomial->coefficients.size - 1;
}

void setPolynomialCoefficient(Polynomial* polynomial, size_t power, const void* value)
{
    if (polynomial == NULL) {
        fprintf(stderr, "setPolynomialCoefficient: polynomial is NULL\n");
        return;
    }

    setDynamicArrayElement(&polynomial->coefficients, power, value);
}

const void* getPolynomialCoefficient(const Polynomial* polynomial, size_t power)
{
    if (polynomial == NULL) {
        fprintf(stderr, "getPolynomialCoefficient: polynomial is NULL\n");
        return NULL;
    }

    return getDynamicArrayConstElement(&polynomial->coefficients, power);
}

Polynomial addPolynomials(const Polynomial* left, const Polynomial* right)
{
    Polynomial result;
    size_t maxSize;
    size_t index;
    void* sum;

    if (left == NULL) {
        fprintf(stderr, "addPolynomials: left polynomial is NULL\n");
        return createEmptyPolynomial();
    }

    if (right == NULL) {
        fprintf(stderr, "addPolynomials: right polynomial is NULL\n");
        return createEmptyPolynomial();
    }

    if (left->coefficients.type == NULL) {
        fprintf(stderr, "addPolynomials: left polynomial type is NULL\n");
        return createEmptyPolynomial();
    }

    if (right->coefficients.type == NULL) {
        fprintf(stderr, "addPolynomials: right polynomial type is NULL\n");
        return createEmptyPolynomial();
    }

    if (left->coefficients.data == NULL) {
        fprintf(stderr, "addPolynomials: left coefficients data is NULL\n");
        return createEmptyPolynomial();
    }

    if (right->coefficients.data == NULL) {
        fprintf(stderr, "addPolynomials: right coefficients data is NULL\n");
        return createEmptyPolynomial();
    }

    if (left->coefficients.type != right->coefficients.type) {
        fprintf(stderr, "addPolynomials: polynomial types are different\n");
        return createEmptyPolynomial();
    }

    if (left->coefficients.size > right->coefficients.size) {
        maxSize = left->coefficients.size;
    } else {
        maxSize = right->coefficients.size;
    }

    result = createPolynomial(maxSize - 1, left->coefficients.type);

    if (result.coefficients.data == NULL) {
        fprintf(stderr, "addPolynomials: result coefficients data is NULL\n");
        return result;
    }

    sum = malloc(left->coefficients.type->size);

    if (sum == NULL) {
        fprintf(stderr, "addPolynomials: memory allocation failed\n");
        freePolynomial(&result);
        return createEmptyPolynomial();
    }

    for (index = 0; index < maxSize; index++) {
        const void* leftCoefficient = NULL;
        const void* rightCoefficient = NULL;

        if (index < left->coefficients.size) {
            leftCoefficient = getPolynomialCoefficient(left, index);
        }

        if (index < right->coefficients.size) {
            rightCoefficient = getPolynomialCoefficient(right, index);
        }

        if (leftCoefficient != NULL && rightCoefficient != NULL) {
            left->coefficients.type->add(sum, leftCoefficient, rightCoefficient);
            setPolynomialCoefficient(&result, index, sum);
        } else if (leftCoefficient != NULL) {
            setPolynomialCoefficient(&result, index, leftCoefficient);
        } else if (rightCoefficient != NULL) {
            setPolynomialCoefficient(&result, index, rightCoefficient);
        }
    }

    free(sum);

    trimTrailingZeroCoefficients(&result);

    return result;
}

Polynomial multiplyPolynomialByScalar(const Polynomial* polynomial, const void* scalar)
{
    Polynomial result;
    size_t index;
    void* product;

    if (polynomial == NULL) {
        fprintf(stderr, "multiplyPolynomialByScalar: polynomial is NULL\n");
        return createEmptyPolynomial();
    }

    if (polynomial->coefficients.type == NULL) {
        fprintf(stderr, "multiplyPolynomialByScalar: polynomial type is NULL\n");
        return createEmptyPolynomial();
    }

    if (polynomial->coefficients.data == NULL) {
        fprintf(stderr, "multiplyPolynomialByScalar: coefficients data is NULL\n");
        return createEmptyPolynomial();
    }

    if (scalar == NULL) {
        fprintf(stderr, "multiplyPolynomialByScalar: scalar is NULL\n");
        return createEmptyPolynomial();
    }

    result = createPolynomial(getPolynomialDegree(polynomial), polynomial->coefficients.type);

    if (result.coefficients.data == NULL) {
        fprintf(stderr, "multiplyPolynomialByScalar: result coefficients data is NULL\n");
        return result;
    }

    product = malloc(polynomial->coefficients.type->size);

    if (product == NULL) {
        fprintf(stderr, "multiplyPolynomialByScalar: memory allocation failed\n");
        freePolynomial(&result);
        return createEmptyPolynomial();
    }

    for (index = 0; index < polynomial->coefficients.size; index++) {
        const void* coefficient = getPolynomialCoefficient(polynomial, index);

        if (coefficient == NULL) {
            free(product);
            freePolynomial(&result);
            return createEmptyPolynomial();
        }

        polynomial->coefficients.type->multiply(product, coefficient, scalar);
        setPolynomialCoefficient(&result, index, product);
    }

    free(product);

    trimTrailingZeroCoefficients(&result);

    return result;
}

Polynomial multiplyPolynomials(const Polynomial* left, const Polynomial* right)
{
    Polynomial result;
    size_t resultDegree;
    size_t leftIndex;
    void* product;
    void* sum;

    if (left == NULL) {
        fprintf(stderr, "multiplyPolynomials: left polynomial is NULL\n");
        return createEmptyPolynomial();
    }

    if (right == NULL) {
        fprintf(stderr, "multiplyPolynomials: right polynomial is NULL\n");
        return createEmptyPolynomial();
    }

    if (left->coefficients.type == NULL) {
        fprintf(stderr, "multiplyPolynomials: left polynomial type is NULL\n");
        return createEmptyPolynomial();
    }

    if (right->coefficients.type == NULL) {
        fprintf(stderr, "multiplyPolynomials: right polynomial type is NULL\n");
        return createEmptyPolynomial();
    }

    if (left->coefficients.data == NULL) {
        fprintf(stderr, "multiplyPolynomials: left coefficients data is NULL\n");
        return createEmptyPolynomial();
    }

    if (right->coefficients.data == NULL) {
        fprintf(stderr, "multiplyPolynomials: right coefficients data is NULL\n");
        return createEmptyPolynomial();
    }

    if (left->coefficients.type != right->coefficients.type) {
        fprintf(stderr, "multiplyPolynomials: polynomial types are different\n");
        return createEmptyPolynomial();
    }

    resultDegree = getPolynomialDegree(left) + getPolynomialDegree(right);
    result = createPolynomial(resultDegree, left->coefficients.type);

    if (result.coefficients.data == NULL) {
        fprintf(stderr, "multiplyPolynomials: result coefficients data is NULL\n");
        return result;
    }

    product = malloc(left->coefficients.type->size);

    if (product == NULL) {
        fprintf(stderr, "multiplyPolynomials: product memory allocation failed\n");
        freePolynomial(&result);
        return createEmptyPolynomial();
    }

    sum = malloc(left->coefficients.type->size);

    if (sum == NULL) {
        fprintf(stderr, "multiplyPolynomials: sum memory allocation failed\n");
        free(product);
        freePolynomial(&result);
        return createEmptyPolynomial();
    }

    for (leftIndex = 0; leftIndex < left->coefficients.size; leftIndex++) {
        size_t rightIndex;
        const void* leftCoefficient = getPolynomialCoefficient(left, leftIndex);

        if (leftCoefficient == NULL) {
            free(product);
            free(sum);
            freePolynomial(&result);
            return createEmptyPolynomial();
        }

        for (rightIndex = 0; rightIndex < right->coefficients.size; rightIndex++) {
            size_t resultIndex = leftIndex + rightIndex;
            const void* rightCoefficient = getPolynomialCoefficient(right, rightIndex);
            const void* currentResultCoefficient;

            if (rightCoefficient == NULL) {
                free(product);
                free(sum);
                freePolynomial(&result);
                return createEmptyPolynomial();
            }

            currentResultCoefficient = getPolynomialCoefficient(&result, resultIndex);

            if (currentResultCoefficient == NULL) {
                free(product);
                free(sum);
                freePolynomial(&result);
                return createEmptyPolynomial();
            }

            left->coefficients.type->multiply(product, leftCoefficient, rightCoefficient);
            left->coefficients.type->add(sum, currentResultCoefficient, product);
            setPolynomialCoefficient(&result, resultIndex, sum);
        }
    }

    free(product);
    free(sum);

    trimTrailingZeroCoefficients(&result);

    return result;
}

void evaluatePolynomialAt(const Polynomial* polynomial, const void* point, void* result)
{
    size_t index;
    void* product;
    void* sum;

    if (polynomial == NULL) {
        fprintf(stderr, "evaluatePolynomialAt: polynomial is NULL\n");
        return;
    }

    if (polynomial->coefficients.type == NULL) {
        fprintf(stderr, "evaluatePolynomialAt: polynomial type is NULL\n");
        return;
    }

    if (polynomial->coefficients.data == NULL) {
        fprintf(stderr, "evaluatePolynomialAt: coefficients data is NULL\n");
        return;
    }

    if (point == NULL) {
        fprintf(stderr, "evaluatePolynomialAt: point is NULL\n");
        return;
    }

    if (result == NULL) {
        fprintf(stderr, "evaluatePolynomialAt: result is NULL\n");
        return;
    }

    product = malloc(polynomial->coefficients.type->size);

    if (product == NULL) {
        fprintf(stderr, "evaluatePolynomialAt: product memory allocation failed\n");
        return;
    }

    sum = malloc(polynomial->coefficients.type->size);

    if (sum == NULL) {
        fprintf(stderr, "evaluatePolynomialAt: sum memory allocation failed\n");
        free(product);
        return;
    }

    memset(result, 0, polynomial->coefficients.type->size);

    for (index = polynomial->coefficients.size; index > 0; index--) {
        const void* coefficient = getPolynomialCoefficient(polynomial, index - 1);

        if (coefficient == NULL) {
            free(product);
            free(sum);
            return;
        }

        polynomial->coefficients.type->multiply(product, result, point);
        polynomial->coefficients.type->add(sum, product, coefficient);
        memcpy(result, sum, polynomial->coefficients.type->size);
    }

    free(product);
    free(sum);
}

Polynomial composePolynomials(const Polynomial* outer, const Polynomial* inner)
{
    Polynomial result;
    size_t index;

    if (outer == NULL) {
        fprintf(stderr, "composePolynomials: outer polynomial is NULL\n");
        return createEmptyPolynomial();
    }

    if (inner == NULL) {
        fprintf(stderr, "composePolynomials: inner polynomial is NULL\n");
        return createEmptyPolynomial();
    }

    if (outer->coefficients.type == NULL) {
        fprintf(stderr, "composePolynomials: outer polynomial type is NULL\n");
        return createEmptyPolynomial();
    }

    if (inner->coefficients.type == NULL) {
        fprintf(stderr, "composePolynomials: inner polynomial type is NULL\n");
        return createEmptyPolynomial();
    }

    if (outer->coefficients.data == NULL) {
        fprintf(stderr, "composePolynomials: outer coefficients data is NULL\n");
        return createEmptyPolynomial();
    }

    if (inner->coefficients.data == NULL) {
        fprintf(stderr, "composePolynomials: inner coefficients data is NULL\n");
        return createEmptyPolynomial();
    }

    if (outer->coefficients.type != inner->coefficients.type) {
        fprintf(stderr, "composePolynomials: polynomial types are different\n");
        return createEmptyPolynomial();
    }

    result = createPolynomial(0, outer->coefficients.type);

    if (result.coefficients.data == NULL) {
        fprintf(stderr, "composePolynomials: result coefficients data is NULL\n");
        return result;
    }

    for (index = outer->coefficients.size; index > 0; index--) {
        Polynomial multiplied;
        Polynomial coefficientPolynomial;
        Polynomial nextResult;
        const void* coefficient = getPolynomialCoefficient(outer, index - 1);

        if (coefficient == NULL) {
            freePolynomial(&result);
            return createEmptyPolynomial();
        }

        multiplied = multiplyPolynomials(&result, inner);
        coefficientPolynomial = createPolynomial(0, outer->coefficients.type);

        if (coefficientPolynomial.coefficients.data == NULL) {
            freePolynomial(&result);
            freePolynomial(&multiplied);
            return createEmptyPolynomial();
        }

        setPolynomialCoefficient(&coefficientPolynomial, 0, coefficient);
        nextResult = addPolynomials(&multiplied, &coefficientPolynomial);

        freePolynomial(&result);
        freePolynomial(&multiplied);
        freePolynomial(&coefficientPolynomial);

        result = nextResult;
    }

    trimTrailingZeroCoefficients(&result);

    return result;
}

void printPolynomial(const Polynomial* polynomial)
{
    size_t index;
    int printedAnyCoefficient = 0;

    if (polynomial == NULL) {
        fprintf(stderr, "printPolynomial: polynomial is NULL\n");
        return;
    }

    if (polynomial->coefficients.type == NULL) {
        fprintf(stderr, "printPolynomial: polynomial type is NULL\n");
        return;
    }

    if (polynomial->coefficients.data == NULL) {
        fprintf(stderr, "printPolynomial: coefficients data is NULL\n");
        return;
    }

    if (polynomial->coefficients.type->isZero == NULL) {
        fprintf(stderr, "printPolynomial: type isZero function is NULL\n");
        return;
    }

    for (index = 0; index < polynomial->coefficients.size; index++) {
        const void* coefficient = getPolynomialCoefficient(polynomial, index);

        if (coefficient == NULL) {
            return;
        }

        if (polynomial->coefficients.type->isZero(coefficient)) {
            continue;
        }

        if (printedAnyCoefficient) {
            printf(" + ");
        }

        polynomial->coefficients.type->print(coefficient);
        printedAnyCoefficient = 1;

        if (index > 0) {
            printf("*x");
        }

        if (index > 1) {
            printf("^%zu", index);
        }
    }

    if (!printedAnyCoefficient) {
        printf("0");
    }
}
