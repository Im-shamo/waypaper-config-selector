#ifndef UTILS_H
#define UTILS_H

enum WindowSystem
{
    xorg,
    wayland
};

WindowSystem currentWindowSystem();
#endif // UTILS_H
