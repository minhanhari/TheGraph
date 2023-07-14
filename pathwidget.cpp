#include <QtWidgets>
#include "pathwidget.h"

PathWidget::PathWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *description = new QLabel("Calculate the shortest path\n"
                                     "from source to target\n"
                                     "using Dijkstra algorithm.");
    layout->addWidget(description);

    QGroupBox *main_box = new QGroupBox;
    QFormLayout *form_layout = new QFormLayout(main_box);
    layout->addWidget(main_box);

    QLabel *label = new QLabel("Source:");
    _source = new QLineEdit; _source->setPlaceholderText("source vertex name");
    form_layout->addRow(label, _source);

    label = new QLabel("Target:");
    _target = new QLineEdit; _target->setPlaceholderText("target vertex name");
    form_layout->addRow(label, _target);

    resultEdit = new QTextEdit(this);
    resultEdit->setReadOnly(true);
    layout->addWidget(resultEdit);
}

char* PathWidget::source() { return _source->text().toLocal8Bit().data(); }
char* PathWidget::target() { return _target->text().toLocal8Bit().data(); }
