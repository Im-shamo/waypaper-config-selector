#include "config.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>

Config::Config()
    : settings("Im-shamo", "waypaper-config-selector")
    , configFolderPath(QDir::homePath().append("/.config/waypaper-config-selector"))
    , waypaperConfigsFolderPath(QDir::homePath().append("/.config/waypaper-config-selector/waypaper_configs"))
    , waypaperConfigFilePath(QDir::homePath().append("/.config/waypaper/config.ini"))
    , currentConfig(settings.value("current").toString())
    , xorgBackends{"feh", "mpvpaper", "wallutils"}
    , waylandBackends{"hyprpaper", "swaybg", "swww", "wallutils"}
    , xorgBackendPreference(settings.value("xorg_backend_preference").toString())
    , waylandBackendPreference(settings.value("wayland_backend_preference").toString())
    , windowSystemList{"Xorg", "Wayland"}
    , windowSystem(getWindowSystemFromSettings())
    , autoChangeWindowSystem(settings.value("auto_change_window_system", false).toBool())
    , autoChangeBackend(settings.value("auto_change_backend", false).toBool())
{
    scanDir();
    if (autoChangeWindowSystem)
    {
        setWindowSystem(currentWindowSystem());
    }
    qDebug() << "---- config ----";
    qDebug() << "Config Folder: " << configFolderPath.absolutePath();
    qDebug() << "waypaper_configs foler:" << waypaperConfigsFolderPath.absolutePath();
    qDebug() << "waypaper config: " << waypaperConfigFilePath.absolutePath();
    qDebug() << "configs:: " << waypaperConfigs;
    qDebug() << "Window System: " << windowSystemList[windowSystem];
    qDebug() << "Xorg backend preference: " << xorgBackendPreference;
    qDebug() << "Xorg backends: " << xorgBackends;
    qDebug() << "Wayland backend preference: " << waylandBackendPreference;
    qDebug() << "Wayland backends: " << waylandBackends;
}

void Config::scanDir()
{
    qDebug() << "---- scanDir ----";
    waypaperConfigs.clear();
    QStringList filenames = waypaperConfigsFolderPath.entryList(
        QDir::Files | QDir::NoDot | QDir::Readable | QDir::Writable);

    for (QString &filename : filenames)
    {
        QStringList parts = filename.split(".");
        if (parts.last().contains("ini", Qt::CaseInsensitive))
        {
            parts.pop_back();
            waypaperConfigs.push_back(parts.join("."));
        }
    }
}

const QStringList &Config::getWaypaperConfigs()
{
    qDebug() << "---- getWaypaperConfigs ----";
    return waypaperConfigs;
}

const QString &Config::getCurrentConfig()
{
    qDebug() << "---- getCurrentConfig ----";
    return currentConfig;
}

QDir Config::getCurrentConfigPath() const
{
    qDebug() << "---- getCurrentConfigPath ----";
    return QDir(
        waypaperConfigsFolderPath
            .absolutePath()
            .append("/")
            .append(currentConfig)
            .append(".ini"));
}

QDir Config::getConfigPath(const QString &name) const
{
    qDebug() << "---- getConfigPath ----";
    return QDir(
        waypaperConfigsFolderPath
            .absolutePath()
            .append("/")
            .append(name)
            .append(".ini"));
}

void Config::setCurrentConfig(const QString &name)
{
    qDebug() << "---- setCurrentConfig ----";
    if(!waypaperConfigs.contains(name))
    {
        qDebug() << name << "does not exists!";
        return;
    }

    currentConfig = name;
    settings.setValue("current", name);

    QFile waypaperConfigFile(waypaperConfigFilePath.absolutePath());
    QFileInfo waypaperConfigFileInfo(waypaperConfigFile);
    QFile currentConfigFile(getCurrentConfigPath().absolutePath());
    QFileInfo currentConfigFileInfo(currentConfigFile);

    if (!currentConfigFileInfo.exists())
    {
        qDebug() << "Error " << getCurrentConfigPath().absolutePath() << "does not exists!";
        return;
    }

    if (waypaperConfigFileInfo.isSymLink())
    {
        if (waypaperConfigFile.remove())
        {
            qDebug() << "Removed " << waypaperConfigFilePath.absolutePath();
        }
        else
        {
            qDebug() << "Failed to remove " << waypaperConfigFilePath.absolutePath();
            return;
        }
    }

    else if (waypaperConfigFileInfo.isFile())
    {
        QString filename(waypaperConfigFileInfo.fileName().append(".bak"));
        if (waypaperConfigFile.rename(filename))
        {
            qDebug() << "Renamed " << waypaperConfigFileInfo.fileName() << " To " << filename;
        }
        else
        {
            qDebug() << "Failed to rename" << waypaperConfigFileInfo.fileName();
            return;
        }
    }

    qDebug() << "Linked " << getCurrentConfigPath().absolutePath() << " To " << waypaperConfigFilePath.absolutePath();
    currentConfigFile.link(waypaperConfigFilePath.absolutePath());
}

void Config::renameConfig(const QString &name, const QString &newName)
{
    qDebug() << "---- renameConfig ----";
    if(!waypaperConfigs.contains(name))
    {
        qDebug() << name << "does not exists!";
        return;
    }

    if(currentConfig == name)
    {
        qDebug() << "Cannot rename in use";
        return;
    }

    QString oldFilePath(getConfigPath(name).absolutePath());
    QString newFilePath(waypaperConfigsFolderPath.absolutePath() + "/" + newName + ".ini");

    QFile configFile(oldFilePath);

    if (configFile.rename(newFilePath))
    {
        qDebug() << "Renamed " << oldFilePath << " To " << newFilePath;
    }
    else
    {
        qDebug() << "Failed to rename " << oldFilePath;
        return;
    }

    scanDir();
}

void Config::deleteConfig(const QString &name)
{
    qDebug() << "---- deleteConfig ----";
    if(!waypaperConfigs.contains(name))
    {
        qDebug() << name << "does not exists!";
        return;
    }

    if (currentConfig == name)
    {
        qDebug() << "Cannot delete currenly using config!";
        return;
    }

    QString filePath(getConfigPath(name).absolutePath());

    QFile configFile(filePath);

    if (configFile.remove())
    {
        qDebug() << "Deleted " << filePath;
    }
    else
    {
        qDebug() << "Cannot delete " << filePath;
        return;
    }

    scanDir();
}

void Config::addConfig(const QDir &filePath, const QString &name)
{
    qDebug() << "---- addConfig ----";
    QFile file(filePath.absolutePath());
    QFileInfo fileInfo(file);

    if (!fileInfo.isFile())
    {
        qDebug() << filePath << " is not a file!";
        return;
    }

    QDir configPath(waypaperConfigsFolderPath.absolutePath() + "/" + name + ".ini");

    if (file.copy(configPath.absolutePath()))
    {
        qDebug() << "Copied " << filePath.absolutePath() << " To " << configPath.absolutePath();
    }
    else
    {
        qDebug() << "Failed to copy " << filePath.absolutePath() << " To " << configPath.absolutePath();
    }
    scanDir();
}

void Config::addConfig(const QDir &filePath)
{
    qDebug() << "---- addConfig ----";
    QFile file(filePath.absolutePath());
    QFileInfo fileInfo(file);

    if (!fileInfo.isFile())
    {
        qDebug() << filePath << " is not a file!";
        return;
    }

    QDir configPath(waypaperConfigsFolderPath.absolutePath().append("/").append(fileInfo.fileName()));
    if (file.copy(configPath.absolutePath()))
    {
        qDebug() << "Copied " << filePath.absolutePath() << " To " << configPath.absolutePath();
    }
    else
    {
        qDebug() << "Failed to copy " << filePath.absolutePath() << " To " << configPath.absolutePath();
    }
    scanDir();
}

void Config::setAutoChangeWindowSystem(bool set)
{
    qDebug() << "---- setAutoChangeWindowSystem ----";
    autoChangeWindowSystem = set;
    settings.setValue("auto_change_window_system", set);
    qDebug() << "set auto_change_window_system to " << set;
}

bool Config::getAutoChangeWindowSystem() const
{
    qDebug() << "---- getAutoChangeWindowSystem ----";
    return autoChangeWindowSystem;
}

const QStringList &Config::getXorgBackends() const
{
    qDebug() << "---- getXorgBackends ----";
    return xorgBackends;
}

const QStringList &Config::getWaylandBackends() const
{
    return waylandBackends;
}

void Config::setXorgBackendPreference(const QString &backend)
{
    qDebug() << "---- setXorgBackendPreference ----";
    if (xorgBackends.contains(backend))
    {
        qDebug() << "Set xorg backend preference to " << backend;
        xorgBackendPreference = backend;
        settings.setValue("xorg_backend_preference", backend);
    }
    else
    {
        qDebug() << backend << " does not exists!";
    }
}

const QString &Config::getXorgBackendPreference() const
{
    qDebug() << "---- getWindowSystem ----";
    return xorgBackendPreference;
}

void Config::setWaylandBackendPreference(const QString &backend)
{
    qDebug() << "---- setWaylandBackendPreference ----";
    if (waylandBackends.contains(backend))
    {
        qDebug() << "Set wayland backend preference to " << backend;
        waylandBackendPreference = backend;
        settings.setValue("wayland_backend_preference", backend);
    }
    else
    {
        qDebug() << backend << " does not exists!";
    }
}

const QString &Config::getWaylandBackendPreference() const
{
    qDebug() << "---- getWindowSystem ----";
    return waylandBackendPreference;
}

WindowSystem Config::getWindowSystem() const
{
    qDebug() << "---- getWindowSystem ----";
    return windowSystem;
}

const QStringList &Config::getWindowSystemList() const
{
    qDebug() << "---- getWindowSystemList ----";
    return windowSystemList;
}

WindowSystem Config::getWindowSystemFromSettings() const
{
    qDebug() << "---- getWindowSystemFromSettings ----";
    QString sys = settings.value("window_system").toString();
    qDebug() << "window_system: " << sys;
    if (sys == windowSystemList[1])
    {
        return WindowSystem::wayland;
    }
    else
    {
        return WindowSystem::xorg;
    }
}

void Config::setWindowSystem(WindowSystem sys)
{
    qDebug() << "---- setWindowSystem ----";
    settings.setValue("window_system", windowSystemList[sys]);
    windowSystem = sys;
    qDebug() << "Set window_system to " << windowSystemList[sys];
}

void Config::changeConfigSettings(const QString &name, const QString &key, const QVariant &value) const
{
    qDebug() << "---- getConfigSettings ----";
    if (!waypaperConfigs.contains(name))
    {
        qDebug() << name << "does not exists";
        return;
    }
    qDebug() << "Set " << name << " setting " << key << " to " << value;
    QSettings configSettings(getConfigPath(name).absolutePath(), QSettings::Format::IniFormat);
    configSettings.setValue(key, value);
}

QVariant Config::getConfigSettings(const QString &name, const QString &key, const QVariant &defaultValue)
{
    qDebug() << "---- getConfigSettings ----";
    if (!waypaperConfigs.contains(name))
    {
        qDebug() << name << "does not exists";
        return defaultValue;
    }
    QSettings configSettings(getConfigPath(name).absolutePath(), QSettings::Format::IniFormat);
    return configSettings.value(key, defaultValue);
}

void Config::setAutoChangeBackend(bool set)
{
    qDebug() << "---- setAutoChangeBackend ----";
    autoChangeBackend = set;
    settings.setValue("auto_change_backend", set);
    qDebug() << "set auto_change_backend to " << set;
}

bool Config::getAutoChangeBackend() const
{
    qDebug() << "---- getAutoChangeBackend ----";
    return autoChangeBackend;
}
