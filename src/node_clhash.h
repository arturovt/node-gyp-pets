#ifndef NODE_CLHASH_H
#define NODE_CLHASH_H

#include <clhash.h>
#include <napi.h>
#include <stdlib.h>

#include <random>
#include <string>

static Napi::Value Clhash(const Napi::CallbackInfo&);
static Napi::Object Init(Napi::Env, Napi::Object);

#endif
