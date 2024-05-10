#ifndef TEST_MOCK_H_
#define TEST_MOCK_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TYPE_NONE = 0,
  TYPE_BOOL,
  TYPE_CHAR,
  TYPE_UNSIGNED_CHAR,
  TYPE_CONST_TIME_ST,
  TYPE_CONST_TIME_ST_PTR,
} type_e;

typedef struct {
  bool is_recorded;
  type_e type;
  void *value;
} type_st;

typedef struct {
  bool is_recorded;
  char function_name[50];
  type_st params[2];
  type_st ret;

  bool is_called;
  char result[100];
  bool is_matched;
} mock_call_st;

#define NO_MOCK_CALLS 100

extern mock_call_st mock_calls[NO_MOCK_CALLS];

#define mock_prepare_param(dest, data)		\
  dest = (void*)malloc(sizeof(data));		\
  memcpy(dest, &data, sizeof(data));

void mock_clear_calls(void);
void mock_expect(const char *function_name, type_st *params, int no_params, type_st *ret);

#define mock_record(params, no_params, ret) __mock_record(__func__, params, no_params, ret)
void __mock_record(const char *function_name, type_st *params, int no_params, type_st *ret);
bool mock_is_succeeded(void);

#endif // TEST_MOCK_H_
