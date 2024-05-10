#ifndef TEST_FRAMEWORK_H_
#define TEST_FRAMEWORK_H_

#define log_error(format, ...) own_log(__func__, __LINE__, "ERROR", format, ##__VA_ARGS__)
#define log_fail(format, ...) own_log(__func__, __LINE__, "FAIL", format, ##__VA_ARGS__)
#define log_info(format, ...) own_log(__func__, __LINE__, "INFO", format, ##__VA_ARGS__)
#define log_test(format, ...) own_log(__func__, __LINE__, "TEST", format, ##__VA_ARGS__)

void own_log(const char *func, unsigned int line, const char *lvl, const char *format, ...);

#define TEST_BEGIN() \
  log_test("--------------------------------------------------------"); \
  log_test("TC: %s", __func__);						\
  bool is_succeeded = true;						\
  mock_clear_calls();

#define TEST_ASSERT(condition, step)					\
  if (!(condition)) {							\
    log_fail("Step (%s) failed: %s", step, #condition);			\
    is_succeeded = false;						\
  }

#define TEST_END()				\
  bool is_mock_succeeded = mock_is_succeeded();	\
  if (is_succeeded && is_mock_succeeded) {	\
    log_info("Test succeeded");			\
  } else {					\
    log_fail("Test FAILED!!!!!");		\
  }

#endif // TEST_FRAMEWORK_H_
