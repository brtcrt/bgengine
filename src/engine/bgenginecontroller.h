#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <QPointF>
#include <QSizeF>
#include <QString>
#include <QVector4D>
#include <QtQmlIntegration/qqmlintegration.h>

class BgEngineController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString sourceMode READ sourceMode WRITE setSourceMode NOTIFY sourceModeChanged FINAL)
    Q_PROPERTY(QString shaderMode READ shaderMode WRITE setShaderMode NOTIFY shaderModeChanged FINAL)
    Q_PROPERTY(QString imageSource READ imageSource WRITE setImageSource NOTIFY imageSourceChanged FINAL)
    Q_PROPERTY(QString videoSource READ videoSource WRITE setVideoSource NOTIFY videoSourceChanged FINAL)
    Q_PROPERTY(int fpsCap READ fpsCap WRITE setFpsCap NOTIFY fpsCapChanged FINAL)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged FINAL)
    Q_PROPERTY(bool mouseUniforms READ mouseUniforms WRITE setMouseUniforms NOTIFY mouseUniformsChanged FINAL)
    Q_PROPERTY(QSizeF renderSize READ renderSize WRITE setRenderSize NOTIFY renderSizeChanged FINAL)
    Q_PROPERTY(QPointF mousePosition READ mousePosition WRITE setMousePosition NOTIFY mousePositionChanged FINAL)
    Q_PROPERTY(bool mousePressed READ mousePressed WRITE setMousePressed NOTIFY mousePressedChanged FINAL)
    Q_PROPERTY(qreal time READ time NOTIFY frameTick FINAL)
    Q_PROPERTY(int frame READ frame NOTIFY frameTick FINAL)
    Q_PROPERTY(QVector4D iMouse READ iMouse NOTIFY mouseUniformChanged FINAL)

public:
    explicit BgEngineController(QObject *parent = nullptr);

    QString sourceMode() const;
    void setSourceMode(const QString &sourceMode);

    QString shaderMode() const;
    void setShaderMode(const QString &shaderMode);

    QString imageSource() const;
    void setImageSource(const QString &imageSource);

    QString videoSource() const;
    void setVideoSource(const QString &videoSource);

    int fpsCap() const;
    void setFpsCap(int fpsCap);

    bool isPaused() const;
    void setPaused(bool paused);

    bool mouseUniforms() const;
    void setMouseUniforms(bool mouseUniforms);

    QSizeF renderSize() const;
    void setRenderSize(const QSizeF &renderSize);

    QPointF mousePosition() const;
    void setMousePosition(const QPointF &mousePosition);

    bool mousePressed() const;
    void setMousePressed(bool mousePressed);

    qreal time() const;
    int frame() const;
    QVector4D iMouse() const;

    Q_INVOKABLE int frameIntervalMs() const;
    Q_INVOKABLE void advanceFrame();
    Q_INVOKABLE void resetClock();

Q_SIGNALS:
    void sourceModeChanged();
    void shaderModeChanged();
    void imageSourceChanged();
    void videoSourceChanged();
    void fpsCapChanged();
    void pausedChanged();
    void mouseUniformsChanged();
    void renderSizeChanged();
    void mousePositionChanged();
    void mousePressedChanged();
    void mouseUniformChanged();
    void frameTick();

private:
    QString m_sourceMode = QStringLiteral("shader");
    QString m_shaderMode = QStringLiteral("shadertoy");
    QString m_imageSource;
    QString m_videoSource;
    int m_fpsCap = 60;
    bool m_paused = false;
    bool m_mouseUniforms = true;
    QSizeF m_renderSize;
    QPointF m_mousePosition;
    QPointF m_lastPressedPosition;
    bool m_mousePressed = false;
    QElapsedTimer m_clock;
    qreal m_pausedAt = 0.0;
    int m_frame = 0;
};
