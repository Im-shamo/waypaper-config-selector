#ifndef PREFERENCE_H
#define PREFERENCE_H

#include <QDialog>
#include "config.h"

namespace Ui {
class Preference;
}

class Preference : public QDialog
{
    Q_OBJECT

public:
    explicit Preference(Config &config, QWidget *parent = nullptr);
    ~Preference();
private slots:
    void saveConfig(int result);

private:
    Ui::Preference *ui;
    Config& config;

    void loadComboBox();
};

#endif // PREFERENCE_H
