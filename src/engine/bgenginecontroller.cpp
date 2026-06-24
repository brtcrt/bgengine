#include "bgenginecontroller.h"

#include <QtMath>

BgEngineController::BgEngineController(QObject *parent)
    : QObject(parent)
{
    m_clock.start();
}

QString BgEngineController::sourceMode() const
{
    return m_sourceMode;
}

void BgEngineController::setSourceMode(const QString &sourceMode)
{
    if (m_sourceMode == sourceMode) {
        return;
    }

    m_sourceMode = sourceMode;
    Q_EMIT sourceModeChanged();
}

QString BgEngineController::shaderMode() const
{
    return m_shaderMode;
}

void BgEngineController::setShaderMode(const QString &shaderMode)
{
    if (m_shaderMode == shaderMode) {
        return;
    }

    m_shaderMode = shaderMode;
    Q_EMIT shaderModeChanged();
}

QString BgEngineController::imageSource() const
{
    return m_imageSource;
}

void BgEngineController::setImageSource(const QString &imageSource)
{
    if (m_imageSource == imageSource) {
        return;
    }

    m_imageSource = imageSource;
    Q_EMIT imageSourceChanged();
}

QString BgEngineController::videoSource() const
{
    return m_videoSource;
}

void BgEngineController::setVideoSource(const QString &videoSource)
{
    if (m_videoSource == videoSource) {
        return;
    }

    m_videoSource = videoSource;
    Q_EMIT videoSourceChanged();
}

int BgEngineController::fpsCap() const
{
    return m_fpsCap;
}

void BgEngineController::setFpsCap(int fpsCap)
{
    const int normalized = qBound(1, fpsCap, 240);
    if (m_fpsCap == normalized) {
        return;
    }

    m_fpsCap = normalized;
    Q_EMIT fpsCapChanged();
}

bool BgEngineController::isPaused() const
{
    return m_paused;
}

void BgEngineController::setPaused(bool paused)
{
    if (m_paused == paused) {
        return;
    }

    if (paused) {
        m_pausedAt = time();
    } else {
        const qreal resumeAt = m_pausedAt;
        m_clock.restart();
        m_pausedAt = resumeAt;
    }

    m_paused = paused;
    Q_EMIT pausedChanged();
    Q_EMIT frameTick();
}

bool BgEngineController::mouseUniforms() const
{
    return m_mouseUniforms;
}

void BgEngineController::setMouseUniforms(bool mouseUniforms)
{
    if (m_mouseUniforms == mouseUniforms) {
        return;
    }

    m_mouseUniforms = mouseUniforms;
    Q_EMIT mouseUniformsChanged();
    Q_EMIT mouseUniformChanged();
}

QSizeF BgEngineController::renderSize() const
{
    return m_renderSize;
}

void BgEngineController::setRenderSize(const QSizeF &renderSize)
{
    if (m_renderSize == renderSize) {
        return;
    }

    m_renderSize = renderSize;
    Q_EMIT renderSizeChanged();
}

QPointF BgEngineController::mousePosition() const
{
    return m_mousePosition;
}

void BgEngineController::setMousePosition(const QPointF &mousePosition)
{
    if (m_mousePosition == mousePosition) {
        return;
    }

    m_mousePosition = mousePosition;
    Q_EMIT mousePositionChanged();
    Q_EMIT mouseUniformChanged();
}

bool BgEngineController::mousePressed() const
{
    return m_mousePressed;
}

void BgEngineController::setMousePressed(bool mousePressed)
{
    if (m_mousePressed == mousePressed) {
        return;
    }

    m_mousePressed = mousePressed;
    if (mousePressed) {
        m_lastPressedPosition = m_mousePosition;
    }
    Q_EMIT mousePressedChanged();
    Q_EMIT mouseUniformChanged();
}

qreal BgEngineController::time() const
{
    if (m_paused) {
        return m_pausedAt;
    }

    return m_pausedAt + (static_cast<qreal>(m_clock.elapsed()) / 1000.0);
}

int BgEngineController::frame() const
{
    return m_frame;
}

QVector4D BgEngineController::iMouse() const
{
    if (!m_mouseUniforms) {
        return {};
    }

    const qreal y = qMax<qreal>(0.0, m_renderSize.height() - m_mousePosition.y());
    const qreal pressY = qMax<qreal>(0.0, m_renderSize.height() - m_lastPressedPosition.y());
    const qreal sign = m_mousePressed ? 1.0 : -1.0;

    return QVector4D(
        static_cast<float>(m_mousePosition.x()),
        static_cast<float>(y),
        static_cast<float>(m_lastPressedPosition.x() * sign),
        static_cast<float>(pressY * sign));
}

int BgEngineController::frameIntervalMs() const
{
    return qMax(1, qRound(1000.0 / static_cast<qreal>(m_fpsCap)));
}

void BgEngineController::advanceFrame()
{
    if (m_paused) {
        return;
    }

    ++m_frame;
    Q_EMIT frameTick();
}

void BgEngineController::resetClock()
{
    m_clock.restart();
    m_pausedAt = 0.0;
    m_frame = 0;
    Q_EMIT frameTick();
}
