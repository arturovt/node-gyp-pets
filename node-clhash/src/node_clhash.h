#ifndef NODE_CLHASH_H
#define NODE_CLHASH_H

#include <clhash.h>
#include <js_native_api.h>
#include <node_api.h>

#include <random>
#include <utility>

static napi_value clhash_sync(napi_env, napi_callback_info);
static napi_value clhash(napi_env, napi_callback_info);
static napi_value init(napi_env, napi_value);

#endif
