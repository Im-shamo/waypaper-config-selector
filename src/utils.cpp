#include "utils.h"
#include <QProcessEnvironment>

WindowSystem currentWindowSystem()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if (env.value("WAYLAND_DISPLAY", "") != "")
    {
        return WindowSystem::wayland;
    }
    else
    {
        return WindowSystem::xorg;
    }
}
