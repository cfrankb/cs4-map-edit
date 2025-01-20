#pragma once

#include <QDialog>
#include <stdint.h>

namespace Ui
{
    class CDlgRawAttr;
}

class CDlgRawAttr : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgRawAttr(QWidget *parent = nullptr);
    ~CDlgRawAttr();
    uint8_t attr();
    void attr(const uint8_t &a);

private slots:
    void on_line_AttrEdit_textChanged(const QString &text);

private:
    Ui::CDlgRawAttr *ui;
};
