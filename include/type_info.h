#ifndef TYPE_INFO_H
  #define TYPE_INFO_H

  #include <stddef.h>

  typedef struct TypeInfo {
      size_t size;

      void (*add)(void* result, const void* a, const void* b);
      void (*multiply)(void* result, const void* a, const void* b);
      void (*print)(const void* value);
      int (*isZero)(const void* value);
  } TypeInfo;

  const TypeInfo* getIntTypeInfo(void);
  const TypeInfo* getDoubleTypeInfo(void);

  #endif
