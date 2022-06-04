#include <js_native_api.h>

#include <utility>

inline char* get_input(napi_env env, napi_value* argv) {
  size_t length = 0;
  napi_get_value_string_utf8(env, argv[0], nullptr, 0, &length);
  // We need to increase the length of the input since the last symbol is NULL.
  length += 1;
  char* input = new char[length]();
  napi_get_value_string_utf8(env, argv[0], input, length, nullptr);
  return input;
}

inline std::pair<char*, size_t> get_input_with_length(napi_env env,
                                                      napi_value* argv) {
  size_t length = 0;
  napi_get_value_string_utf8(env, argv[0], nullptr, 0, &length);
  // We need to increase the length of the input since the last symbol is NULL.
  char* input = new char[length + 1]();
  napi_get_value_string_utf8(env, argv[0], input, length + 1, nullptr);
  return std::make_pair(input, /* Preserve the original length. */ length);
}
