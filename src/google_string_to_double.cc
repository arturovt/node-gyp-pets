#include "google_string_to_double.h"
#include "utils.h"

static double_conversion::StringToDoubleConverter* _string_to_double_converter =
    nullptr;

static inline double_conversion::StringToDoubleConverter*
get_string_to_double_converter() {
  if (!_string_to_double_converter) {
    _string_to_double_converter =
        new double_conversion::StringToDoubleConverter(
            /* flags */ 0,
            /* empty_string_value */ 0.0,
            /* junk_string_value */ 0.0,
            /* infinity_symbol */ "Infinity",
            /* nan_symbol */ "NaN");
  }
  return _string_to_double_converter;
}

napi_value google_string_to_double(napi_env env, napi_callback_info info) {
  double_conversion::StringToDoubleConverter* string_to_double_converter =
      get_string_to_double_converter();
  return nullptr;
}
