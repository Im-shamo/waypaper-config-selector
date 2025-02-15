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
}
