#include "dynamic_array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DynamicArray createDynamicArray(size_t size, const TypeInfo* type)
{
    DynamicArray array;

    array.size = size;
    array.type = type;
    array.data = NULL;

    if (type == NULL) {
        fprintf(stderr, "createDynamicArray: type is NULL\n");
        array.size = 0;

        return array;
    }

    array.data = calloc(size, type->size);

    if (size > 0 && array.data == NULL) {
        fprintf(stderr, "createDynamicArray: memory allocation failed\n");
        array.size = 0;
        array.type = NULL;
    }

    return array;
}

void freeDynamicArray(DynamicArray* array)
{
    free(array->data);

    array->data = NULL;
    array->size = 0;
    array->type = NULL;
}

void* getDynamicArrayElement(DynamicArray* array, size_t index)
{
    if (array == NULL) {
        fprintf(stderr, "getDynamicArrayElement: array is NULL\n");
        return NULL;
    }

    if (array->data == NULL) {
        fprintf(stderr, "getDynamicArrayElement: array data is NULL\n");
        return NULL;
    }

    if (array->type == NULL) {
        fprintf(stderr, "getDynamicArrayElement: array type is NULL\n");
        return NULL;
    }

    if (index >= array->size) {
        fprintf(stderr, "getDynamicArrayElement: index out of range\n");
        return NULL;
    }

    return (char*)array->data + index * array->type->size;
}

const void* getDynamicArrayConstElement(const DynamicArray* array, size_t index)
{
    if (array == NULL) {
        fprintf(stderr, "getDynamicArrayConstElement: array is NULL\n");
        return NULL;
    }

    if (array->data == NULL) {
        fprintf(stderr, "getDynamicArrayConstElement: array data is NULL\n");
        return NULL;
    }

    if (array->type == NULL) {
        fprintf(stderr, "getDynamicArrayConstElement: array type is NULL\n");
        return NULL;
    }

    if (index >= array->size) {
        fprintf(stderr, "getDynamicArrayConstElement: index out of range\n");
        return NULL;
    }

    return (const char*)array->data + index * array->type->size;
}

void setDynamicArrayElement(DynamicArray* array, size_t index, const void* value)
{
    void* element = getDynamicArrayElement(array, index);

    if (element == NULL) {
        fprintf(stderr, "setDynamicArrayElement: element is NULL\n");
        return;
    }

    if (value == NULL) {
        fprintf(stderr, "setDynamicArrayElement: value is NULL\n");
        return;
    }

    memcpy(element, value, array->type->size);
}
