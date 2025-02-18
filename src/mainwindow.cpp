#include "mainwindow.h"
#include "preference.h"
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
    , columnSize(columnSize)
{
    ui->setupUi(this);
    setUsingLabel();
    loadButtons();
    loadOptions();
}

MainWindow::~MainWindow()
{
    delete ui;
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
        ui->configGrid->addWidget(button, row, column);
        index++;
    }
}

void MainWindow::loadOptions()
{
    WindowSystem sys = config.getWindowSystem();
    ui->windowSystemComboBox->insertItems(0, config.getWindowSystemList());
    ui->windowSystemComboBox->setCurrentIndex(sys);
}

void MainWindow::reloadButtons()
{
    // Clear the buttons
    for (QPushButton *button : std::as_const(buttonList))
    {
        ui->configGrid->removeWidget(button);
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
    ui->selectedLabel->setText(name);
    ui->backendLabel->setText(config.getConfigSettings(name, "Settings/backend", "/").toString());
    selectedConfig = name;
    qDebug() << "Selected " + name;
}

void MainWindow::useConfig()
{
    usingConfig = selectedConfig;
    if (config.getAutoChangeBackend())
    {
        switch (config.getWindowSystem())
        {
        case WindowSystem::xorg:
            config.changeConfigSettings(usingConfig, "Settings/backend", config.getXorgBackendPreference());
            break;

        case WindowSystem::wayland:
            config.changeConfigSettings(usingConfig, "Settings/backend", config.getWaylandBackendPreference());
            break;
        }
    }
    ui->backendLabel->setText(config.getConfigSettings(usingConfig, "Settings/backend", "/").toString());
    setUsingLabel();
    config.setCurrentConfig(usingConfig);
}

void MainWindow::selectAndUseConfig(const QString &name)
{
    selectConfig(name);
    useConfig();
}

void MainWindow::addConfig()
{
    QString name(ui->addName->text());
    QString filePath(QFileDialog::getOpenFileName(this,
        tr("Select a config file"), QDir::homePath(), tr("Text files (*.ini)")));

    if (name == "")
    {
        QStringList part = filePath.split("/").last().split(".");
        part.pop_back();
        name = part.join("");
    }

    config.addConfig(QDir(filePath), name);
    reloadButtons();
}

void MainWindow::deleteConfig()
{
    if (selectedConfig == "" || !config.getWaypaperConfigs().contains(selectedConfig))
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
    if (selectedConfig == "" || !config.getWaypaperConfigs().contains(selectedConfig))
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

void MainWindow::openPreferenceDialogue()
{
    Preference *widget = new Preference(config);
    widget->setModal(true);
    widget->show();
}

void MainWindow::setWindowSystem(int index)
{
    config.setWindowSystem(static_cast<WindowSystem>(index));
}
