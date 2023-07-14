// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QSessionManager;
QT_END_NAMESPACE
class RenderArea;
struct Graph;
class GroupWidget;
class PathWidget;
class CentralityWidget;

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    void loadFile(const QString &fileName);
    void openFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    bool exportSvg();
    void about();
    void setting();

    void group();
    void setGroupColors();
    void path();
    void getPath();
    void centrality();
    void getCentrality();

    void rbp();
#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private: signals:
    void graphLoaded(bool success);

private:
    struct Graph *graph = nullptr;
    RenderArea *renderArea;
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    void createDock();

    QDockWidget *dock = nullptr;
    GroupWidget *groupWidget = nullptr;
    PathWidget *pathWidget = nullptr;
    CentralityWidget *centralityWidget = nullptr;

    QString curFile;
};
//! [0]

#endif
