#include <QtWidgets>

#include "centralitywidget.h"

CentralityWidget::CentralityWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *description = new QLabel("Calculate centrality of all vertexes\n"
                                     "using one of the methods below\n"
                                     "and sort from highest to lowest.\n");
    layout->addWidget(description);

    QGroupBox *main_box = new QGroupBox;
    layout->addWidget(main_box);

    QRadioButton *radio1 = new QRadioButton(tr("&Degree centrality"));
    QRadioButton *radio2 = new QRadioButton(tr("&Closeness centrality"));
    QRadioButton *radio3 = new QRadioButton(tr("&Betweenness centrality"));
    radio1->setChecked(true);

    _showValue = new QCheckBox(tr("Show &value"),this);
    _showValue->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout(main_box);
    vbox->addWidget(radio1);
    vbox->addWidget(radio2);
    vbox->addWidget(radio3);
    vbox->addWidget(_showValue);
    //vbox->addStretch(1);

    _buttons = new QButtonGroup(this);
    _buttons->addButton(radio1, 1);
    _buttons->addButton(radio2, 2);
    _buttons->addButton(radio3, 3);

    resultEdit = new QTextEdit(this);
    resultEdit->setReadOnly(true);
    QScrollArea* scroll_area = new QScrollArea;
    scroll_area->setVisible(true);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(resultEdit);
    layout->addWidget(scroll_area, 1);
}

int CentralityWidget::selectedMethod()
{
    return _buttons->checkedId();
}

bool CentralityWidget::showValue()
{
    return _showValue->isChecked();
}
