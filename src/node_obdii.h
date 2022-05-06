#ifndef NODE_OBDII_H
#define NODE_OBDII_H

#include <napi.h>
#include <stdlib.h>

#include <map>

extern "C" {
#include <OBDII.h>
#include <OBDIICommunication.h>
}

class NodeOBDII : public Napi::ObjectWrap<NodeOBDII> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);

  NodeOBDII(const Napi::CallbackInfo& info);
  ~NodeOBDII();

  Napi::Value IsOpened(const Napi::CallbackInfo& info);
  Napi::Value OpenSocket(const Napi::CallbackInfo& info);
  Napi::Value CloseSocket(const Napi::CallbackInfo& info);
  Napi::Value GetSupportedCommands(const Napi::CallbackInfo& info);
  Napi::Value PerformQuery(const Napi::CallbackInfo& info);

 private:
  std::map<int, OBDIISocket*> sockets;

  constexpr short GetNumberOfCommands() {
    return sizeof OBDIIMode1Commands / sizeof *OBDIIMode1Commands;
  }
};

#ifdef DEBUG
#define DEBUG_HEADER                                                   \
  fprintf(stderr, "node-obdii [%s:%s() %d]: ", __FILE__, __FUNCTION__, \
          __LINE__);
#define DEBUG_FOOTER fprintf(stderr, "\n");
#define DEBUG_LOG(...)                       \
  DEBUG_HEADER fprintf(stderr, __VA_ARGS__); \
  DEBUG_FOOTER;
#else
#define DEBUG_LOG(...)
#endif

#endif
