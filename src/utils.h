#include <utility>

#include <js_native_api.h>

inline std::pair<char*, size_t> get_input(napi_env env, napi_value* argv) {
  size_t length = 0;
  napi_get_value_string_utf8(env, argv[0], nullptr, 0, &length);
  // We need to increase the length of the input since the last symbol is NULL.
  length += 1;
  char* input = new char[length]();
  napi_get_value_string_utf8(env, argv[0], input, length, nullptr);
  return std::make_pair(input, length);
}
