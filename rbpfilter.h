#ifndef RBPFILTER_H
#define RBPFILTER_H

#include <QDialog>
QT_BEGIN_NAMESPACE
class QButtonGroup;
class QLineEdit;
class QTextEdit;
class QSpinBox;
class QGridLayout;
QT_END_NAMESPACE
struct EdgeList;

class RbpFilter : public QDialog
{
    Q_OBJECT

public:
    explicit RbpFilter(QWidget *parent = nullptr);
    static struct EdgeList *filterByCell(QString cell, int min_co_regulated);
    static struct EdgeList *filterbyGenes(QStringList genes, int min_co_regulated);

public slots:
    bool save();

private slots:
    void displayEdit(int filter_option, bool checked);

private:
    QButtonGroup *_options;
    QLineEdit *_cell_edit;
    QTextEdit *_genes_edit;
    QSpinBox *_min_coregulation;
    QGridLayout *_layout;
};

#endif // RBPFILTER_H
