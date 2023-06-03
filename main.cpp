#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "inputvalidator.h"
#include "translater.h"
#include "settingsmanager.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<InputValidator>("com.application.inputvalidator", 1, 0, "InputValidator");
    qmlRegisterType<Translater>("com.application.translater", 1, 0, "Translater");
    qmlRegisterType<SettingsManager>("com.application.settingsmanager", 1, 0, "SettingsManager");

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("TranslateBar", "Main");

    return app.exec();
}
