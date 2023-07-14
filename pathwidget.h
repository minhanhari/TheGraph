
#ifndef PATHWIDGET_H
#define PATHWIDGET_H


#include <QWidget>
QT_BEGIN_NAMESPACE
class QLineEdit;
class QTextEdit;
QT_END_NAMESPACE

class PathWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PathWidget(QWidget *parent = nullptr);
    QTextEdit* resultEdit = nullptr;
    char* source();
    char* target();

private:
    QLineEdit *_source;
    QLineEdit *_target;
};

#endif // PATHWIDGET_H
