#include <QGuiApplication>
#include <QDebug>
#include <QQmlError>
#include <QQmlApplicationEngine>
#include <QUrl>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("bgengine-preview"));
    QGuiApplication::setOrganizationName(QStringLiteral("brtcrt"));

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral(BGENGINE_BUILD_IMPORT_PATH));
    QObject::connect(&engine, &QQmlApplicationEngine::warnings, &app, [](const QList<QQmlError> &warnings) {
        for (const QQmlError &warning : warnings) {
            qWarning().noquote() << warning.toString();
        }
    });
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, [] {
        QCoreApplication::exit(1);
    }, Qt::QueuedConnection);

    engine.load(QUrl(QStringLiteral("qrc:/BgEnginePreview/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qCritical() << "Failed to load BgEnginePreview.Main";
        return 1;
    }

    return app.exec();
}
