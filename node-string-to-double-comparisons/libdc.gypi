{
  'targets': [
    {
      'target_name': 'libdc',
      'type': 'static_library',
      'include_dirs': ['libdc/double-conversion'],
      'sources': [
        'libdc/double-conversion/strtod.cc',
        'libdc/double-conversion/cached-powers.cc',
        'libdc/double-conversion/string-to-double.cc'
      ]
    }
  ]
}
