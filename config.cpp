#include "config.h"
#include "QFile"
#include "QFileInfo"

Config::Config()
    : settings("Im-Shamo", "Waypaper-config-selector")
    , configFolderPath(QDir::homePath().append("/.config/waypaper-config-selector"))
    , waypaperConfigsFolderPath(QDir::homePath().append("/.config/waypaper-config-selector/waypaper_configs"))
    , waypaperConfigFilePath(QDir::homePath().append("/.config/waypaper/config.ini"))
    , currentConfig(settings.value("current").toString())
    , autoChangeBackend(settings.value("auto_change_backend").toBool())
    , windowSystemList{"Xorg", "Wayland"}
    , windowSystem(getWindowSystemFromSettings())
    , xorgBackends{"feh", "mpvpaper", "wallutils"}
    , waylandBackends{"hyprpaper", "swaybg", "swww", "wallutils"}
    , xorgBackendPreference(settings.value("xorg_backend_preference").toString())
    , waylandBackendPreference(settings.value("wayland_backend_preference").toString())
{
    scanDir();
    if (autoChangeBackend)
    {
        setWindowSystem(currentWindowSystem());
    }
    qDebug() << configFolderPath.absolutePath();
    qDebug() << waypaperConfigsFolderPath.absolutePath();
    qDebug() << waypaperConfigFilePath.absolutePath();
    qDebug() << waypaperConfigs;
    qDebug() << "Window System: " << windowSystem;
    qDebug() << "Xorg backend preference: " << xorgBackendPreference;
    qDebug() << "Xorg backends: " << xorgBackends;
    qDebug() << "Wayland backend preference: " << waylandBackendPreference;
    qDebug() << "Wayland backends: " << waylandBackends;
}

void Config::scanDir()
{
    waypaperConfigs.clear();
    QStringList filenames = waypaperConfigsFolderPath.entryList(
        QDir::Files | QDir::NoDot | QDir::Readable | QDir::Writable);

    for (QString &filename : filenames)
    {
        QStringList parts = filename.split(".");
        if (parts.back().contains("ini", Qt::CaseInsensitive))
        {
            parts.pop_back();
            waypaperConfigs.push_back(parts.join(""));
        }
    }
}

const QStringList& Config::getWaypaperConfigs()
{
    return waypaperConfigs;
}

const QString& Config::getCurrentConfig()
{
    return currentConfig;
}

QDir Config::getCurrentConfigPath() const
{
    return QDir(
        waypaperConfigsFolderPath
            .absolutePath()
            .append("/") .append(currentConfig)
            .append(".ini"));
}

QDir Config::getConfigPath(const QString& name) const
{
    return QDir(
        waypaperConfigsFolderPath
            .absolutePath()
            .append("/")
            .append(name)
            .append(".ini"));
}

void Config::setCurrentConfig(const QString& name)
{
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
        }
    }

    qDebug() << "Linked " << getCurrentConfigPath().absolutePath() << " To " << waypaperConfigFilePath.absolutePath();
    currentConfigFile.link(waypaperConfigFilePath.absolutePath());
}

void Config::renameConfig(const QString& name, const QString& newName)
{
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

void Config::deleteConfig(const QString& name)
{
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

void Config::addConfig(const QDir& filePath, const QString& name)
{
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

void Config::setAutoChangeBackend(bool set)
{
    autoChangeBackend = set;
    settings.setValue("auto_change_backend", set);
    qDebug() << "set auto_change_backend to " << set;
}

bool Config::getAutoChangeBackend() const
{
    return autoChangeBackend;
}

const QStringList& Config::getXorgBackends() const
{
    return xorgBackends;
}

const QStringList& Config::getWaylandBackends() const
{
    return waylandBackends;
}

void Config::setXorgBackendPreference(const QString& backend)
{
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

const QString& Config::getXorgBackendPreference() const
{
    return xorgBackendPreference;
}

void Config::setWaylandBackendPreference(const QString& backend)
{
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

const QString& Config::getWaylandBackendPreference() const
{
    return waylandBackendPreference;
}

WindowSystem Config::getWindowSystem() const
{
    return windowSystem;
}

const QStringList& Config::getWindowSystemList() const
{
    return windowSystemList;
}

WindowSystem Config::getWindowSystemFromSettings() const
{
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
    settings.setValue("window_system", windowSystemList[sys]);
    windowSystem = sys;
    qDebug() << "Set window_system to " << windowSystemList[sys];
}
