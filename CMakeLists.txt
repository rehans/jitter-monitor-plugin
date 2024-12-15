cmake_minimum_required(VERSION 3.14.0)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "")

project("Jitter Monitor"
    # This is your plug-in version number. Change it here only.
    # Version number symbols usable in C++ can be found in
    # source/version.h and ${PROJECT_BINARY_DIR}/projectversion.h.
    VERSION 1.0.0.0 
    DESCRIPTION "Jitter Monitor VST 3 Plug-in"
)

add_subdirectory(extern)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(JitterMonitor
    source/version.h
	source/jitter_algo.h
	source/jitter_algo.cpp
    source/jitter_monitor_param_ids.h
    source/jitter_monitor_cids.h
    source/jitter_monitor_processor.h
    source/jitter_monitor_processor.cpp
    source/jitter_monitor_controller.h
    source/jitter_monitor_controller.cpp
    source/jitter_monitor_entry.cpp
)

#- VSTGUI Wanted ----
if(SMTG_ENABLE_VSTGUI_SUPPORT)
    target_sources(JitterMonitor
        PRIVATE
            resource/jitter_monitor_editor.uidesc
    )
    target_link_libraries(JitterMonitor
        PRIVATE
            vstgui_support
    )
    smtg_target_add_plugin_resources(JitterMonitor
        RESOURCES
            "resource/jitter_monitor_editor.uidesc"
    )
endif(SMTG_ENABLE_VSTGUI_SUPPORT)
# -------------------

smtg_target_add_plugin_snapshots (JitterMonitor
    RESOURCES
        resource/A1DCF1D20AD05BFE93E7CDC4EE65D072_snapshot.png
        resource/A1DCF1D20AD05BFE93E7CDC4EE65D072_snapshot_2.0x.png
)

target_link_libraries(JitterMonitor
    PRIVATE
        sdk
)

smtg_target_configure_version_file(JitterMonitor)

if(SMTG_MAC)
    smtg_target_set_bundle(JitterMonitor
        BUNDLE_IDENTIFIER org.hansen-audio.jitter-monitor
        COMPANY_NAME "Steinberg Media Technologies GmbH"
    )
    smtg_target_set_debug_executable(JitterMonitor
        "/Applications/Cubase 14.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(JitterMonitor PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT JitterMonitor)

        smtg_target_set_debug_executable(JitterMonitor
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)
