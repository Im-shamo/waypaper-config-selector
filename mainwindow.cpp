#include "mainwindow.h"
#include "config.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QString>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QObject>
#include <QDebug>
#include <QFileDialog>
#include <QCheckBox>
#include <QMenu>

MainWindow::MainWindow(Config &config, int columnSize, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , config(config)
    , usingConfig(config.getCurrentConfig())
    , selectedConfig("")
    , configGrid(new QGridLayout())
    , columnSize(columnSize)
{
    ui->setupUi(this);
    createButtonGrid();

    setUsingLabel();
    loadButtons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createButtonGrid()
{
    QVBoxLayout *vbox = new QVBoxLayout();
    QHBoxLayout *hbox = new QHBoxLayout();

    QSpacerItem *leftSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored);
    QSpacerItem *rightSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored);
    QSpacerItem *bottomSpacer = new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::Expanding);

    hbox->setSpacing(10);
    hbox->addSpacerItem(leftSpacer);
    hbox->addLayout(configGrid);
    hbox->addSpacerItem(rightSpacer);

    vbox->addLayout(hbox);
    vbox->addSpacerItem(bottomSpacer);
    ui->configWidget->setLayout(vbox);
}

void MainWindow::loadButtons()
{
    int index = 0;
    buttonList.clear();

    for (const QString &name : config.getWaypaperConfigs())
    {
        int row = index / columnSize;
        int column = index % columnSize;

        QPushButton *button = new QPushButton(name);
        buttonList.push_back(button);
        connect(button, &QPushButton::clicked, this, [=]() {
            this->selectConfig(name);
        });
        configGrid->addWidget(button, row, column);
        index++;
    }
}

void MainWindow::reloadButtons()
{
    // Clear the buttons
    for (QPushButton *button : buttonList)
    {
        configGrid->removeWidget(button);
        delete button;
    }
    loadButtons();
}

void MainWindow::setUsingLabel()
{
    ui->usingLabel->setText(usingConfig);
}

void MainWindow::selectConfig(const QString &name)
{
    QString message("Selected " + name);
    qDebug() << message;
    ui->selectedLabel->setText(name);
    selectedConfig = name;
}

void MainWindow::useConfig()
{
    usingConfig = selectedConfig;
    setUsingLabel();
    config.setCurrentConfig(selectedConfig);
}

void MainWindow::addConfig()
{
    QString name(ui->addName->text());
    QString filePath(QFileDialog::getOpenFileName(this,
        tr("Select a config file"), QDir::homePath(), tr("Text files (*.ini)")));

    if (name == "")
    {
        name = filePath.split("/").last();
    }

    config.addConfig(QDir(filePath), name);
    reloadButtons();
}

void MainWindow::deleteConfig()
{
    if (selectedConfig == "")
    {
        ui->statusbar->showMessage("Select a config");
        return;
    }

    QString massage("Deleted: " + selectedConfig);
    ui->statusbar->showMessage(massage);

    config.deleteConfig(selectedConfig);
    reloadButtons();
    selectedConfig = "";
}

void MainWindow::renameConfig()
{
    if (selectedConfig == "")
    {
        ui->statusbar->showMessage("Select a config");
        return;
    }

    QString newName = ui->renameEdit->text();
    if (config.getWaypaperConfigs().contains(newName))
    {
        qDebug() << "Already exists!";
        ui->statusbar->showMessage("Already exists");
        return;
    }

    if (newName.size() <= 0)
    {
        qDebug() << "New name is empty";
        ui->statusbar->showMessage("New name must not be empty");
        return;
    }
    config.renameConfig(selectedConfig, newName);
    reloadButtons();
    selectedConfig = "";
}
