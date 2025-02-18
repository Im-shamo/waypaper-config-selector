#include "mainwindow.h"
#include "config.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Config config;

    // Parse command line arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("Waypaper-Config-Selector");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption changeConfig({"c", "change"}, "change config.", "name");
    QCommandLineOption listConfig({"l", "list"}, "list configs");

    parser.addOption(changeConfig);
    parser.addOption(listConfig);
    parser.process(app);

    if (parser.isSet("change"))
    {
        QString name = parser.value("change");
        if (config.getAutoChangeBackend())
            switch (config.getWindowSystem())
            {
            case WindowSystem::xorg:
                config.changeConfigSettings(name, "Settings/backend", config.getXorgBackendPreference());
                break;

            case WindowSystem::wayland:
                config.changeConfigSettings(name, "Settings/backend", config.getWaylandBackendPreference());
                break;
        }
        config.setCurrentConfig(name);
        return EXIT_SUCCESS;
    }

    else if (parser.isSet("list"))
    {
        QTextStream cout(stdout);
        int index = 0;
        int lastIndex = config.getWaypaperConfigs().size() - 1;
        for (const QString &name : config.getWaypaperConfigs())
        {
            cout << name;
            if (index < lastIndex)
            {
                cout << ", ";
            }
            index++;
        }
        cout << "\n";
        return EXIT_SUCCESS;
    }

    else
    {
        MainWindow w(config, 5);
        w.show();
        return app.exec();
    }
}
