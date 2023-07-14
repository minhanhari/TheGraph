#ifndef CENTRALITYWIDGET_H
#define CENTRALITYWIDGET_H

#include <QWidget>
QT_BEGIN_NAMESPACE
class QTextEdit;
class QButtonGroup;
class QCheckBox;
QT_END_NAMESPACE

class CentralityWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CentralityWidget(QWidget *parent = nullptr);
    QTextEdit *resultEdit = nullptr;

    int selectedMethod();
    bool showValue();

private:
    QButtonGroup *_buttons;
    QCheckBox *_showValue;
};

#endif // CENTRALITYWIDGET_H
