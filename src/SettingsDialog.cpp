#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "utils/ConfigManager.h"
#include "utils/Startup.h"
#include "utils/Util.h"

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    loadSettings();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::saveSettings);
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::loadSettings() {
    // App switch hotkey modifier
    ui->combo_appSwitchMod->setCurrentIndex(cfg.getAppSwitchHotkey() == VK_CONTROL ? 1 : 0);

    // Window switch hotkey modifier
    ui->combo_winSwitchMod->setCurrentIndex(cfg.getWindowSwitchHotkey() == VK_CONTROL ? 1 : 0);

    // General
    ui->check_startup->setChecked(Startup::isOn());
    ui->check_preview->setChecked(cfg.getShowPreview());
    ui->check_showAllWindows->setChecked(cfg.getShowAllWindows());

    // Display Monitor
    if (cfg.getDisplayMonitor() == PrimaryMonitor)
        ui->radio_primaryMonitor->setChecked(true);
    else
        ui->radio_mouseMonitor->setChecked(true);
}

void SettingsDialog::saveSettings() {
    // App switch hotkey modifier
    int appHotkey = (ui->combo_appSwitchMod->currentIndex() == 1) ? VK_CONTROL : VK_MENU;
    cfg.setAppSwitchHotkey(appHotkey);

    // Window switch hotkey modifier
    int winHotkey = (ui->combo_winSwitchMod->currentIndex() == 1) ? VK_CONTROL : VK_MENU;
    cfg.setWindowSwitchHotkey(winHotkey);

    // General
    Startup::set(ui->check_startup->isChecked());
    cfg.setShowPreview(ui->check_preview->isChecked());
    cfg.setShowAllWindows(ui->check_showAllWindows->isChecked());

    // Display Monitor
    DisplayMonitor monitor = ui->radio_primaryMonitor->isChecked() ? PrimaryMonitor : MouseMonitor;
    cfg.setDisplayMonitor(monitor);

    cfg.sync();
    emit cfg.configEdited();
}
