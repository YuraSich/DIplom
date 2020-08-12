#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "cpu_emu.h"

int main(int argc, char *argv[])
{

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setOrganizationName("yury PI61");
    app.setOrganizationDomain("yurysich");
    app.setApplicationName("WLIVEmu");
    qmlRegisterType<CPU>("com.cpmpany.cpu",1,0,"CPU_WLIV_EMU");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);


    return app.exec();
}
