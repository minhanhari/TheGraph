// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

//! [0]
#include <QtWidgets>

#include <QPainter>
#include <QSvgGenerator>
#include "mainwindow.h"
#include "renderarea.h"
#include "groupwidget.h"
#include "pathwidget.h"
#include "centralitywidget.h"
#include "settingdialog.h"
#include "rbpfilter.h"
#include <iostream>
extern "C" {
#include "include/graph.h"
}
//! [0]

//! [1]
MainWindow::MainWindow()
{
    renderArea = new RenderArea(this);
    setCentralWidget(renderArea);

    createActions();
    createStatusBar();

    readSettings();

#ifndef QT_NO_SESSIONMANAGER
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}
//! [2]

//! [3]
void MainWindow::closeEvent(QCloseEvent *event)
//! [3] //! [4]
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}
//! [4]

//! [5]
void MainWindow::newFile()
//! [5] //! [6]
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}
//! [6]

//! [7]
void MainWindow::open()
//! [7] //! [8]
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            openFile(fileName);
    }
}
//! [8]

//! [9]
bool MainWindow::save()
//! [9] //! [10]
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}
//! [10]

//! [11]
bool MainWindow::saveAs()
//! [11] //! [12]
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}
//! [12]

bool MainWindow::exportSvg()
{
    QString newPath = QFileDialog::getSaveFileName(this, tr("Save SVG"), QFileInfo(curFile).baseName(),
                                                   tr("SVG files (*.svg)"));
    if (newPath.isEmpty())
        return false;

    QSvgGenerator generator;
    generator.setFileName(newPath);
    generator.setSize(renderArea->size());
    generator.setViewBox(QRect(0, 0, renderArea->size().width(), renderArea->size().height()));

    QPainter painter(&generator);
    renderArea->paintGraph(&painter);

    return true;
}

//! [13]
void MainWindow::about()
//! [13] //! [14]
{
   QMessageBox::about(this, tr("About The Graph"),
            tr("The <b>Graph</b> is a program, used to "
               "analyze graph using some basic problems "
               "in graph theory."));
}
//! [14]

void MainWindow::setting()
{
   SettingDialog::setRenderArea(renderArea);
}

void MainWindow::createDock()
{
   if (dock != nullptr) dock->close();

   dock = new QDockWidget(this);
   dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

   QWidget *container = new QWidget;
   dock->setWidget(container);
   QVBoxLayout *layout = new QVBoxLayout(container);
   layout->setDirection(QBoxLayout::BottomToTop);

   QDialogButtonBox *button = new QDialogButtonBox(QDialogButtonBox::Ok);
   layout->addWidget(button);
   layout->addStretch(0);
}

void MainWindow::group()
{
   createDock();
   dock->setWindowTitle(tr("Display groups"));
   addDockWidget(Qt::LeftDockWidgetArea, dock);

   QLayout *dock_layout = dock->widget()->layout();
   if (groupWidget == nullptr) groupWidget = new GroupWidget(renderArea->mainColor(), dock);
   dock_layout->addWidget(groupWidget);

   QDialogButtonBox *button = dynamic_cast<QDialogButtonBox*>(dock_layout->itemAt(0)->widget());
   connect(button, &QDialogButtonBox::accepted, this, &MainWindow::setGroupColors);
}

void MainWindow::setGroupColors()
{
   QHash<char*, QColor> list = groupWidget->namesAndColors();

   for (int i = 0; i < graph->vertexes_num; ++i) {
        QHashIterator<char*, QColor> it(list);
        while (it.hasNext()) {
            it.next();
            if (strstr(graph->vertexes[i]->name, it.key())) {
                renderArea->setColor(i, it.value());
            }
        }
    }
}

void MainWindow::path()
{
    createDock();
    dock->setWindowTitle(tr("Calculate shortest path"));
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    QLayout *dock_layout = dock->widget()->layout();
    if (pathWidget == nullptr) pathWidget = new PathWidget(dock);
    dock_layout->addWidget(pathWidget);

    QDialogButtonBox *button = dynamic_cast<QDialogButtonBox*>(dock_layout->itemAt(0)->widget());
    connect(button, &QDialogButtonBox::accepted, this, &MainWindow::getPath);
}

void MainWindow::getPath()
{
    struct Vertex *source = nullptr, *target = nullptr;
    char* src_name = pathWidget->source();
    char* tgt_name = pathWidget->target();

    for (int i = 0; i < graph->vertexes_num; ++i) {
        struct Vertex *v = graph->vertexes[i];
        if (strcmp(src_name, v->name) == 0)
            source = v;
        else if (strcmp(tgt_name, v->name) == 0)
            target = v;
        else
            renderArea->setColor(i, Qt::gray);
    }

    if (!source || !target) return;

    double result;
    QStringList result_string("Path:");

    result_string.append(source->name);
    renderArea->setColor(vertexIndex(graph, source), renderArea->mainColor());

    struct EdgeList *result_edge_list = Dijsktra(graph, source, target, &result);
    while (result_edge_list != NULL) {
        struct Edge* edge = result_edge_list->data;

        if (result_string.contains(edge->start->name)) {
            result_string.append(edge->end->name);
            renderArea->setColor(vertexIndex(graph, edge->end), renderArea->mainColor());
        }
        else {
            result_string.append(edge->start->name);
            renderArea->setColor(vertexIndex(graph, edge->start), renderArea->mainColor());
        }
        result_edge_list = result_edge_list->nextPtr;
    };
    result_string.append("\nResult:"); result_string.append(QString::number(result));

    pathWidget->resultEdit->setPlainText(result_string.join("\n"));
}

void MainWindow::centrality()
{
    createDock();
    dock->setWindowTitle(tr("Calculate centrality"));
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    QVBoxLayout *dock_layout = dynamic_cast<QVBoxLayout*>(dock->widget()->layout());
    if (centralityWidget == nullptr) centralityWidget = new CentralityWidget(dock);
    dock_layout->addWidget(centralityWidget, 1);

    QDialogButtonBox *button = dynamic_cast<QDialogButtonBox*>(dock_layout->itemAt(0)->widget());
    connect(button, &QDialogButtonBox::accepted, this, &MainWindow::getCentrality);
}

bool secondLess(QPair<int, double> pair_a, QPair<int, double> pair_b)
{
    return pair_a.second < pair_b.second;
}

bool secondMore(QPair<int, double> pair_a, QPair<int, double> pair_b)
{
    return pair_a.second > pair_b.second;
}

void MainWindow::getCentrality()
{
    QList<QPair<int, double>> centralities;

    switch (centralityWidget->selectedMethod()) {
    case 1:
        {
            double* results = DegreeCentrality(graph);
            for (int i = 0; i < graph->vertexes_num; ++i) {
                centralities.append(QPair<int, double>(i, results[i]));
            }
        }
        break;
    case 2:
        {
            double* results = ClosenessCentrality(graph);
            for (int i = 0; i < graph->vertexes_num; ++i) {
                centralities.append(QPair<int, double>(i, results[i]));
            }
        }
        break;
    case 3:
        {
            double* results = BetweennessCentrality(graph);
            for (int i = 0; i < graph->vertexes_num; ++i) {
                centralities.append(QPair<int, double>(i, results[i]));
            }
        }
        break;
    default:
        return;
    }

    std::sort(centralities.begin(), centralities.end(), secondMore);
    double high_centrality = centralities.first().second;
    double low_centrality = centralities.last().second;
    bool show_value = centralityWidget->showValue();

    QStringList result_string("Centralities:");
    QPair<int, double> pair;
    foreach (pair, centralities) {
        if (show_value)
            result_string.append(tr(graph->vertexes[pair.first]->name) + ": " + QString::number(pair.second));
        else
            result_string.append(tr(graph->vertexes[pair.first]->name));

        QColor color = renderArea->mainColor();
        float value = (pair.second - low_centrality) / (high_centrality - low_centrality);
        renderArea->setColor(pair.first, color.fromHsvF(color.hueF(), color.saturationF(), value));
    }

    centralityWidget->resultEdit->setPlainText(result_string.join("\n"));
}

void MainWindow::rbp()
{
    RbpFilter *rbp_dialog = new RbpFilter(this);
    rbp_dialog->show();
}
//! [17]
void MainWindow::createActions()
//! [17] //! [18]
{

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

//! [19]
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/load.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);
//! [18] //! [19]

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

//! [20]
    fileMenu->addSeparator();

    QMenu *exportMenu = fileMenu->addMenu(tr("&Export"));
    QAction *svgAct = exportMenu->addAction(tr("SVG"), this, &MainWindow::exportSvg);
    svgAct->setStatusTip(tr("Export graph as SVG file"));
    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
//! [20]
    exitAct->setStatusTip(tr("Exit the application"));

    QToolBar *graphToolBar = addToolBar(tr("Graph"));
    graphToolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

    const QIcon groupIcon = QIcon::fromTheme("graph-group", QIcon(":/images/network.png"));
    QAction *groupAct = new QAction(groupIcon, tr("Group"), this);
    //! [21]
    groupAct->setStatusTip(tr("Change display colors of different groups in graph"));
    connect(groupAct, &QAction::triggered, this, &MainWindow::group);
    graphToolBar->addAction(groupAct);

    const QIcon communityIcon = QIcon::fromTheme("graph-community", QIcon(":/images/community.png"));
    QAction *communityAct = new QAction(communityIcon, tr("Community"), this);
//! [21]
    communityAct->setStatusTip(tr("Detect communities in graph"));
//    connect(communityAct, &QAction::triggered, textEdit, &QPlainTextEdit::cut);
    graphToolBar->addAction(communityAct);

    const QIcon centralityIcon = QIcon::fromTheme("graph-centrality", QIcon(":/images/centrality.png"));
    QAction *centralityAct = new QAction(centralityIcon, tr("Centrality"), this);
    centralityAct->setStatusTip(tr("Calculate centrality of each vertex"));
    connect(centralityAct, &QAction::triggered, this, &MainWindow::centrality);
    graphToolBar->addAction(centralityAct);

    const QIcon pathIcon = QIcon::fromTheme("graph-path", QIcon(":/images/path.png"));
    QAction *pathAct = new QAction(pathIcon, tr("Shortest path"), this);
    pathAct->setStatusTip(tr("Find the shortest path between to vertexes"));
    connect(pathAct, &QAction::triggered, this, &MainWindow::path);
    graphToolBar->addAction(pathAct);

    QMenu *taskMenu = menuBar()->addMenu(tr("&Task"));
    QAction *rbpAct = taskMenu->addAction(tr("&RBP"), this, &MainWindow::rbp);
    rbpAct->setStatusTip("Graph connection of RBPs");

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *settingAct = helpMenu->addAction(tr("S&ettings"), this, &MainWindow::setting);
    settingAct->setStatusTip(tr("Change the display of elements in the graph"));

    if (graph == NULL) {
        groupAct->setEnabled(false);
        communityAct->setEnabled(false);
        centralityAct->setEnabled(false);
        pathAct->setEnabled(false);
        settingAct->setEnabled(false);

        svgAct->setEnabled(false);
    }
    connect(this, &MainWindow::graphLoaded, groupAct, &QAction::setEnabled);
    connect(this, &MainWindow::graphLoaded, communityAct, &QAction::setEnabled);
    connect(this, &MainWindow::graphLoaded, centralityAct, &QAction::setEnabled);
    connect(this, &MainWindow::graphLoaded, pathAct, &QAction::setEnabled);
    connect(this, &MainWindow::graphLoaded, settingAct, &QAction::setEnabled);

    connect(this, &MainWindow::graphLoaded, svgAct, &QAction::setEnabled);

    helpMenu->addSeparator();

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

//! [22]

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
//! [22]
}
//! [24]

//! [32]
void MainWindow::createStatusBar()
//! [32] //! [33]
{
    statusBar()->showMessage(tr("Ready"));
}
//! [33]

//! [34] //! [35]
void MainWindow::readSettings()
//! [34] //! [36]
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}
//! [35] //! [36]

//! [37] //! [38]
void MainWindow::writeSettings()
//! [37] //! [39]
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}
//! [38] //! [39]

//! [40]
bool MainWindow::maybeSave()
//! [40] //! [41]
{
    if (graph == nullptr || curFile.isEmpty())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}
//! [41]

//! [42]
void MainWindow::loadFile(const QString &fileName)
//! [42] //! [43]
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    struct VertexList *vert_list = NULL;
    struct EdgeList *edge_list = NULL;

    QString line;
    while (!file.atEnd()) {
        line = file.readLine();
        QStringList list = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        if (list.size() >= 1) {
            char* _name = list.at(0).toLocal8Bit().data();

            struct Vertex *a = (struct Vertex *)malloc(sizeof(struct Vertex));
            a->name = (char *)malloc(strlen(_name)+1);
            strncpy(a->name, _name, strlen(_name)+1);
            a->location = new_vector(0, 0);

            if (list.size() >= 2) {
                _name = list.at(1).toLocal8Bit().data();

                struct Vertex *b = (struct Vertex *)malloc(sizeof(struct Vertex));
                b->name = (char *)malloc(strlen(_name)+1);
                strncpy(b->name, _name, strlen(_name)+1);
                b->location = new_vector(0, 0);

                if (strcmp(a->name, b->name) != 0) {
                struct Edge *e = (struct Edge *)malloc(sizeof(struct Edge));
                e->start = addVertex(&vert_list, a)->data;
                e->end = addVertex(&vert_list, b)->data;
                e->weight = list.size() == 3 ? list.at(2).toDouble() : 1;

                addEdge(&edge_list, e);
                }
                else {
                addVertex(&vert_list, a);
                free(b);
                }
            }
            else
                addVertex(&vert_list, a);
        }
    }
    graph = newGraph(vert_list, edge_list);
    emit graphLoaded(graph != nullptr);

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Graph loaded"), 2000);
    printf("Vertexes number: %i\n", graph->vertexes_num);
    printf("Edges number: %i\n", graph->edges_num);

    renderArea->setGraph(graph);
}
//! [43]
void MainWindow::openFile(const QString &fileName)
//! [42] //! [43]
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    struct VertexList *vert_list = NULL;
    struct EdgeList *edge_list = NULL;

    QString line;
    while (!file.atEnd()) {
        line = file.readLine();
        QStringList list = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        if (list.size() > 2) {
            char* _name = list.at(1).toLocal8Bit().data();

            struct Vertex *a = (struct Vertex *)malloc(sizeof(struct Vertex));
            a->name = (char *)malloc(strlen(_name)+1);
            strncpy(a->name, _name, strlen(_name)+1);
            if (list.at(0) == "v" && list.size() == 4) {
                a->location = new_vector(list[2].toDouble(), list[3].toDouble());
                addVertex(&vert_list, a);
            }
            else {
                if (list.at(0) == "e" && list.size() == 4){
                _name = list.at(2).toLocal8Bit().data();

                struct Vertex *b = (struct Vertex *)malloc(sizeof(struct Vertex));
                b->name = (char *)malloc(strlen(_name)+1);
                strncpy(b->name, _name, strlen(_name)+1);

                struct Edge *e = (struct Edge *)malloc(sizeof(struct Edge));
                e->start = addVertex(&vert_list, a)->data;
                e->end = addVertex(&vert_list, b)->data;
                e->weight = list.at(3).toDouble();

                addEdge(&edge_list, e);
                }
                else
                free(a);
            }
        }
    }
    graph = newGraph(vert_list, edge_list);
    emit graphLoaded(graph != nullptr);

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Graph loaded"), 2000);
    printf("Vertexes number: %i\n", graph->vertexes_num);
    printf("Edges number: %i\n", graph->edges_num);

    renderArea->setGraph(graph);
}
//! [44]
bool MainWindow::saveFile(const QString &fileName)
//! [44] //! [45]
{
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);

        struct Vertex** vertexes = graph->vertexes;
        for (int i = 0; i < graph->vertexes_num; ++i) {
            out << "v " << vertexes[i]->name << ' ' << vertexes[i]->location.x << ' ' << vertexes[i]->location.y << '\n';
        }
        struct Edge** edges = graph->edges;
        for (int i = 0; i < graph->edges_num; ++i) {
            out << "e " << edges[i]->start->name << ' ' << edges[i]->end->name << ' ' << edges[i]->weight << '\n';
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

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
//! [45]

//! [46]
void MainWindow::setCurrentFile(const QString &fileName)
//! [46] //! [47]
{
    curFile = fileName;
    //textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "The Graph";
    setWindowFilePath(shownName);
}
//! [47]

//! [48]
QString MainWindow::strippedName(const QString &fullFileName)
//! [48] //! [49]
{
    return QFileInfo(fullFileName).fileName();
}
//! [49]

#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (graph != NULL && !curFile.isEmpty())
            save();
    }
}
#endif
