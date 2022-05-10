#include "node_clhash.h"

// This code is a mix of C and C++ since it links with a C library, which still
// requires calling C functions (e.g. when generating a key, the caller is
// responsible for calling `free` on the pointer).

static std::random_device rd;
static std::mt19937_64 gen(rd());
static std::uniform_int_distribution<uint64_t> distribute;

static inline std::pair<char*, size_t> get_input(napi_env env,
                                                 napi_value* argv) {
  size_t input_size;
  napi_get_value_string_utf8(env, argv[0], NULL, 0, &input_size);
  // We need to increase the size of the input since the last symbol is NULL.
  input_size += 1;
  char* input = new char[input_size]();
  napi_get_value_string_utf8(env, argv[0], input, input_size, NULL);
  return std::make_pair(input, input_size);
}

static napi_value clhash_sync(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv[1];
  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc != 1) {
    napi_throw_error(env, NULL, "Invalid number of arguments.");
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

  std::pair<char*, size_t> pair = get_input(env, argv);
  char* input = std::get<char*>(pair);
  size_t input_size = std::get<size_t>(pair);

  napi_value key_bigint;
  napi_create_bigint_uint64(env, *(uint64_t*)key, &key_bigint);

  uint64_t hashed = clhash(key, input, input_size);
  napi_value hashed_bigint;
  napi_create_bigint_uint64(env, hashed, &hashed_bigint);

  // Note: this should be `free` since the key space was allocated through the
  // `posix_memalign`.
  free(key);
  delete input;

  napi_value result;
  napi_create_object(env, &result);
  napi_set_named_property(env, result, "key", key_bigint);
  napi_set_named_property(env, result, "hashed", hashed_bigint);
  return result;
}

typedef struct {
  napi_ref callback;
  napi_async_work async_work;
  char* input;
  size_t input_size;
  void* key;
  uint64_t hashed;
} clhash_async_context;

static napi_value clhash(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc != 2) {
    napi_throw_type_error(env, NULL, "Invalid number of arguments.");
    return NULL;
  }

  napi_valuetype first_argument_type, second_argument_type;
  napi_typeof(env, argv[0], &first_argument_type);
  napi_typeof(env, argv[1], &second_argument_type);
  if (first_argument_type != napi_string) {
    napi_throw_type_error(env, NULL,
                          "The input data to be encoded should be a string.");
    return NULL;
  }

  if (second_argument_type != napi_function) {
    napi_throw_type_error(env, NULL,
                          "The second argument should be a callback.");
    return NULL;
  }

  std::pair<char*, size_t> pair = get_input(env, argv);
  char* input = std::get<char*>(pair);
  size_t input_size = std::get<size_t>(pair);

  napi_value async_resource_name;
  napi_create_string_utf8(env, "clhash", NAPI_AUTO_LENGTH,
                          &async_resource_name);

  clhash_async_context* async_context = new clhash_async_context;
  async_context->input = input;
  async_context->input_size = input_size;
  napi_create_reference(env, argv[1], 1, &async_context->callback);

  napi_create_async_work(
      env, NULL, async_resource_name,
      [](napi_env env, void* data) {
        clhash_async_context* async_context = (clhash_async_context*)data;

        void* key = get_random_key_for_clhash(
            /* seed1 */ distribute(gen),
            /* seed2 */ distribute(gen));

        if (key != NULL) {
          async_context->hashed =
              clhash(key, async_context->input, async_context->input_size);
        }

        async_context->key = key;
      },
      [](napi_env env, napi_status status, void* data) {
        clhash_async_context* async_context = (clhash_async_context*)data;

        napi_value global;
        napi_get_global(env, &global);

        napi_value callback;
        napi_get_reference_value(env, async_context->callback, &callback);

        napi_value callback_arguments[2] = {0};
        // The `hashed` value has not been set when the async work was
        // executing, so we have to report the error.
        if (async_context->hashed == 0) {
          napi_value message;
          napi_create_string_utf8(env, "The key was failed to be generated.",
                                  NAPI_AUTO_LENGTH, &message);
          napi_create_error(env, NULL, message, &callback_arguments[0]);
          napi_get_undefined(env, &callback_arguments[1]);
        } else {
          napi_value key_bigint;
          napi_create_bigint_uint64(env, *(uint64_t*)async_context->key,
                                    &key_bigint);

          napi_value hashed_bigint;
          napi_create_bigint_uint64(env, async_context->hashed, &hashed_bigint);

          napi_get_null(env, &callback_arguments[0]);
          napi_create_object(env, &callback_arguments[1]);
          napi_set_named_property(env, callback_arguments[1], "key",
                                  key_bigint);
          napi_set_named_property(env, callback_arguments[1], "hashed",
                                  hashed_bigint);
        }

        napi_value result;
        napi_call_function(env, global, callback, 2, callback_arguments,
                           &result);

        napi_delete_reference(env, async_context->callback);
        napi_delete_async_work(env, async_context->async_work);

        // Note: this should be `free` since the key space was allocated through
        // the `posix_memalign`.
        free(async_context->key);
        delete async_context->input;
        delete async_context;
      },
      (void*)async_context, &async_context->async_work);

  napi_queue_async_work(env, async_context->async_work);

  napi_value undefined;
  napi_get_undefined(env, &undefined);
  return undefined;
}

napi_value init(napi_env env, napi_value exports) {
  napi_value clhash_fn;
  napi_create_function(env, NULL, NAPI_AUTO_LENGTH, clhash, NULL, &clhash_fn);
  napi_set_named_property(env, exports, "clhash", clhash_fn);

  napi_value clhash_sync_fn;
  napi_create_function(env, NULL, NAPI_AUTO_LENGTH, clhash_sync, NULL,
                       &clhash_sync_fn);
  napi_set_named_property(env, clhash_fn, "sync", clhash_sync_fn);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init)
