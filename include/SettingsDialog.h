#ifndef WIN_SWITCHER_SETTINGSDIALOG_H
#define WIN_SWITCHER_SETTINGSDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsDialog; }
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog() override;

private:
    void loadSettings();
    void saveSettings();

    Ui::SettingsDialog* ui;
};

#endif // WIN_SWITCHER_SETTINGSDIALOG_H
