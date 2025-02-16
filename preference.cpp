#include "preference.h"
#include "ui_preference.h"

Preference::Preference(Config &config, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Preference)
    , config(config)
{
    ui->setupUi(this);
    loadComboBox();
}

Preference::~Preference()
{
    delete ui;
}

void Preference::loadComboBox()
{
    ui->xorgBackendComboBox->addItems(config.getXorgBackends());
    ui->waylandBackendComboBox->addItems(config.getWaylandBackends());

    ui->xorgBackendComboBox->setCurrentText(config.getXorgBackendPreference());
    ui->waylandBackendComboBox->setCurrentText(config.getWaylandBackendPreference());
    ui->autoBackendCheckBox->setChecked(config.getAutoChangeBackend());
}

void Preference::saveConfig(int result)
{
    if (result == DialogCode::Accepted)
    {
        config.setXorgBackendPreference(ui->xorgBackendComboBox->currentText());
        config.setWaylandBackendPreference(ui->waylandBackendComboBox->currentText());
        config.setAutoChangeBackend(ui->autoBackendCheckBox->isChecked());
    }
}
