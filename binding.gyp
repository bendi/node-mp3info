{
  'targets': [
    {
      'target_name': 'bindings',
      'sources': [
        'src/bindings.cc',
        'src/node_mp3info.cc'
      ],
      'dependencies': [
        'deps/mp3info/mp3info.gyp:mp3info'
      ],
    }
  ]
}
