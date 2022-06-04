#include "google_string_to_double.h"
#include "utils.h"

static double_conversion::StringToDoubleConverter* _converter = nullptr;

static inline double_conversion::StringToDoubleConverter* get_converter() {
  return _converter
             ? _converter
             : (_converter = new double_conversion::StringToDoubleConverter(
                    /* flags */ double_conversion::StringToDoubleConverter::
                        NO_FLAGS,
                    /* empty_string_value */ 0.0,
                    /* junk_string_value */ 0.0,
                    /* infinity_symbol */ "Infinity",
                    /* nan_symbol */ "NaN"));
}

napi_value google_string_to_double(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv[argc];
  napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

  if (argc != 1) {
    napi_throw_error(env, nullptr, "Invalid number of arguments.");
    return nullptr;
  }

  double_conversion::StringToDoubleConverter* converter = get_converter();

  int processed_characters_count = 0;
  std::pair<char*, size_t> pair = get_input_with_length(env, argv);

  double parsed = converter->StringToDouble(pair.first, pair.second,
                                            &processed_characters_count);

  napi_value result;
  napi_create_double(env, parsed, &result);
  return result;
}
