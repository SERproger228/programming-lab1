#define _POSIX_C_SOURCE 200809L

#include "dynamic_array.h"
#include "polynomial.h"
#include "type_info.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void assert_int_coefficient(const Polynomial* polynomial, size_t power, int expected)
{
    const int* actual = getPolynomialCoefficient(polynomial, power);

    assert(actual != NULL);
    assert(*actual == expected);
}

static void assert_double_coefficient(const Polynomial* polynomial, size_t power, double expected)
{
    const double* actual = getPolynomialCoefficient(polynomial, power);

    assert(actual != NULL);
    assert(fabs(*actual - expected) < 0.000001);
}

static void test_type_info_int_operations(void)
{
    printf("test_type_info_int_operations... ");

    const TypeInfo* type = getIntTypeInfo();
    int left = 6;
    int right = 3;
    int result = 0;

    assert(type->size == sizeof(int));

    type->add(&result, &left, &right);
    assert(result == 9);

    type->multiply(&result, &left, &right);
    assert(result == 18);

    result = 0;
    assert(type->isZero(&result));

    result = 5;
    assert(!type->isZero(&result));

    printf("OK\n");
}

static void test_type_info_double_operations(void)
{
    printf("test_type_info_double_operations... ");

    const TypeInfo* type = getDoubleTypeInfo();
    double left = 2.5;
    double right = 4.0;
    double result = 0.0;

    assert(type->size == sizeof(double));

    type->add(&result, &left, &right);
    assert(fabs(result - 6.5) < 0.000001);

    type->multiply(&result, &left, &right);
    assert(fabs(result - 10.0) < 0.000001);

    result = 0.0;
    assert(type->isZero(&result));

    result = 0.25;
    assert(!type->isZero(&result));

    printf("OK\n");
}

static void test_dynamic_array_int(void)
{
    printf("test_dynamic_array_int... ");

    DynamicArray array = createDynamicArray(3, getIntTypeInfo());
    int value = 42;
    int* actual;

    assert(array.data != NULL);
    assert(array.size == 3);
    assert(array.type == getIntTypeInfo());

    actual = getDynamicArrayElement(&array, 1);
    assert(actual != NULL);
    assert(*actual == 0);

    setDynamicArrayElement(&array, 1, &value);
    actual = getDynamicArrayElement(&array, 1);
    assert(actual != NULL);
    assert(*actual == 42);

    freeDynamicArray(&array);
    assert(array.data == NULL);
    assert(array.size == 0);
    assert(array.type == NULL);

    printf("OK\n");
}

static void test_dynamic_array_out_of_range(void)
{
    printf("test_dynamic_array_out_of_range... ");

    DynamicArray array = createDynamicArray(2, getIntTypeInfo());
    int value = 99;
    int* element;
    int* firstElement;

    element = getDynamicArrayElement(&array, 2);
    assert(element == NULL);

    setDynamicArrayElement(&array, 2, &value);
    firstElement = getDynamicArrayElement(&array, 0);
    assert(firstElement != NULL);
    assert(*firstElement == 0);

    freeDynamicArray(&array);

    printf("OK\n");
}

static void test_create_polynomial_int(void)
{
    printf("test_create_polynomial_int... ");

    Polynomial polynomial = createPolynomial(2, getIntTypeInfo());
    int a0 = 5;
    int a1 = 2;
    int a2 = 7;

    assert(polynomial.coefficients.data != NULL);
    assert(polynomial.coefficients.size == 3);
    assert(getPolynomialDegree(&polynomial) == 2);

    setPolynomialCoefficient(&polynomial, 0, &a0);
    setPolynomialCoefficient(&polynomial, 1, &a1);
    setPolynomialCoefficient(&polynomial, 2, &a2);

    assert_int_coefficient(&polynomial, 0, 5);
    assert_int_coefficient(&polynomial, 1, 2);
    assert_int_coefficient(&polynomial, 2, 7);

    freePolynomial(&polynomial);

    printf("OK\n");
}

static void test_add_polynomials_int_same_degree(void)
{
    printf("test_add_polynomials_int_same_degree... ");

    Polynomial left = createPolynomial(2, getIntTypeInfo());
    Polynomial right = createPolynomial(2, getIntTypeInfo());
    Polynomial result;
    int valuesLeft[] = {5, 2, 7};
    int valuesRight[] = {1, 3, 4};
    size_t index;

    for (index = 0; index < 3; index++) {
        setPolynomialCoefficient(&left, index, &valuesLeft[index]);
        setPolynomialCoefficient(&right, index, &valuesRight[index]);
    }

    result = addPolynomials(&left, &right);

    assert(result.coefficients.size == 3);
    assert_int_coefficient(&result, 0, 6);
    assert_int_coefficient(&result, 1, 5);
    assert_int_coefficient(&result, 2, 11);

    freePolynomial(&left);
    freePolynomial(&right);
    freePolynomial(&result);

    printf("OK\n");
}

static void test_add_polynomials_int_different_degree(void)
{
    printf("test_add_polynomials_int_different_degree... ");

    Polynomial left = createPolynomial(2, getIntTypeInfo());
    Polynomial right = createPolynomial(1, getIntTypeInfo());
    Polynomial result;
    int leftValues[] = {5, 2, 7};
    int rightValues[] = {1, 3};
    size_t index;

    for (index = 0; index < 3; index++) {
        setPolynomialCoefficient(&left, index, &leftValues[index]);
    }

    for (index = 0; index < 2; index++) {
        setPolynomialCoefficient(&right, index, &rightValues[index]);
    }

    result = addPolynomials(&left, &right);

    assert(result.coefficients.size == 3);
    assert_int_coefficient(&result, 0, 6);
    assert_int_coefficient(&result, 1, 5);
    assert_int_coefficient(&result, 2, 7);

    freePolynomial(&left);
    freePolynomial(&right);
    freePolynomial(&result);

    printf("OK\n");
}

static void test_add_polynomial_with_zero(void)
{
    printf("test_add_polynomial_with_zero... ");

    Polynomial left = createPolynomial(2, getIntTypeInfo());
    Polynomial zero = createPolynomial(0, getIntTypeInfo());
    Polynomial result;
    int values[] = {5, 2, 7};
    size_t index;

    for (index = 0; index < 3; index++) {
        setPolynomialCoefficient(&left, index, &values[index]);
    }

    result = addPolynomials(&left, &zero);

    assert(result.coefficients.size == 3);
    assert_int_coefficient(&result, 0, 5);
    assert_int_coefficient(&result, 1, 2);
    assert_int_coefficient(&result, 2, 7);

    freePolynomial(&left);
    freePolynomial(&zero);
    freePolynomial(&result);

    printf("OK\n");
}

static void test_scalar_multiply_polynomial_int(void)
{
    printf("test_scalar_multiply_polynomial_int... ");

    Polynomial polynomial = createPolynomial(2, getIntTypeInfo());
    Polynomial result;
    int values[] = {5, 2, 7};
    int scalar = 3;
    size_t index;

    for (index = 0; index < 3; index++) {
        setPolynomialCoefficient(&polynomial, index, &values[index]);
    }

    result = multiplyPolynomialByScalar(&polynomial, &scalar);

    assert(result.coefficients.size == 3);
    assert_int_coefficient(&result, 0, 15);
    assert_int_coefficient(&result, 1, 6);
    assert_int_coefficient(&result, 2, 21);

    freePolynomial(&polynomial);
    freePolynomial(&result);

    printf("OK\n");
}

static void test_scalar_multiply_by_zero_and_one(void)
{
    printf("test_scalar_multiply_by_zero_and_one... ");

    Polynomial polynomial = createPolynomial(2, getIntTypeInfo());
    Polynomial zeroResult;
    Polynomial oneResult;
    int values[] = {5, 2, 7};
    int zero = 0;
    int one = 1;
    size_t index;

    for (index = 0; index < 3; index++) {
        setPolynomialCoefficient(&polynomial, index, &values[index]);
    }

    zeroResult = multiplyPolynomialByScalar(&polynomial, &zero);
    oneResult = multiplyPolynomialByScalar(&polynomial, &one);

    assert(zeroResult.coefficients.size == 1);
    assert_int_coefficient(&zeroResult, 0, 0);

    assert(oneResult.coefficients.size == 3);
    assert_int_coefficient(&oneResult, 0, 5);
    assert_int_coefficient(&oneResult, 1, 2);
    assert_int_coefficient(&oneResult, 2, 7);

    freePolynomial(&polynomial);
    freePolynomial(&zeroResult);
    freePolynomial(&oneResult);

    printf("OK\n");
}

static void test_multiply_polynomials_int(void)
{
    printf("test_multiply_polynomials_int... ");

    Polynomial left = createPolynomial(2, getIntTypeInfo());
    Polynomial right = createPolynomial(1, getIntTypeInfo());
    Polynomial result;
    int leftValues[] = {5, 2, 7};
    int rightValues[] = {1, 3};
    size_t index;

    for (index = 0; index < 3; index++) {
        setPolynomialCoefficient(&left, index, &leftValues[index]);
    }

    for (index = 0; index < 2; index++) {
        setPolynomialCoefficient(&right, index, &rightValues[index]);
    }

    result = multiplyPolynomials(&left, &right);

    assert(result.coefficients.size == 4);
    assert_int_coefficient(&result, 0, 5);
    assert_int_coefficient(&result, 1, 17);
    assert_int_coefficient(&result, 2, 13);
    assert_int_coefficient(&result, 3, 21);

    freePolynomial(&left);
    freePolynomial(&right);
    freePolynomial(&result);

    printf("OK\n");
}

static void test_evaluate_polynomial_int(void)
{
    printf("test_evaluate_polynomial_int... ");

    Polynomial polynomial = createPolynomial(2, getIntTypeInfo());
    int values[] = {5, 2, 7};
    int point = 2;
    int result = 0;
    size_t index;

    for (index = 0; index < 3; index++) {
        setPolynomialCoefficient(&polynomial, index, &values[index]);
    }

    evaluatePolynomialAt(&polynomial, &point, &result);
    assert(result == 37);

    freePolynomial(&polynomial);

    printf("OK\n");
}

static void test_evaluate_polynomial_negative_point(void)
{
    printf("test_evaluate_polynomial_negative_point... ");

    Polynomial polynomial = createPolynomial(2, getIntTypeInfo());
    int values[] = {5, 2, 7};
    int point = -1;
    int result = 0;
    size_t index;

    for (index = 0; index < 3; index++) {
        setPolynomialCoefficient(&polynomial, index, &values[index]);
    }

    evaluatePolynomialAt(&polynomial, &point, &result);
    assert(result == 10);

    freePolynomial(&polynomial);

    printf("OK\n");
}

static void test_compose_polynomials_int(void)
{
    printf("test_compose_polynomials_int... ");

    Polynomial outer = createPolynomial(2, getIntTypeInfo());
    Polynomial inner = createPolynomial(1, getIntTypeInfo());
    Polynomial result;
    int outerValues[] = {5, 2, 7};
    int innerValues[] = {1, 3};
    size_t index;

    for (index = 0; index < 3; index++) {
        setPolynomialCoefficient(&outer, index, &outerValues[index]);
    }

    for (index = 0; index < 2; index++) {
        setPolynomialCoefficient(&inner, index, &innerValues[index]);
    }

    result = composePolynomials(&outer, &inner);

    assert(result.coefficients.size == 3);
    assert_int_coefficient(&result, 0, 14);
    assert_int_coefficient(&result, 1, 48);
    assert_int_coefficient(&result, 2, 63);

    freePolynomial(&outer);
    freePolynomial(&inner);
    freePolynomial(&result);

    printf("OK\n");
}

static void test_compose_with_constant_polynomial(void)
{
    printf("test_compose_with_constant_polynomial... ");

    Polynomial outer = createPolynomial(2, getIntTypeInfo());
    Polynomial inner = createPolynomial(0, getIntTypeInfo());
    Polynomial result;
    int outerValues[] = {5, 2, 7};
    int constant = 3;
    size_t index;

    for (index = 0; index < 3; index++) {
        setPolynomialCoefficient(&outer, index, &outerValues[index]);
    }

    setPolynomialCoefficient(&inner, 0, &constant);

    result = composePolynomials(&outer, &inner);

    assert(result.coefficients.size == 1);
    assert_int_coefficient(&result, 0, 74);

    freePolynomial(&outer);
    freePolynomial(&inner);
    freePolynomial(&result);

    printf("OK\n");
}

static void test_polynomial_operations_double(void)
{
    printf("test_polynomial_operations_double... ");

    Polynomial left = createPolynomial(1, getDoubleTypeInfo());
    Polynomial right = createPolynomial(1, getDoubleTypeInfo());
    Polynomial sum;
    Polynomial product;
    double leftValues[] = {1.5, 2.0};
    double rightValues[] = {0.5, 3.0};
    double point = 2.0;
    double value = 0.0;
    size_t index;

    for (index = 0; index < 2; index++) {
        setPolynomialCoefficient(&left, index, &leftValues[index]);
        setPolynomialCoefficient(&right, index, &rightValues[index]);
    }

    sum = addPolynomials(&left, &right);
    product = multiplyPolynomials(&left, &right);
    evaluatePolynomialAt(&left, &point, &value);

    assert_double_coefficient(&sum, 0, 2.0);
    assert_double_coefficient(&sum, 1, 5.0);

    assert_double_coefficient(&product, 0, 0.75);
    assert_double_coefficient(&product, 1, 5.5);
    assert_double_coefficient(&product, 2, 6.0);

    assert(fabs(value - 5.5) < 0.000001);

    freePolynomial(&left);
    freePolynomial(&right);
    freePolynomial(&sum);
    freePolynomial(&product);

    printf("OK\n");
}

static void test_zero_coefficients_are_preserved(void)
{
    printf("test_zero_coefficients_are_preserved... ");

    Polynomial left = createPolynomial(1, getIntTypeInfo());
    Polynomial right = createPolynomial(1, getIntTypeInfo());
    Polynomial result;
    int one = 1;
    int minusOne = -1;

    setPolynomialCoefficient(&left, 0, &one);
    setPolynomialCoefficient(&left, 1, &one);
    setPolynomialCoefficient(&right, 0, &one);
    setPolynomialCoefficient(&right, 1, &minusOne);

    result = multiplyPolynomials(&left, &right);

    assert(result.coefficients.size == 3);
    assert_int_coefficient(&result, 0, 1);
    assert_int_coefficient(&result, 1, 0);
    assert_int_coefficient(&result, 2, -1);

    freePolynomial(&left);
    freePolynomial(&right);
    freePolynomial(&result);

    printf("OK\n");
}

static void test_print_zero_polynomial(void)
{
    printf("test_print_zero_polynomial... ");

    Polynomial polynomial = createPolynomial(0, getIntTypeInfo());
    FILE* capture;
    int savedStdout;
    char buffer[16] = {0};
    size_t readSize;

    capture = tmpfile();
    assert(capture != NULL);

    savedStdout = dup(fileno(stdout));
    assert(savedStdout != -1);

    fflush(stdout);
    assert(dup2(fileno(capture), fileno(stdout)) != -1);

    printPolynomial(&polynomial);

    fflush(stdout);
    assert(dup2(savedStdout, fileno(stdout)) != -1);
    close(savedStdout);

    rewind(capture);
    readSize = fread(buffer, 1, sizeof(buffer) - 1, capture);
    buffer[readSize] = '\0';

    assert(strcmp(buffer, "0") == 0);

    fclose(capture);
    freePolynomial(&polynomial);

    printf("OK\n");
}

static void test_error_handling_returns_empty_polynomial(void)
{
    printf("test_error_handling_returns_empty_polynomial... ");

    Polynomial intPolynomial = createPolynomial(1, getIntTypeInfo());
    Polynomial doublePolynomial = createPolynomial(1, getDoubleTypeInfo());
    Polynomial result;
    int intValue = 1;
    double doubleValue = 1.0;

    setPolynomialCoefficient(&intPolynomial, 0, &intValue);
    setPolynomialCoefficient(&intPolynomial, 1, &intValue);
    setPolynomialCoefficient(&doublePolynomial, 0, &doubleValue);
    setPolynomialCoefficient(&doublePolynomial, 1, &doubleValue);

    result = addPolynomials(NULL, &intPolynomial);
    assert(result.coefficients.data == NULL);
    assert(result.coefficients.size == 0);
    assert(result.coefficients.type == NULL);

    result = multiplyPolynomials(&intPolynomial, &doublePolynomial);
    assert(result.coefficients.data == NULL);
    assert(result.coefficients.size == 0);
    assert(result.coefficients.type == NULL);

    result = multiplyPolynomialByScalar(&intPolynomial, NULL);
    assert(result.coefficients.data == NULL);
    assert(result.coefficients.size == 0);
    assert(result.coefficients.type == NULL);

    freePolynomial(&intPolynomial);
    freePolynomial(&doublePolynomial);

    printf("OK\n");
}

int main(void)
{
    printf("\n========= Running tests =========\n\n");

    test_type_info_int_operations();
    test_type_info_double_operations();
    test_dynamic_array_int();
    test_dynamic_array_out_of_range();
    test_create_polynomial_int();
    test_add_polynomials_int_same_degree();
    test_add_polynomials_int_different_degree();
    test_add_polynomial_with_zero();
    test_scalar_multiply_polynomial_int();
    test_scalar_multiply_by_zero_and_one();
    test_multiply_polynomials_int();
    test_evaluate_polynomial_int();
    test_evaluate_polynomial_negative_point();
    test_compose_polynomials_int();
    test_compose_with_constant_polynomial();
    test_polynomial_operations_double();
    test_zero_coefficients_are_preserved();
    test_print_zero_polynomial();
    test_error_handling_returns_empty_polynomial();

    printf("\n====== All tests passed! ======\n\n");

    return 0;
}
