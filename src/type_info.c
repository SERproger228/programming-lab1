#include "type_info.h"

#include <stdio.h>

static void intAdd(void* result, const void* a, const void* b)
{
    *(int*)result = *(const int*)a + *(const int*)b;
}

static void intMultiply(void* result, const void* a, const void* b)
{
    *(int*)result = *(const int*)a * *(const int*)b;
}

static void intPrint(const void* value)
{
    printf("%d", *(const int*)value);
}

static int intIsZero(const void* value)
{
    return *(const int*)value == 0;
}

static void doubleAdd(void* result, const void* a, const void* b)
{
    *(double*)result = *(const double*)a + *(const double*)b;
}

static void doubleMultiply(void* result, const void* a, const void* b)
{
    *(double*)result = *(const double*)a * *(const double*)b;
}

static void doublePrint(const void* value)
{
    printf("%.2lf", *(const double*)value);
}

static int doubleIsZero(const void* value)
{
    return *(const double*)value == 0.0;
}

const TypeInfo* getIntTypeInfo(void)
{
    static const TypeInfo type = {
        sizeof(int),
        intAdd,
        intMultiply,
        intPrint,
        intIsZero
    };

    return &type;
}

const TypeInfo* getDoubleTypeInfo(void)
{
    static const TypeInfo type = {
        sizeof(double),
        doubleAdd,
        doubleMultiply,
        doublePrint,
        doubleIsZero
    };

    return &type;
}
