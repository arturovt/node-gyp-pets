#include "atof.h"
#include "utils.h"

napi_value stdlib_atof(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv[argc];
  napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

  if (argc != 1) {
    napi_throw_error(env, nullptr, "Invalid number of arguments.");
    return nullptr;
  }

  const char* input = get_input(env, argv);
  double parsed = atof(input);
  delete input;

  napi_value result;
  napi_create_double(env, parsed, &result);
  return result;  
}
