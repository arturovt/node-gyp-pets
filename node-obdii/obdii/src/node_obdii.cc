#include "node_obdii.h"

#define CHECK_SOCKET()                                           \
  if (it != sockets.end()) {                                     \
    DEBUG_LOG("There is no socket with such FD: %d", socket_fd); \
    return env.Null();                                           \
  }

NodeOBDII::NodeOBDII(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<NodeOBDII>(info) {}

NodeOBDII::~NodeOBDII() {
  for (auto const& it : sockets) {
    OBDIICloseSocket(it.second);
    free(it.second);
  }
}

Napi::Value NodeOBDII::IsOpened(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::TypeError::New(env,
                         "Wrong number of arguments. One argument expected.")
        .ThrowAsJavaScriptException();
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(env, "The socket FD should be a number.")
        .ThrowAsJavaScriptException();
  }

  int socket_fd = info[0].As<Napi::Number>().Uint32Value();
  auto it = sockets.find(socket_fd);
  return Napi::Boolean::New(env, it != sockets.end());
}

Napi::Value NodeOBDII::OpenSocket(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  // This actually should be `vcan0` since it's been tested with a virtual CAN
  // network.
  const char* ifr_name = "can0";
  // The CAN ID 0x7E0 is usually the address of the engine controller (ECU).
  canid_t physical_request_id = 0x7E0;
  // This should response with a packet with the identifier 0x7E8.
  canid_t physical_response_id = 0x7E8;
  // Determines whether the connection should be opened by daemon or not.
  short shared = 0;

  OBDIISocket* socket = (OBDIISocket*)malloc(sizeof(OBDIISocket));
  if (OBDIIOpenSocket(socket, ifr_name, physical_request_id,
                      physical_response_id, shared) < 0) {
    free(socket);
    DEBUG_LOG("Was not able to open socket connection.");
    return env.Null();
  }

  int socket_fd = socket->s;
  sockets[socket_fd] = socket;
  return Napi::Number::New(env, socket_fd);
}

Napi::Value NodeOBDII::CloseSocket(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::TypeError::New(env,
                         "Wrong number of arguments. One argument expected.")
        .ThrowAsJavaScriptException();
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(env, "The socket FD should be a number.")
        .ThrowAsJavaScriptException();
  }

  bool closed = false;
  int socket_fd = info[0].As<Napi::Number>().Uint32Value();
  auto it = sockets.find(socket_fd);
  if (it != sockets.end()) {
    OBDIISocket* socket = it->second;
    closed = OBDIICloseSocket(socket) == 0;
    if (!closed) {
      DEBUG_LOG("Was not able to close socket connection for %d", socket_fd);
    }
    free(socket);
    sockets.erase(it);
  } else {
    DEBUG_LOG("There is no socket with such FD: %d", socket_fd);
  }
  return Napi::Boolean::New(env, closed);
}

Napi::Value NodeOBDII::GetSupportedCommands(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::TypeError::New(env,
                         "Wrong number of arguments. One argument expected.")
        .ThrowAsJavaScriptException();
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(env, "The socket FD should be a number.")
        .ThrowAsJavaScriptException();
  }

  int socket_fd = info[0].As<Napi::Number>().Uint32Value();
  auto it = sockets.find(socket_fd);

  CHECK_SOCKET();

  OBDIISocket* socket = it->second;
  OBDIICommandSet command_set = OBDIIGetSupportedCommands(socket);
  DEBUG_LOG("The number of supported commands: %d", command_set.numCommands);
  Napi::Array js_commands = Napi::Array::New(env, command_set.numCommands);

  for (int index = 0; index < command_set.numCommands; index++) {
    OBDIICommand* command = command_set.commands[index];
    Napi::Object js_command = Napi::Object::New(env);
    js_command["pid"] = Napi::Number::New(env, OBDIICommandGetPID(command));
    js_command["mode"] = Napi::Number::New(env, OBDIICommandGetMode(command));
    js_command["name"] = Napi::String::New(env, command->name);
    js_commands[index] = js_command;
  }

  return js_commands;
}

Napi::Value NodeOBDII::PerformQuery(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 2) {
    Napi::TypeError::New(env,
                         "Wrong number of arguments. Two arguments expected.")
        .ThrowAsJavaScriptException();
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(env, "The socket FD should be a number.")
        .ThrowAsJavaScriptException();
  }

  if (!info[1].IsNumber()) {
    Napi::TypeError::New(env, "The command PID should be a number.")
        .ThrowAsJavaScriptException();
  }

  int socket_fd = info[0].As<Napi::Number>().Uint32Value();
  auto it = sockets.find(socket_fd);

  CHECK_SOCKET();

  int command_pid = info[1].As<Napi::Number>().Uint32Value();

  if (command_pid < 0 || command_pid > GetNumberOfCommands()) {
    Napi::TypeError::New(env, "The command with such PID does not exist.")
        .ThrowAsJavaScriptException();
  }

  OBDIISocket* socket = it->second;
  OBDIICommand command = OBDIIMode1Commands[command_pid];
  OBDIIResponse response = OBDIIPerformQuery(socket, &command);

  Napi::Object js_response = Napi::Object::New(env);
  js_response["success"] = Napi::Number::New(env, response.success);

  switch (response.command->responseType) {
    case OBDIIResponseTypeBitfield:
      js_response["bitfieldValue"] =
          Napi::Number::New(env, response.bitfieldValue);
      break;

    case OBDIIResponseTypeNumeric:
      js_response["numericValue"] =
          Napi::Number::New(env, response.numericValue);
      break;

    case OBDIIResponseTypeString:
      js_response["stringValue"] = Napi::String::New(env, response.stringValue);
      break;

    case OBDIIResponseTypeOther:
      if (response.command == OBDIICommands.DTCs) {
        Napi::Array dtcs = Napi::Array::New(env, response.DTCs.numTroubleCodes);
        for (int index = 0; index < response.DTCs.numTroubleCodes; index++) {
          auto troubles_codes = response.DTCs.troubleCodes[index];
        }
        js_response["DTCs"] = dtcs;
      }

      break;
  }

  OBDIIResponseFree(&response);

  return js_response;
}

void NodeOBDII::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(
      env, "NodeOBDII",
      {
          InstanceMethod<&NodeOBDII::IsOpened>(
              "isOpened", static_cast<napi_property_attributes>(
                              napi_writable | napi_configurable)),
          InstanceMethod<&NodeOBDII::OpenSocket>(
              "openSocket", static_cast<napi_property_attributes>(
                                napi_writable | napi_configurable)),
          InstanceMethod<&NodeOBDII::CloseSocket>(
              "closeSocket", static_cast<napi_property_attributes>(
                                 napi_writable | napi_configurable)),
          InstanceMethod<&NodeOBDII::GetSupportedCommands>(
              "getSupportedCommands", static_cast<napi_property_attributes>(
                                          napi_writable | napi_configurable)),
          InstanceMethod<&NodeOBDII::PerformQuery>(
              "performQuery", static_cast<napi_property_attributes>(
                                  napi_writable | napi_configurable)),
      });

  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);

  exports.Set("NodeOBDII", func);
  env.SetInstanceData<Napi::FunctionReference>(constructor);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  NodeOBDII::Init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
