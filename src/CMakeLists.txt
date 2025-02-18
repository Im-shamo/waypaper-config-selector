qt_add_executable(waypaper-config-selector
    mainwindow.h mainwindow.ui mainwindow.cpp
    preference.h preference.ui preference.cpp
    config.h config.cpp
    utils.h utils.cpp
    main.cpp
)

target_link_libraries(waypaper-config-selector PRIVATE Qt6::Widgets)
