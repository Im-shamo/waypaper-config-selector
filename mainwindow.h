#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QList>
#include "config.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Config &config, int columnSize, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void selectConfig(const QString &name);
    void useConfig();
    void addConfig();
    void deleteConfig();
    void renameConfig();

private:
    Ui::MainWindow *ui;
    Config &config;
    QString usingConfig;
    QString selectedConfig;
    QGridLayout *configGrid;
    QList<QPushButton*> buttonList;
    int columnSize;

    void createButtonGrid();
    void loadButtons();
    void reloadButtons();
    void setUsingLabel();
};
#endif // MAINWINDOW_H
