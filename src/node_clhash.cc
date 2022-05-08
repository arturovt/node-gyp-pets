#include "node_clhash.h"

static std::random_device rd;
static std::mt19937_64 gen(rd());
static std::uniform_int_distribution<uint64_t> distribute;

static Napi::Value Clhash(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error::New(env, "Invalid number of arguments.")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsString()) {
    Napi::TypeError::New(env,
                         "The input data to be encoded should be a string.")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  void* key = get_random_key_for_clhash(
      /* seed1 */ distribute(gen),
      /* seed2 */ distribute(gen));

  if (key != NULL) {
    uint64_t* key64 = (uint64_t*)key;
    Napi::External<uint64_t> external = Napi::External<uint64_t>::New(
        env, key64, [](Napi::Env env, uint64_t* key64) { free(key64); });

    std::string input = info[0].As<Napi::String>().Utf8Value();
    uint64_t hashed = clhash((void*)&key, input.c_str(), input.length());

    Napi::Object result = Napi::Object::New(env);
    result["key"] = Napi::BigInt::New(env, *external.Data());
    result["hashed"] = Napi::BigInt::New(env, hashed);
    return result;
  } else {
    Napi::TypeError::New(env, "The key was failed to be generated.")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "clhash"),
              Napi::Function::New(env, Clhash));
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
