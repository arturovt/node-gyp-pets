{
  'targets': [
    {
      'target_name': 'node_fdp',
      'cflags!': ['-fno-exceptions'],
      'cflags_cc!': ['-fno-exceptions'],
      'include_dirs': ['libdc/double-conversion', 'libfdp/include'],
      'sources': ['src/node_fdp.cc', 'src/google_string_to_double.cc'],
      'cflags_cc': ['-std=c++14', '-Wall', '-Wextra'],
      'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
      'dependencies': ['libdc.gypi:libdc']
    }
  ]
}
