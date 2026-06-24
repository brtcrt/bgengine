#pragma once

#include <QObject>
#include <QUrl>
#include <QtQmlIntegration/qqmlintegration.h>

class PreviewApplier : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString lastMessage READ lastMessage NOTIFY lastMessageChanged FINAL)
    Q_PROPERTY(bool lastApplySucceeded READ lastApplySucceeded NOTIFY lastApplySucceededChanged FINAL)

public:
    explicit PreviewApplier(QObject *parent = nullptr);

    QString lastMessage() const;
    bool lastApplySucceeded() const;

    Q_INVOKABLE bool applyToPlasma(const QString &sourceMode,
                                   const QString &shaderMode,
                                   const QUrl &imageSource,
                                   const QUrl &videoSource,
                                   const QUrl &vertexShader,
                                   const QUrl &fragmentShader,
                                   const QUrl &postProcessShader,
                                   int fpsCap,
                                   bool postProcessEnabled,
                                   bool mouseUniforms,
                                   double effectOpacity,
                                   int fillMode);
    Q_INVOKABLE QUrl compileShader(const QUrl &source);

Q_SIGNALS:
    void lastMessageChanged();
    void lastApplySucceededChanged();

private:
    void setResult(bool succeeded, const QString &message);
    static QString jsString(const QString &value);
    static QString jsBool(bool value);
    static QString jsNumber(double value);
    static QString urlString(const QUrl &url);
    static QString convertShadertoyFragment(const QString &source);
    static bool looksLikeShadertoyFragment(const QString &source);

    QString m_lastMessage;
    bool m_lastApplySucceeded = false;
};
