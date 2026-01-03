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
    // Hotkey
    int hotkey = cfg.getHotkey();
    if (hotkey == VK_CONTROL) {
        ui->combo_hotkey->setCurrentIndex(1);
    } else {
        ui->combo_hotkey->setCurrentIndex(0);
    }

    // Window Mode
    bool showAll = cfg.getShowAllWindows();
    ui->combo_windowMode->setCurrentIndex(showAll ? 1 : 0);

    // Startup
    ui->check_startup->setChecked(Startup::isOn());
}

void SettingsDialog::saveSettings() {
    // Hotkey
    int hotkey = (ui->combo_hotkey->currentIndex() == 1) ? VK_CONTROL : VK_MENU;
    cfg.setHotkey(hotkey);

    // Window Mode
    bool showAll = (ui->combo_windowMode->currentIndex() == 1);
    cfg.setShowAllWindows(showAll);

    // Startup
    Startup::set(ui->check_startup->isChecked());

    cfg.sync();
    emit cfg.configEdited();
}
