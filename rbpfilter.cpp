#include <QtWidgets>
#include <QRegularExpressionValidator>
extern "C" {
#include "include/edge.h"
}

#include "rbpfilter.h"

QString GENE_LIST = "data/gene_attribute_edges.txt";
QString TRANS_FACTORS = "data/AURAlight_trans-factors.txt";

RbpFilter::RbpFilter(QWidget *parent) : QDialog(parent)
{
    QLabel *option_label = new QLabel(tr("Filter by:"), this);
    QRadioButton *radio1 = new QRadioButton(tr("Cell type"));
    QRadioButton *radio2 = new QRadioButton(tr("Genes"));

    QLabel *search_label = new QLabel(tr("Search:"), this);
    _cell_edit = new QLineEdit(this);
    _genes_edit = new QTextEdit(this);

    QLabel *corr_label = new QLabel(tr("Min co-regulation"), this);
    _min_coregulation = new QSpinBox(this);
    _min_coregulation->setRange(0,10);
    corr_label->setBuddy(_min_coregulation);

    _layout = new QGridLayout(this);
    _layout->addWidget(option_label,0,0,Qt::AlignRight);
    _layout->addWidget(radio1,0,1,Qt::AlignLeft);
    _layout->addWidget(radio2,0,2,Qt::AlignLeft);
    _layout->addWidget(search_label,1,0,Qt::AlignRight);
    _layout->addWidget(corr_label,2,0, Qt::AlignRight);
    _layout->addWidget(_min_coregulation,2,1,1,2, Qt::AlignLeft);

    _options = new QButtonGroup(this);
    _options->addButton(radio1, 1);
    _options->addButton(radio2, 2);
    connect(_options, &QButtonGroup::idToggled, this, &RbpFilter::displayEdit);
    radio2->setChecked(true);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Close);
    _layout->addWidget(buttons,3,0,1,3);
    connect(buttons, &QDialogButtonBox::accepted, this, &RbpFilter::save);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::close);
}

void RbpFilter::displayEdit(int filter_option, bool checked)
{
    switch (filter_option) {
    case 1:
        if (checked) {
            _cell_edit->setVisible(true); _genes_edit->setVisible(false);
            _cell_edit->setPlaceholderText("Enter cell, ex: HEK293, MCF7...");
            _layout->addWidget(_cell_edit,1,1,1,2);
        }
        break;
    case 2:
        if (checked) {
            _cell_edit->setVisible(false); _genes_edit->setVisible(true);
            _genes_edit->setPlaceholderText("Enter genes, seperated by enter, ex: \nTP53\nFAT1\n...");
            _layout->addWidget(_genes_edit,1,1,1,2);
            _genes_edit->setAcceptRichText(false);
        }
    default:
        break;
    }
}

bool RbpFilter::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save RBPs graph"), tr("rbps_graph"),
                                                    tr("Text files (*.txt)"));
    if (fileName.isEmpty())
        return false;
    QString errorMessage;

    struct EdgeList *result = NULL;
    switch (_options->checkedId()) {
    case 1:
        result = filterByCell(_cell_edit->text(), _min_coregulation->text().toInt());
        break;
    case 2:
        result = filterbyGenes(_genes_edit->toPlainText().split(QRegularExpression("\\s+")), _min_coregulation->text().toInt());
        break;
    default:
        break;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);

        while (result != NULL) {
            out << result->data->start->name << ' ' << result->data->end->name << ' ' << result->data->weight << Qt::endl;
            result = result->nextPtr;
        }

        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                               .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }
    return true;
}

struct EdgeList *RbpFilter::filterByCell(QString cell, int min_co_reg)
{
    QFile file(GENE_LIST);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(nullptr, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(GENE_LIST), file.errorString()));
        return nullptr;
    }

    QStringList genes;

    QString line;
    while (!file.atEnd()) {
        line = file.readLine();
        QStringList args = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        if (args.at(3) == cell)
            genes.append(args.at(0));
    }

    return filterbyGenes(genes, min_co_reg);
}

struct EdgeList *RbpFilter::filterbyGenes(QStringList genes, int min_co_regulated)
{
    QFile file;

    QMap<QString, QStringList> rbp_genes_map;

    file.setFileName(TRANS_FACTORS);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(nullptr, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(TRANS_FACTORS), file.errorString()));
        return nullptr;
    }

    QString line;
    while (!file.atEnd()) {
        line = file.readLine();
        QStringList args = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        QString tf = args.at(0);
        QString gene = args.at(1);

        /* Create a map of trans factors and their targeted genes */
        if (genes.contains(gene) && !rbp_genes_map[tf].contains(gene))
                rbp_genes_map[tf].append(gene);
    }

    struct EdgeList *result = NULL;

    QMapIterator<QString, QStringList> tf1(rbp_genes_map);
    while (tf1.hasNext()) {
        tf1.next();

        QMapIterator<QString, QStringList> tf2 = tf1;
        while (tf2.hasNext()) {
            tf2.next();

            QSet<QString> co_reg = QSet(tf1.value().begin(), tf1.value().end()).intersect(QSet(tf2.value().begin(), tf2.value().end()));

            if (co_reg.count() >= min_co_regulated) {
                char* _name = tf1.key().toLocal8Bit().data();

                struct Vertex *a = (struct Vertex *)malloc(sizeof(struct Vertex));
                a->name = (char *)malloc(strlen(_name)+1);
                strncpy(a->name, _name, strlen(_name)+1);
                a->location = new_vector(0, 0);

                _name = tf2.key().toLocal8Bit().data();

                struct Vertex *b = (struct Vertex *)malloc(sizeof(struct Vertex));
                b->name = (char *)malloc(strlen(_name)+1);
                strncpy(b->name, _name, strlen(_name)+1);
                b->location = new_vector(0, 0);

                double weight = (double)co_reg.count() / genes.count();
                struct Edge *new_edge = newEdge(a, b, weight);
                addEdge(&result, new_edge);
            }
        }
    }
    return result;
}
