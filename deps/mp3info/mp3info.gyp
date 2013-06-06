# This file is used with the GYP meta build system.
# http://code.google.com/p/gyp

{
  'variables': {
    'target_arch%': 'ia32',
  },
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 1, # static debug
          },
        },
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 0, # static release
          },
        },
      }
    },
    'msvs_settings': {
      'VCLinkerTool': {
        'GenerateDebugInformation': 'true',
      },
    },
    'conditions': [
      ['OS=="mac"', {
        'conditions': [
          ['target_arch=="ia32"', { 'xcode_settings': { 'ARCHS': [ 'i386' ] } }],
          ['target_arch=="x64"', { 'xcode_settings': { 'ARCHS': [ 'x86_64' ] } }]
        ],
      }],
    ]
  },

  'targets': [
    {
      'target_name': 'mp3info',
      'product_prefix': 'lib',
      'type': 'static_library',
      'variables': {
        'conditions': [
          # "mp3info_cpu" is the cpu optimization to use
          # Windows uses "i386_fpu" even on x64 to avoid compiling .S asm files
          # (I don't think the 64-bit ASM files are compatible with `ml`/`ml64`...)
          ['OS=="win"', { 'mp3info_cpu%': 'i386_fpu' },
          { 'conditions': [
            ['target_arch=="arm"', { 'mp3info_cpu%': 'arm_nofpu' }],
            ['target_arch=="ia32"', { 'mp3info_cpu%': 'i386_fpu' }],
            ['target_arch=="x64"', { 'mp3info_cpu%': 'x86-64' }],
          ]}],
        ]
      },
      'sources': [
        'src/mp3tech.c'
      ],
      'include_dirs': [
        'src',
        # platform and arch-specific headers
 #       'config/<(OS)/<(target_arch)',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          'src',
          # platform and arch-specific headers
#          'config/<(OS)/<(target_arch)',
        ]
      },
	  'conditions': [
        ['OS=="win"', {
          'link_settings': {
            'libraries': [
              '-lWs2_32.lib',
            ]
		  }
        }]
	  ]
    }
  ]
}
