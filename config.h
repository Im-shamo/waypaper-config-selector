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

    QDir getCurrentConfigPath() const;
    QDir getConfigPath(const QString& name) const;

    void setAutoChangeBackend(bool set);
    bool getAutoChangeBackend() const;

    WindowSystem getWindowSystem() const;
    void setWindowSystem(WindowSystem sys);

    const QStringList& getWindowSystemList() const;
    const QStringList& getWaylandBackends() const;
    const QStringList& getXorgBackends() const;

    void setXorgBackendPreference(const QString& backend);
    const QString& getXorgBackendPreference() const;

    void setWaylandBackendPreference(const QString& backend);
    const QString& getWaylandBackendPreference() const;

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
    QStringList windowSystemList;
    WindowSystem windowSystem;
    bool autoChangeBackend;

    WindowSystem getWindowSystemFromSettings() const;
};
#endif // CONFIG_H
