#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QDir>
#include <QSettings>
#include <QStringList>
#include "utils.h"

class Config
{
public:
    Config();
    void scanDir();

    const QStringList& getWaypaperConfigs();
    const QString& getCurrentConfig();
    QDir getCurrentConfigPath();
    QDir getConfigPath(const QString& name);
    void setAutoChangeBackend(bool set);
    bool getAutoChangeBackend();
    WindowSystem getWindowSystem();
    QStringList& getWaylandBackends();
    QStringList& getXorgBackends();


    void setCurrentConfig(const QString& name);
    void renameConfig(const QString& name, const QString& newName);
    void deleteConfig(const QString& name);
    void addConfig(const QDir& filePath, const QString& name);

private:
    QSettings settings;
    QDir configFolderPath;
    QDir waypaperConfigsFolderPath;
    QDir waypaperConfigFilePath;
    QString currentConfig;
    QStringList waypaperConfigs;
    QStringList xorgBackends;
    QStringList waylandBackends;
    QString xorgBackendPreference;
    QString waylandBackendPreference;
    WindowSystem windowSystem;
    bool autoChangeBackend;
};
#endif // CONFIG_H
