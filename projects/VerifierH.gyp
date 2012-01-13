{
	'conditions': [
        ['OS=="win"', {		
			'target_defaults': {
				'configurations': {
					'Debug_Win32': {
						'msvs_configuration_platform': 'Win32',
						'msvs_settings': {
							'VCCLCompilerTool': {
								'RuntimeLibrary': '1',
							},
							'VCLinkerTool': {
								'AdditionalDependencies':[
									'cryptlib.lib',
									'libxml2.lib'
								],
								'AdditionalLibraryDirectories': [
									'$(CRYPTOPP_SOURCE)\crypto++\$(Platform)\Output\$(Configuration)',
									'$(BOOST_SOURCE)\lib\$(PlatformShortName)',
									'$(LIBXML_SOURCE)\lib',
									'$(ICU_SOURCE)\lib',
								],
							},
						}
					},
					'Release_Win32': {
						'msvs_configuration_platform': 'Win32',
						'msvs_settings': {
							'VCCLCompilerTool': {
								'RuntimeLibrary': '2',
							},
							'VCLinkerTool': {
								'AdditionalDependencies':[
									'cryptlib.lib',
									'libxml2.lib'
								],
								'AdditionalLibraryDirectories': [
									'$(CRYPTOPP_SOURCE)\crypto++\$(Platform)\Output\$(Configuration)',
									'$(BOOST_SOURCE)\lib\$(PlatformShortName)',
									'$(LIBXML_SOURCE)\lib',
									'$(ICU_SOURCE)\lib',
								],
							},
						}
					},
					'Debug_x64': {
						'msvs_configuration_platform': 'x64',
						'msvs_settings': {
							'VCCLCompilerTool': {
								'RuntimeLibrary': '1',
							},
						}
					},
					'Release_x64': {
						'msvs_configuration_platform': 'x64',
						'msvs_settings': {
							'VCCLCompilerTool': {
								'RuntimeLibrary': '2',
							},
						}
					},
				},
			},
		}],	
	],
	'targets': [
    {
      'target_name': 'VerifierH',
      'type': 'executable',
      'sources': [
        './../src/HtmlScanner.cpp',		
		'./../src/HtmlScanner.h',		
		'./../src/Logger.h',	
		'./../src/VerifierH.cpp',
      ],
	'conditions': [
        ['OS=="win"', {
			'sources': [
				'./../src/targetver.h',
				'./../src/stdafx.cpp',
				'./../src/stdafx.h',
			],
			'include_dirs': [
				'$(BOOST_SOURCE)',		 
				'$(CRYPTOPP_SOURCE)',
				'$(LIBXML_SOURCE)\include',
				'$(ICU_SOURCE)\include',
				'$(VCInstallDir)include',
				'$(VCInstallDir)atlmfc\include',
				'$(WindowsSdkDir)include',
				'$(FrameworkSDKDir)\include',
			]      			
		}],
		['OS=="linux"', {
		    'configurations': {
				'Debug_Base': {
					'defines': [
						'DEBUG',
						'O0',
						'g3',
						'fPIC',
						'm32',
					],
				},
				'Release_Base': {
					'defines': [
						'O3',
						'fPIC',
						'm32',
					],
				},
			}, 
		    'include_dirs': [
				'/usr/include/libxml2',
				'/usr/include/c++/4.6',
				'/usr/include/c++/4.6/x86_64-linux-gnu',
				'/usr/include/c++/4.6/backward',
				'/usr/lib/gcc/x86_linux-gnu/4.6.1/include',
				'/usr/local/include',
				'/usr/lib/gcc/x86_64-linux-gnu/4.6.1/include-fixed',
				'/usr/include/x86_64-linux-gnu',
				'/usr/include',
			],

			'link_settings': {
				'libraries': [
					'/usr/lib/libboost_filesystem.a',
					'/usr/lib/libboost_regex.a',
					'/usr/lib/libboost_program_options.a',
					'/usr/lib/libcrypto++.a',
					'/usr/lib/libboost_system.a',
					'/usr/lib/libxml2.a',
					'/usr/lib/x86_64-linux-gnu/libz.a',
				],
			},
		}],
	],		
	},
  ]  
}
