#include <fast_double_parser.h>
#include <js_native_api.h>
#include <node_api.h>

static inline char* get_input(napi_env env, napi_value* argv) {
  size_t input_size;
  napi_get_value_string_utf8(env, argv[0], nullptr, 0, &input_size);
  // We need to increase the size of the input since the last symbol is NULL.
  input_size += 1;
  char* input = new char[input_size]();
  napi_get_value_string_utf8(env, argv[0], input, input_size, nullptr);
  return input;
}

static inline napi_value parse(napi_env env,
                               napi_callback_info info,
                               const char* (*parser)(const char*, double*)) {
  size_t argc = 1;
  napi_value argv[argc];
  napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

  if (argc != 1) {
    napi_throw_error(env, nullptr, "Invalid number of arguments.");
    return nullptr;
  }

  napi_valuetype argument_type;
  napi_typeof(env, argv[0], &argument_type);
  if (argument_type != napi_string) {
    napi_throw_type_error(env, nullptr,
                          "The input data to be encoded should be a string.");
    return nullptr;
  }

  double parsed = 0.0;
  const char* input = get_input(env, argv);
  // If it is `nullptr`, then the function refused to parse the input.
  const char* result_pointer = parser(input, &parsed);
  delete input;

  if (!result_pointer) {
    napi_throw_error(
        env, nullptr,
        "The input has been refused to be parsed. The input should be string "
        "numbers that follow RFC 7159 (JSON standard).");
    return nullptr;
  }

  napi_value result;
  napi_create_double(env, parsed, &result);
  return result;
}

napi_value strtod(napi_env env, napi_callback_info info) {
  return parse(env, info, fast_double_parser::parse_float_strtod);
}

napi_value parse_number(napi_env env, napi_callback_info info) {
  return parse(env, info, fast_double_parser::parse_number);
}

napi_value init(napi_env env, napi_value exports) {
  napi_value strtod_fn;
  napi_create_function(env, nullptr, NAPI_AUTO_LENGTH, strtod, nullptr,
                       &strtod_fn);
  napi_set_named_property(env, exports, "strtod", strtod_fn);

  napi_value parse_number_fn;
  napi_create_function(env, nullptr, NAPI_AUTO_LENGTH, parse_number, nullptr,
                       &parse_number_fn);
  napi_set_named_property(env, exports, "parseNumber", parse_number_fn);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init)
