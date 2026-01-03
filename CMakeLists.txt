cmake_minimum_required(VERSION 3.29)
project(Win_Switcher)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "D:/Qt/6.8.1/msvc2022_64/lib/cmake")
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS ui) # 如果.ui文件不在.cpp同一目录下，需要添加搜索路径

# 设置版本信息变量
set(VERSION_MAJOR 0)
set(VERSION_MINOR 6)
set(VERSION_PATCH 0)

set(COMPANY_NAME "MrBeanCpp")
set(FILE_DESCRIPTION "AltTaber - Window Switcher")
set(PRODUCT_NAME "AltTaber")
# 设置图标的相对路径
# 图标会被缓存，修改后不生效的话，需要删除AppData\Local\IconCache.db & 重启资源管理器，该文件在PC重启后自动创建
set(ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/img/icon.ico")

set(VERSIONINFO_RC "${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.rc")
# 处理.in模板文件 生成版本信息文件
configure_file(
        VersionInfo.rc.in
        ${VERSIONINFO_RC}
        @ONLY # 只处理@var@格式的变量
)

include_directories(include)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Xml
        Network
        REQUIRED)

add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        ${VERSIONINFO_RC}
        res.qrc
        src/widget.cpp
        include/widget.h
        ui/widget.ui
        src/utils/winEventHook.cpp
        include/utils/winEventHook.h
        src/utils/Util.cpp
        include/utils/Util.h
        include/utils/setWindowBlur.h
        src/utils/setWindowBlur.cpp
        src/utils/IconOnlyDelegate.cpp
        include/utils/IconOnlyDelegate.h
        include/utils/uiautomation.h
        src/utils/uiautomation.cpp
        src/utils/AppUtil.cpp
        include/utils/AppUtil.h
        src/utils/KeyboardHooker.cpp
        include/utils/KeyboardHooker.h
        src/utils/TaskbarWheelHooker.cpp
        include/utils/TaskbarWheelHooker.h
        src/utils/QtWin.cpp
        include/utils/QtWin.h
        include/utils/ComInitializer.h
        include/utils/SingleApp.h
        include/utils/Startup.h
        include/utils/SystemTray.h
        include/utils/ConfigManager.h
        include/utils/ConfigManagerBase.h
        src/UpdateDialog.cpp
        include/UpdateDialog.h
        ui/UpdateDialog.ui
        src/utils/ScheduledTask.cpp
        include/utils/ScheduledTask.h)
target_link_libraries(${PROJECT_NAME}
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Xml
        Qt::Network
        Dwmapi
)


# 启用`gui_private`模块
# 必须放在find_package之后，否则`Qt6Gui_PRIVATE_INCLUDE_DIRS`变量未定义
target_include_directories(${PROJECT_NAME} PRIVATE ${Qt6Gui_PRIVATE_INCLUDE_DIRS})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "AltTaber") # 设置输出文件名
if (CMAKE_BUILD_TYPE MATCHES "Release")
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE true) # 设置为窗口应用程序, 无控制台，同时会导致Clion无法捕获输出
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE
        QT_DEPRECATED_WARNINGS
        UNICODE # for Windows API，使用W版本
        _UNICODE # for C/C++ 标准库
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8")
endif ()

# --- Installation and Packaging ---

include(GNUInstallDirs)

# Install the executable
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Application)

# Qt Deployment (Windows)
if(WIN32)
    # Get the location of windeployqt
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS "${_qt_bin_dir}")

    if(WINDEPLOYQT_EXECUTABLE)
        # We use a custom install script to run windeployqt on the installed executable
        install(CODE "
            execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\"
                            --no-translations
                            --compiler-runtime
                            \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/AltTaber.exe\")
        " COMPONENT Application)
    endif()
endif()

# CPack configuration for WiX
set(CPACK_PACKAGE_NAME "${PRODUCT_NAME}")
set(CPACK_PACKAGE_VENDOR "${COMPANY_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${FILE_DESCRIPTION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PRODUCT_NAME}")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")

# WiX specific settings
set(CPACK_GENERATOR "WIX")
set(CPACK_WIX_PRODUCT_GUID "F755C3CE-F23B-426A-B860-8BC87DCD6C75") # Let WiX generate one or specify if needed
set(CPACK_WIX_UPGRADE_GUID "D98A2E8B-1E2A-4B6E-9F9B-7C8D9E0F1A2B") # A stable GUID for upgrades
set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/img/ui.png") # Optional banner
# set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/img/ui.png") # Optional dialog image

# Add Start Menu shortcut
set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "AltTaber")
set(CPACK_WIX_PROGRAM_MENU_FOLDER "${PRODUCT_NAME}")

# Create desktop shortcut (optional, but common for such apps)
set(CPACK_CREATE_DESKTOP_LINKS "${PROJECT_NAME}")

include(CPack)