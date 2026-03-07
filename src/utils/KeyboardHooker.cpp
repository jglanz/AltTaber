#include "utils/KeyboardHooker.h"
#include <QDebug>
#include <QApplication>
#include <QKeyEvent>
#include "utils/Util.h"
#include "widget.h"

static bool isVkReleased(DWORD vkCode, int hotkeyVK) {
    if (hotkeyVK == VK_MENU)
        return (vkCode == VK_LMENU || vkCode == VK_RMENU || vkCode == VK_MENU);
    if (hotkeyVK == VK_CONTROL)
        return (vkCode == VK_LCONTROL || vkCode == VK_RCONTROL || vkCode == VK_CONTROL);
    return false;
}

LRESULT keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    using Hooker = KeyboardHooker;
    if (nCode == HC_ACTION) {
        if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN) {
            auto* pKeyBoard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

            bool isAppModPressed = Util::isKeyPressed(APP_SWITCH_HOTKEY);
            bool isWinModPressed = Util::isKeyPressed(WIN_SWITCH_HOTKEY);

            if (Hooker::receiver) {
                // App switch: AppSwitchModifier + Tab
                if (isAppModPressed && pKeyBoard->vkCode == VK_TAB) {
                    qDebug() << "App Switch (Mod+Tab) detected!";
                    if ((HWND) Hooker::receiver->winId() != GetForegroundWindow()) {
                        QMetaObject::invokeMethod(Hooker::receiver, "requestShow", Qt::QueuedConnection);
                    } else {
                        auto shiftModifier = Util::isKeyPressed(VK_SHIFT) ? Qt::ShiftModifier : Qt::NoModifier;
                        auto tabDownEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, APP_SWITCH_MODIFIER | shiftModifier);
                        QApplication::postEvent(Hooker::receiver, tabDownEvent);
                    }
                    return 1;
                }

                // Window switch: WinSwitchModifier + ` (backtick)
                if (isWinModPressed && pKeyBoard->vkCode == VK_OEM_3) {
                    qDebug() << "Window Switch (Mod+`) detected!";
                    auto shiftModifier = Util::isKeyPressed(VK_SHIFT) ? Qt::ShiftModifier : Qt::NoModifier;
                    auto event = new QKeyEvent(QEvent::KeyPress, Qt::Key_QuoteLeft, WIN_SWITCH_MODIFIER | shiftModifier);
                    QApplication::postEvent(Hooker::receiver, event);
                    return 1;
                }
            }
        } else if (wParam == WM_KEYUP) {
            auto* pKeyBoard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

            if (Hooker::receiver) {
                bool appSwitchReleased = isVkReleased(pKeyBoard->vkCode, APP_SWITCH_HOTKEY);
                bool winSwitchReleased = isVkReleased(pKeyBoard->vkCode, WIN_SWITCH_HOTKEY);

                if (appSwitchReleased) {
                    qDebug() << "App switch modifier released!";
                    auto event = new QKeyEvent(QEvent::KeyRelease, APP_SWITCH_MODIFIER_KEY, Qt::NoModifier);
                    QApplication::postEvent(Hooker::receiver, event);
                }
                if (winSwitchReleased && WIN_SWITCH_MODIFIER_KEY != APP_SWITCH_MODIFIER_KEY) {
                    // Only post separate event if the modifiers are different
                    qDebug() << "Window switch modifier released!";
                    auto event = new QKeyEvent(QEvent::KeyRelease, WIN_SWITCH_MODIFIER_KEY, Qt::NoModifier);
                    QApplication::postEvent(Hooker::receiver, event);
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

KeyboardHooker::KeyboardHooker(QWidget* _receiver) {
    if (KeyboardHooker::receiver) {
        qWarning() << "Only one KeyboardHooker can be installed!";
        return;
    }
    h_keyboard = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) keyboardProc, GetModuleHandle(nullptr), 0);
    if (!h_keyboard) {
        qWarning() << "Failed to install h_keyboard!";
        return;
    }
    if (!_receiver) {
        qWarning() << "Receiver is nullptr!";
        return;
    }
    KeyboardHooker::receiver = _receiver;
    qInfo() << "KeyboardHooker installed";
}

KeyboardHooker::~KeyboardHooker() {
    if (!h_keyboard) return;
    UnhookWindowsHookEx(h_keyboard);
    KeyboardHooker::receiver = nullptr;
    qDebug() << "KeyboardHooker uninstalled";
}
