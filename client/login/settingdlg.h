#ifndef SETTINGDLG_H
#define SETTINGDLG_H

#include <QDialog>
#include <QString>

namespace Ui {
class SettingDlg;
}

class SettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDlg(QString &s, int p, QWidget *parent = 0);
    ~SettingDlg();

signals:
    void setInfo(QString &s, int p);

private slots:
    void on_okBtn_clicked();

private:
    Ui::SettingDlg *ui;
    QString ip;
    int port;
};

#endif // SETTINGDLG_H
