#include <QtWidgets>
#include "groupwidget.h"

GroupWidget::GroupWidget(QWidget *parent) : QWidget(parent)
{
    setUpGroupWidget();
}

GroupWidget::GroupWidget(QColor color, QWidget *parent) : QWidget(parent), mainColor(color) {
    setUpGroupWidget();
}

QHash<char *, QColor> GroupWidget::namesAndColors()
{
    QHash<char*, QColor> result;

    for (const auto& [line, button] : list) {
        char* name = line->text().toLocal8Bit().data();
        QColor color = button->icon().pixmap(button->size()).toImage().pixelColor(1,1);
        result.insert(name, color);
    }
    return result;
}

void GroupWidget::setUpGroupWidget()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *description = new QLabel(tr("Divide groups in graph by color\n"
                                        "using common string in the names\n"
                                        "of vertexes inside each group."), this);
    layout->addWidget(description);

    QGroupBox *mainbox = new QGroupBox(this);
    formLayout = new QFormLayout(mainbox);
    layout->addWidget(mainbox);

    addLine();

    QToolBar *listToolBar = new QToolBar(this);
    QAction *addAct = new QAction(this->style()->standardIcon(QStyle::SP_ArrowDown), tr("+"));
    addAct->setStatusTip(tr("Add group"));
    connect(addAct, &QAction::triggered, this, &GroupWidget::addLine);
    listToolBar->addAction(addAct);

    QAction *removeAct = new QAction(this->style()->standardIcon(QStyle::SP_ArrowUp), tr("-"));
    removeAct->setStatusTip(tr("Remove group"));
    connect(removeAct, &QAction::triggered, this, &GroupWidget::removeLine);
    listToolBar->addAction(removeAct);

    layout->addWidget(listToolBar);
}

void GroupWidget::addLine()
{
    QLineEdit *newname = new QLineEdit(this);
    QPushButton *newcolor = new QPushButton(this);

    list.append({newname, newcolor});
    formLayout->addRow(newname, newcolor);

    newname->setPlaceholderText(tr("Group ") + QString::number(list.size()));

    connect(newcolor, &QPushButton::clicked, this, &GroupWidget::chooseColor);
    QPixmap colorbox(newcolor->sizeHint()); colorbox.fill(mainColor);
    newcolor->setIcon(QIcon(colorbox));
}

void GroupWidget::removeLine()
{
    int rowcount = list.size();
    if (rowcount <= 1) return;

    list.removeLast();
    formLayout->removeRow(rowcount - 1);
}

void GroupWidget::chooseColor()
{
    QPushButton *button = dynamic_cast<QPushButton *>(sender());

    QColor color = QColorDialog::getColor(button->icon().pixmap(button->size()).toImage().pixelColor(1,1), this);
    if (color.isValid()){
        QPixmap colorbox(button->size()); colorbox.fill(color);
        button->setIcon(QIcon(colorbox));
    }
}
