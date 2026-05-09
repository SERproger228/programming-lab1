#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "type_info.h"

#include <stddef.h>

typedef struct DynamicArray {
    void* data;
    size_t size;
    const TypeInfo* type;
} DynamicArray;

DynamicArray createDynamicArray(size_t size, const TypeInfo* type);
void freeDynamicArray(DynamicArray* array);

void* getDynamicArrayElement(DynamicArray* array, size_t index);
const void* getDynamicArrayConstElement(const DynamicArray* array, size_t index);

void setDynamicArrayElement(DynamicArray* array, size_t index, const void* value);

#endif
