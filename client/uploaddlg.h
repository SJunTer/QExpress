#ifndef UPLOADDLG_H
#define UPLOADDLG_H

#include <QDialog>
#include <QString>


#define ACCIDENT 0
#define JAM 1
#define BLOCK 2

QT_BEGIN_NAMESPACE
class ClientSocket;
class QButtonGroup;
QT_END_NAMESPACE


namespace Ui {
class UploadDlg;
}

class UploadDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UploadDlg(ClientSocket *cli, QWidget *parent = 0);
    ~UploadDlg();

private:
    Ui::UploadDlg *ui;

    QButtonGroup *buttonGroup;
    ClientSocket *client;
    int type;
    QString address;
    QString detail;

protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void closed();

private slots:
    void on_submitBtn_clicked();
    void on_cancelBtn_clicked();


};

#endif // UPLOADDLG_H
