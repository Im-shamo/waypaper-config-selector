#include "config.h"
#include "QFile"
#include "QFileInfo"

Config::Config()
    : settings("Im-Shamo", "Waypaper-config-selector")
    , configFolderPath(QDir::homePath().append("/.config/waypaper-config-selector/waypaper_configs"))
    , waypaperConfigsFolderPath(QDir::homePath().append("/.config/waypaper-config-selector/waypaper_configs"))
    , waypaperConfigFilePath(QDir::homePath().append("/.config/waypaper/config.ini"))
    , currentConfig(settings.value("current").toString())
{
    scanDir();
    qDebug() << configFolderPath;
    qDebug() << waypaperConfigsFolderPath;
    qDebug() << waypaperConfigFilePath;
    qDebug() << waypaperConfigs;
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

QDir Config::getCurrentConfigPath()
{
    return QDir(
        waypaperConfigsFolderPath
            .absolutePath()
            .append("/")
            .append(currentConfig)
            .append(".ini"));
}

QDir Config::getConfigPath(const QString& name)
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
            qDebug() << "Failed to remove " << waypaperConfigFilePath.absolutePath();
        }
        else
        {
            qDebug() << "Removed " << waypaperConfigFilePath.absolutePath();
            return;
        }
    }

    else if (waypaperConfigFileInfo.isFile())
    {
        QString filename(waypaperConfigFileInfo.fileName().append(".bak"));
        if (waypaperConfigFile.rename(filename))
        { qDebug() << "Renamed " << waypaperConfigFileInfo.fileName() << " To " << filename;
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
        qDebug() << "Cannot deletee currenly using config!";
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
