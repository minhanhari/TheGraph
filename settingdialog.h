#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QSpinBox;
class QSlider;
class QPushButton;
class QCheckBox;
QT_END_NAMESPACE
class RenderArea;

class SettingDialog : public QDialog
{
    Q_OBJECT
public:
    SettingDialog(QWidget *parent = nullptr);
    static void setRenderArea(RenderArea *area);

private slots:
    void chooseColor();

private:
    QComboBox *layoutBox;
    QSpinBox *vertexSizeBox;
    QPushButton *vertexColorBox;
    QSpinBox *edgeWidthBox;
    QSlider *edgeBlackPercent;
    QSpinBox *fontSizeBox;
    QCheckBox *antialiasingCheckBox;
    QCheckBox *displayTextCheckBox;
};

#endif // SETTINGDIALOG_H
