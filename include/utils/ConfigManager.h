#ifndef WIN_SWITCHER_CONFIGMANAGER_H
#define WIN_SWITCHER_CONFIGMANAGER_H

#include <QSettings>
#include <QApplication>
#include "ConfigManagerBase.h"

// 注意：对于大量使用的类，header-only 模式会导致编译时间过长
#define cfg ConfigManager::instance()

enum DisplayMonitor {
    PrimaryMonitor, // 0 主显示器
    MouseMonitor, // 1 跟随鼠标
    EnumCount // Just for count
};

class ConfigManager : public ConfigManagerBase {
    inline static const QString FileName = "config.ini";

public:
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    static ConfigManager& instance() {
        static const auto filePath = QApplication::applicationDirPath() + "/" + FileName;
        static ConfigManager instance{filePath}; // multiple threads safe
        return instance;
    }

public:
    DisplayMonitor getDisplayMonitor() {
        auto monitor = get("DisplayMonitor", DisplayMonitor::MouseMonitor).toInt();
        if (monitor < 0 || monitor >= DisplayMonitor::EnumCount) {
            qWarning() << "Invalid DisplayMonitor enum" << monitor;
            monitor = DisplayMonitor::MouseMonitor;
        }
        return static_cast<DisplayMonitor>(monitor);
    }

    void setDisplayMonitor(DisplayMonitor monitor) {
        set("DisplayMonitor", monitor);
    }

    bool getShowAllWindows() {
        return get("ShowAllWindows", false).toBool();
    }

    void setShowAllWindows(bool show) {
        set("ShowAllWindows", show);
    }

    // App switch hotkey modifier (VK_MENU or VK_CONTROL)
    int getAppSwitchHotkey() {
        return get("AppSwitchHotkey", VK_MENU).toInt();
    }

    void setAppSwitchHotkey(int vkCode) {
        set("AppSwitchHotkey", vkCode);
    }

    Qt::KeyboardModifier getAppSwitchModifier() {
        return (getAppSwitchHotkey() == VK_CONTROL) ? Qt::ControlModifier : Qt::AltModifier;
    }

    Qt::Key getAppSwitchModifierKey() {
        return (getAppSwitchHotkey() == VK_CONTROL) ? Qt::Key_Control : Qt::Key_Alt;
    }

    // Window switch hotkey modifier (VK_MENU or VK_CONTROL)
    int getWindowSwitchHotkey() {
        return get("WindowSwitchHotkey", VK_MENU).toInt();
    }

    void setWindowSwitchHotkey(int vkCode) {
        set("WindowSwitchHotkey", vkCode);
    }

    Qt::KeyboardModifier getWindowSwitchModifier() {
        return (getWindowSwitchHotkey() == VK_CONTROL) ? Qt::ControlModifier : Qt::AltModifier;
    }

    Qt::Key getWindowSwitchModifierKey() {
        return (getWindowSwitchHotkey() == VK_CONTROL) ? Qt::Key_Control : Qt::Key_Alt;
    }

    // Deprecated: use getAppSwitchHotkey() instead
    int getHotkey() { return getAppSwitchHotkey(); }
    void setHotkey(int vkCode) { setAppSwitchHotkey(vkCode); }
    Qt::KeyboardModifier getHotkeyModifier() { return getAppSwitchModifier(); }
    Qt::Key getHotkeyModifierKey() { return getAppSwitchModifierKey(); }

    // Show preview thumbnails instead of icons
    bool getShowPreview() {
        return get("ShowPreview", false).toBool();
    }

    void setShowPreview(bool show) {
        set("ShowPreview", show);
    }

private:
    explicit ConfigManager(const QString& filename) : ConfigManagerBase(filename) {
        migrateConfig();
    }

    void migrateConfig() {
        // Migrate old single "Hotkey" key to new dual-hotkey config
        if (settings.contains("Hotkey") && !settings.contains("AppSwitchHotkey")) {
            int oldHotkey = get("Hotkey", VK_MENU).toInt();
            set("AppSwitchHotkey", oldHotkey);
            remove("Hotkey");
            sync();
            qDebug() << "Migrated Hotkey config to AppSwitchHotkey:" << oldHotkey;
        }
    }
};


#endif //WIN_SWITCHER_CONFIGMANAGER_H
