#include "node_clhash.h"

static std::random_device rd;
static std::mt19937_64 gen(rd());
static std::uniform_int_distribution<uint64_t> distribute;

napi_value clhash(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv[1];
  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc != 1) {
    napi_throw_type_error(env, NULL, "Invalid number of arguments.");
    return NULL;
  }

  napi_valuetype argument_type;
  napi_typeof(env, argv[0], &argument_type);
  if (argument_type != napi_string) {
    napi_throw_type_error(env, NULL,
                          "The input data to be encoded should be a string.");
    return NULL;
  }

  void* key = get_random_key_for_clhash(
      /* seed1 */ distribute(gen),
      /* seed2 */ distribute(gen));

  if (key == NULL) {
    napi_throw_type_error(env, NULL, "The key was failed to be generated.");
    return NULL;
  }

  size_t input_size;
  napi_get_value_string_utf8(env, argv[0], NULL, 0, &input_size);
  // We need to increase the size of the input since the last symbol is NULL.
  input_size += 1;
  char* input;
  input = (char*)calloc(input_size, sizeof(char));
  napi_get_value_string_utf8(env, argv[0], input, input_size, NULL);

  napi_value key_bigint;
  napi_create_bigint_uint64(env, *(uint64_t*)key, &key_bigint);

  uint64_t hashed = clhash(key, input, input_size);
  napi_value hashed_bigint;
  napi_create_bigint_uint64(env, hashed, &hashed_bigint);

  free(input);
  free(key);

  napi_value result;
  napi_create_object(env, &result);
  napi_set_named_property(env, result, "key", key_bigint);
  napi_set_named_property(env, result, "hashed", hashed_bigint);
  return result;
}

napi_value init(napi_env env, napi_value exports) {
  napi_value clhash_fn;
  napi_create_function(env, NULL, 0, clhash, NULL, &clhash_fn);
  napi_set_named_property(env, exports, "clhash", clhash_fn);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init)
