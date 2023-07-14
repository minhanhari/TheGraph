#include <QtWidgets>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>

#include "settingdialog.h"

#include "renderarea.h"

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent)
{
    QLabel *layoutLabel = new QLabel(tr("Layout"), this);
    layoutBox = new QComboBox(this);
    layoutBox->addItem("Force-Directed - Fruchterman-Reingold", QVariant(RenderArea::FR));
    layoutBox->addItem("Local Minimum - Kamanda-Kawaii", QVariant(RenderArea::KK));
    layoutLabel->setBuddy(layoutBox);

    QLabel *vertexSizeLabel = new QLabel(tr("Vertex size"), this);
    vertexSizeBox = new QSpinBox(this);
    vertexSizeBox->setRange(6, 30);
    vertexSizeLabel->setBuddy(vertexSizeBox);

    QLabel *vertexColorLabel = new QLabel(tr("Vertex color"), this);
    vertexColorBox = new QPushButton(this);
    connect(vertexColorBox, &QPushButton::clicked, this, &SettingDialog::chooseColor);
    vertexColorLabel->setBuddy(vertexColorBox);

    QLabel *edgeWidthLabel = new QLabel(tr("Edge width"), this);
    edgeWidthBox = new QSpinBox(this);
    edgeWidthBox->setRange(1, 10);
    edgeWidthLabel->setBuddy(edgeWidthBox);

    QLabel *edgeBlackLabel = new QLabel(tr("Edge blackness"), this);
    edgeBlackPercent = new QSlider(Qt::Horizontal, this);
    edgeBlackPercent->setRange(1, 199);
    edgeBlackLabel->setBuddy(edgeBlackPercent);

    QLabel *fontSizeLabel = new QLabel(tr("Font size"), this);
    fontSizeBox = new QSpinBox(this);
    fontSizeBox->setRange(6, 30);
    fontSizeLabel->setBuddy(fontSizeBox);

    antialiasingCheckBox = new QCheckBox(tr("&Antialiasing"), this);
    displayTextCheckBox = new QCheckBox(tr("Display &Text"), this);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QGridLayout *mainlayout = new QGridLayout;
    mainlayout->addWidget(layoutLabel, 0, 0, Qt::AlignRight);
    mainlayout->addWidget(layoutBox, 0, 1);
    mainlayout->addWidget(vertexSizeLabel, 1, 0, Qt::AlignRight);
    mainlayout->addWidget(vertexSizeBox, 1, 1, Qt::AlignLeft);
    mainlayout->addWidget(vertexColorLabel, 2, 0, Qt::AlignRight);
    mainlayout->addWidget(vertexColorBox, 2, 1, Qt::AlignLeft);
    mainlayout->addWidget(edgeWidthLabel, 3, 0, Qt::AlignRight);
    mainlayout->addWidget(edgeWidthBox, 3, 1, Qt::AlignLeft);
    mainlayout->addWidget(edgeBlackLabel, 4, 0, Qt::AlignRight);
    mainlayout->addWidget(edgeBlackPercent, 4, 1);
    mainlayout->addWidget(fontSizeLabel, 5, 0, Qt::AlignRight);
    mainlayout->addWidget(fontSizeBox, 5, 1, Qt::AlignLeft);
    mainlayout->addWidget(antialiasingCheckBox, 6, 0, Qt::AlignLeft);
    mainlayout->addWidget(displayTextCheckBox, 6, 1, Qt::AlignLeft);
    mainlayout->addWidget(buttonBox, 7, 1);

    setLayout(mainlayout);
}

RenderArea::Layout fromVariant(QVariant var)
{
    if (var == QVariant(RenderArea::FR))
        return RenderArea::FR;
    else
        return RenderArea::KK;
}

void SettingDialog::setRenderArea(RenderArea *area)
{
    SettingDialog dialog;

    QPixmap colorbox(dialog.vertexColorBox->sizeHint());
    colorbox.fill(area->mainColor());
    dialog.vertexColorBox->setIcon(QIcon(colorbox));
    dialog.layoutBox->setCurrentIndex(dialog.layoutBox->findData(QVariant(area->layout())));
    dialog.vertexSizeBox->setValue(area->circleSize());
    dialog.edgeWidthBox->setValue(area->pen().width());
    dialog.edgeBlackPercent->setValue((1.0 - area->pen().color().valueF()) * 200);
    dialog.fontSizeBox->setValue(area->fontSize());
    dialog.antialiasingCheckBox->setChecked(area->antialiased());
    dialog.displayTextCheckBox->setChecked(area->displayText());

    if (dialog.exec() == QDialog::Accepted) {
        area->setLayout(fromVariant(dialog.layoutBox->currentData()));
        area->setMainColor(dialog.vertexColorBox->icon().pixmap(dialog.vertexColorBox->size()).toImage().pixelColor(1,1));
        area->setCircleSize(dialog.vertexSizeBox->value());
        area->setPen(QPen(QColor::fromHsvF(0,0, (1.0 - dialog.edgeBlackPercent->value() / 200.0)), dialog.edgeWidthBox->value()));
        area->setFontSize(dialog.fontSizeBox->value());
        area->setAntialiased(dialog.antialiasingCheckBox->isChecked());
        area->setDisplayText(dialog.displayTextCheckBox->isChecked());
    }
}

void SettingDialog::chooseColor()
{
    QColor color = QColorDialog::getColor(vertexColorBox->icon().pixmap(vertexColorBox->size()).toImage().pixelColor(1,1), this);
    if (color.isValid()){
        QPixmap colorbox(vertexColorBox->size()); colorbox.fill(color);
        vertexColorBox->setIcon(QIcon(colorbox));
    }
}
