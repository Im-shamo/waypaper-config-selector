qt_add_executable(waypaper-config-selector
    mainwindow.ui mainwindow.cpp
    preference.ui preference.cpp
    config.cpp
    utils.cpp
    main.cpp
)

target_link_libraries(waypaper-config-selector PRIVATE Qt6::Widgets)
