#ifndef GROUPWIDGET_H
#define GROUPWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QHash>
#include <QList>

QT_BEGIN_NAMESPACE
class QFormLayout;
class QLineEdit;
class QPushButton;
QT_END_NAMESPACE

class GroupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GroupWidget(QWidget *parent = nullptr);
    explicit GroupWidget(QColor color, QWidget *parent = nullptr);
    QHash<char*, QColor> namesAndColors();

private slots:
    void addLine();
    void removeLine();
    void chooseColor();

private:
    QFormLayout *formLayout;
    QList<QPair<QLineEdit *, QPushButton *>> list;
    QColor mainColor;

    void setUpGroupWidget();
};

#endif // GROUPWIDGET_H
