# Copyright (c) 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
    'grit_out_dir': '<(SHARED_INTERMEDIATE_DIR)/chrome',
  },
  'targets': [
    {
      'target_name': 'wm',
      'type': 'static_library',
      'dependencies': [
        '../ash/ash.gyp:ash',
        '../ash/ash.gyp:ash_resources',
        '../ash/ash_strings.gyp:ash_strings',
        '../base/base.gyp:base',
        '../base/base.gyp:base_i18n',
        '../chrome/chrome_resources.gyp:packed_resources',
        '../content/content.gyp:content',
        '../content/content.gyp:content_shell_lib',
        '../skia/skia.gyp:skia',
        '../third_party/icu/icu.gyp:icui18n',
        '../third_party/icu/icu.gyp:icuuc',
        '../ui/app_list/app_list.gyp:app_list',
        '../ui/aura/aura.gyp:aura',
        '../ui/compositor/compositor.gyp:compositor',
        '../ui/compositor/compositor.gyp:compositor_test_support',
        '../ui/message_center/message_center.gyp:message_center',
        '../ui/ui.gyp:ui',
        '../ui/ui.gyp:ui_resources',
        '../ui/views/views.gyp:views',
        '../ui/views/views.gyp:views_examples_with_content_lib',
        '../ui/views/views.gyp:views_test_support',
      ],
      'sources': [
        'foreign_window.cc',
        'foreign_window.h',
        'foreign_window_client_view.cc',
        'foreign_window_client_view.h',
        'foreign_window_manager.cc',
        'foreign_window_manager.h',
        'foreign_window_widget.cc',
        'foreign_window_widget.h',
        'gpu/foreign_window_texture_factory.cc',
        'gpu/foreign_window_texture_factory.h',
        'host/foreign_window_host.cc',
        'host/foreign_window_host.h',
        'host/foreign_window_host_delegate.h',
        'host/foreign_window_host_linux.cc',
        'host/foreign_window_host_x11.cc',
        'host/foreign_window_host_x11.h',
        'host/foreign_window_manager_host.cc',
        'host/foreign_window_manager_host.h',
        'host/foreign_window_manager_host_linux.cc',
        'host/foreign_window_manager_host_x11.cc',
        'host/foreign_window_manager_host_x11.h',
      ],
      'conditions': [
        ['OS=="linux"', {
          'sources/': [
            ['exclude', 'host/foreign_window_host.cc'],
            ['exclude', 'host/foreign_window_manager_host.cc'],
          ],
          'link_settings': {
            'libraries': [
              '-lX11',
              '-lXdamage',
              '-lXfixes',
              '-lXi',
              '-lXrandr',
            ],
          },
        }],
      ],
    },
    {
      'target_name': 'wm_test_support',
      'type': 'static_library',
      'dependencies': [
        '../ash/ash.gyp:ash',
        '../ash/ash.gyp:ash_resources',
        '../skia/skia.gyp:skia',
        '../testing/gtest.gyp:gtest',
        'wm',
      ],
      'sources': [
        '../ash/test/cursor_manager_test_api.cc',
        '../ash/test/cursor_manager_test_api.h',
        '../ash/test/display_manager_test_api.cc',
        '../ash/test/display_manager_test_api.h',
        '../ash/test/launcher_view_test_api.cc',
        '../ash/test/launcher_view_test_api.h',
        '../ash/test/test_activation_delegate.cc',
        '../ash/test/test_activation_delegate.h',
        '../ash/test/test_launcher_delegate.cc',
        '../ash/test/test_launcher_delegate.h',
        '../ash/test/test_session_state_delegate.cc',
        '../ash/test/test_session_state_delegate.cc',
        '../ash/test/test_suite.cc',
        '../ash/test/test_suite.h',
        '../ash/test/test_suite_init.h',
        '../ash/test/test_suite_init.mm',
        'foreign_test_window.cc',
        'foreign_test_window.h',
        'host/foreign_test_window_host.cc',
        'host/foreign_test_window_host.h',
        'host/foreign_test_window_host_linux.cc',
        'host/foreign_test_window_host_x11.cc',
        'host/foreign_test_window_host_x11.h',
        'test/test_shell_delegate.cc',
        'test/test_shell_delegate.h',
        'test/wm_test_base.cc',
        'test/wm_test_base.h',
        'test/wm_unittests.cc',
      ],
      'conditions': [
        ['OS=="linux"', {
          'sources/': [
            ['exclude', 'host/foreign_test_window_host.cc'],
          ],
        }],
      ],
    },
    {
      'target_name': 'wm_unittests',
      'type': 'executable',
      'dependencies': [
        '../ash/ash.gyp:ash',
        '../ash/ash.gyp:ash_resources',
        '../ash/ash_strings.gyp:ash_strings',
        '../base/base.gyp:base',
        '../base/base.gyp:test_support_base',
        '../build/temp_gyp/googleurl.gyp:googleurl',
        '../chrome/chrome_resources.gyp:packed_resources',
        '../content/content.gyp:content_browser',
        '../content/content.gyp:test_support_content',
        '../skia/skia.gyp:skia',
        '../testing/gtest.gyp:gtest',
        '../third_party/icu/icu.gyp:icui18n',
        '../third_party/icu/icu.gyp:icuuc',
        '../ui/app_list/app_list.gyp:app_list',
        '../ui/aura/aura.gyp:aura',
        '../ui/aura/aura.gyp:aura_test_support',
        '../ui/compositor/compositor.gyp:compositor',
        '../ui/compositor/compositor.gyp:compositor_test_support',
        '../ui/message_center/message_center.gyp:message_center',
        '../ui/ui.gyp:ui',
        '../ui/ui.gyp:ui_resources',
        '../ui/ui.gyp:ui_test_support',
        '../ui/views/views.gyp:views',
        '../ui/views/views.gyp:views_examples_with_content_lib',
        '../ui/views/views.gyp:views_with_content_test_support',
        'wm_test_support',
      ],
      'sources': [
        '../ui/views/test/test_views_delegate.cc',
        '../ui/views/test/test_views_delegate.h',
        '<(SHARED_INTERMEDIATE_DIR)/ash/ash_resources/ash_wallpaper_resources.rc',
        'foreign_window_unittest.cc',
      ],
    },
    {
      'target_name': 'wm_shell',
      'type': 'executable',
      'dependencies': [
        '../ash/ash.gyp:ash',
        '../ash/ash.gyp:ash_resources',
        '../ash/ash_strings.gyp:ash_strings',
        '../base/base.gyp:base',
        '../base/base.gyp:base_i18n',
        '../chrome/chrome_resources.gyp:packed_resources',
        '../content/content.gyp:content',
        '../content/content.gyp:content_shell_lib',
        '../skia/skia.gyp:skia',
        '../third_party/icu/icu.gyp:icui18n',
        '../third_party/icu/icu.gyp:icuuc',
        '../ui/app_list/app_list.gyp:app_list',
        '../ui/aura/aura.gyp:aura',
        '../ui/compositor/compositor.gyp:compositor',
        '../ui/compositor/compositor.gyp:compositor_test_support',
        '../ui/message_center/message_center.gyp:message_center',
        '../ui/ui.gyp:ui',
        '../ui/ui.gyp:ui_resources',
        '../ui/views/views.gyp:views',
        '../ui/views/views.gyp:views_examples_with_content_lib',
        '../ui/views/views.gyp:views_test_support',
        'wm',
      ],
      'sources': [
        '../ash/session_state_delegate_stub.cc',
        '../ash/session_state_delegate_stub.h',
        '../ash/shell/app_list.cc',
        '../ash/shell/bubble.cc',
        '../ash/shell/context_menu.cc',
        '../ash/shell/context_menu.h',
        '../ash/shell/example_factory.h',
        '../ash/shell/launcher_delegate_impl.cc',
        '../ash/shell/launcher_delegate_impl.h',
        '../ash/shell/lock_view.cc',
        '../ash/shell/panel_window.cc',
        '../ash/shell/panel_window.h',
        '../ash/shell/toplevel_window.cc',
        '../ash/shell/toplevel_window.h',
        '../ash/shell/widgets.cc',
        '../ash/shell/window_type_launcher.cc',
        '../ash/shell/window_type_launcher.h',
        '../ash/shell/window_watcher.cc',
        '../ash/shell/window_watcher.h',
        '../content/app/startup_helper_win.cc',
        '../ui/views/test/test_views_delegate.cc',
        '<(SHARED_INTERMEDIATE_DIR)/ash/ash_resources/ash_wallpaper_resources.rc',
        'foreign_test_window.cc',
        'foreign_test_window.h',
        'host/foreign_test_window_host.cc',
        'host/foreign_test_window_host.h',
        'host/foreign_test_window_host_linux.cc',
        'host/foreign_test_window_host_x11.cc',
        'host/foreign_test_window_host_x11.h',
        'shell/content_client/shell_browser_main_parts.cc',
        'shell/content_client/shell_browser_main_parts.h',
        'shell/content_client/shell_content_browser_client.cc',
        'shell/content_client/shell_content_browser_client.h',
        'shell/content_client/shell_main_delegate.cc',
        'shell/content_client/shell_main_delegate.h',
        'shell/shell_delegate_impl.cc',
        'shell/shell_delegate_impl.h',
        'shell/shell_main.cc',
        'shell/shell_main_parts.cc',
        'shell/shell_main_parts.h',
      ],
      'conditions': [
        ['OS=="linux"', {
          'sources/': [
            ['exclude', 'host/foreign_test_window_host.cc'],
          ],
        }],
      ],
    },
  ],
}
