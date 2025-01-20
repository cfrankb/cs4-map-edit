#pragma once
#include <QtWidgets/QDialog>

namespace Ui
{
    class DlgAbout;
}

class DlgAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAbout(QWidget *parent = 0);
    ~DlgAbout();

private:
    Ui::DlgAbout *ui;
};
