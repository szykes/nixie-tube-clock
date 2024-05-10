#include "mock.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "framework.h"

mock_call_st mock_calls[NO_MOCK_CALLS];

void mock_clear_calls(void) {
  for (int i = 0; i < sizeof(mock_calls)/sizeof(mock_call_st); i++) {
    for ( int j = 0; j < sizeof(mock_calls[i].params)/sizeof(type_st); j++) {
      if (mock_calls[i].params[j].value != NULL) {
	free(mock_calls[i].params[j].value);
      }
    }
    if (mock_calls[i].ret.value != NULL) {
      free(mock_calls[i].ret.value);
    }
  }

  memset(&mock_calls, 0, sizeof(mock_calls));
}

void mock_expect(const char *function_name, type_st *params, int no_params, type_st *ret) {
  int i;
  for (i = 0; i < sizeof(mock_calls)/sizeof(mock_call_st); i++) {
    if (mock_calls[i].is_recorded == false) {
      mock_calls[i].is_recorded = true;

      if (sizeof(mock_calls[i].function_name) < (strlen(function_name) + 1)) {
	log_error("Function name too long, name: %s, len: %d", function_name, strlen(function_name));
	break;
      }
      strcpy(mock_calls[i].function_name, function_name);

      if (sizeof(mock_calls[i].params)/sizeof(type_st) < no_params) {
	log_error("Not enough space for parameters, function: %s, no_params: %d", function_name, no_params);
	break;
      }
      for (int i = 0; i < no_params; i++) {
	mock_calls[i].params[i] = params[i];
      }

      if (ret != NULL) {
	mock_calls[i].ret = *ret;
      }

      break;
    }
  }

  if (i >= sizeof(mock_calls)/sizeof(mock_call_st)) {
    log_error("No space for expecting mock call");
  }
}

void __mock_record(const char *function_name, type_st *params, int no_params, type_st *ret) {
  int i;
  for (i = 0; i < sizeof(mock_calls)/sizeof(mock_call_st); i++) {
    if (mock_calls[i].is_called == false) {
      mock_calls[i].is_called = true;

      if (sizeof(mock_calls[i].function_name) < (strlen(function_name) + 1)) {
	log_error("Function name too long, name: %s, len: %d", function_name, strlen(function_name));
	break;
      }

      if (strcmp(mock_calls[i].function_name, function_name) != 0) {
	int result = snprintf(mock_calls[i].result, sizeof(mock_calls[i].result), "wrong function is expected here, expected: %s, got: %s", mock_calls[i].function_name, function_name);
	if (result >= sizeof(mock_calls[i].result)) {
	  log_error("Result buffer too small");
	}
	break;
      }

      if (sizeof(mock_calls[i].params)/sizeof(type_st) < no_params) {
	log_error("Not enough space for parameters, function: %s, no_params: %d", function_name, no_params);
	break;
      }
      for (int j = 0; j < no_params; j++) {
	if (mock_calls[i].params[j].is_recorded == false) {
	  int result = snprintf(mock_calls[i].result, sizeof(mock_calls[i].result), "wrong number of parameters, expected: %d, got: %d", no_params, j);
	  if (result >= sizeof(mock_calls[i].result)) {
	    log_error("Result buffer too small");
	  }
	  break;
	}

	if (mock_calls[i].params[j].type != params[j].type) {
	  int result = snprintf(mock_calls[i].result, sizeof(mock_calls[i].result), "wrong parameter type, expected: %d, got: %d", mock_calls[i].params[j].type, params[j].type);
	  if (result >= sizeof(mock_calls[i].result)) {
	    log_error("Result buffer too small");
	  }
	  break;
	}

	if (memcmp(mock_calls[i].params[j].value, params[j].value, sizeof(mock_calls[i].params[j].value)) != 0) {
	  int result = snprintf(mock_calls[i].result, sizeof(mock_calls[i].result), "wrong parameter value");
	  if (result >= sizeof(mock_calls[i].result)) {
	    log_error("Result buffer too small");
	  }
	  break;
	}
      }

      if (ret != NULL) {
	if (mock_calls[i].ret.is_recorded == false) {
	  int result = snprintf(mock_calls[i].result, sizeof(mock_calls[i].result), "wrong return value, expected: %d, got: NULL", ret->type);
	  if (result >= sizeof(mock_calls[i].result)) {
	    log_error("Result buffer too small");
	  }
	  break;
	}

	memcpy(ret, &mock_calls[i].ret, sizeof(mock_calls[i].ret));
      }

      mock_calls[i].is_matched = true;
      break;
    }
  }

  if (i >= sizeof(mock_calls)/sizeof(mock_call_st)) {
    log_error("No space for recording mock call");
  }
}

bool mock_is_succeeded(void) {
  for (int i = 0; i < sizeof(mock_calls)/sizeof(mock_call_st); i++) {
    if (mock_calls[i].is_recorded == false \
     && mock_calls[i].is_called == true) {
      log_fail("Mock call(s) not expected, %s", mock_calls[i].result);
      return false;
    }
    if (mock_calls[i].is_recorded == true \
     && mock_calls[i].is_called == false) {
      log_fail("Mock call(s) missing call, function is not expected here, function: '%s()'", mock_calls[i].function_name);
      return false;
    }
    if (mock_calls[i].is_recorded == true \
     && mock_calls[i].is_called == true \
     && mock_calls[i].is_matched == false) {
      log_fail("Mock call(s) not matched, %s", mock_calls[i].result);
      return false;
    }
  }

  return true;
}
