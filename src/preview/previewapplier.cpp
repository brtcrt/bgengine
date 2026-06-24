#include "previewapplier.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringList>

namespace
{
#ifndef BGENGINE_PLUGIN_ID
#define BGENGINE_PLUGIN_ID "org.brtcrt.bgengine"
#endif
constexpr auto pluginId = BGENGINE_PLUGIN_ID;
}

PreviewApplier::PreviewApplier(QObject *parent)
    : QObject(parent)
{
}

QString PreviewApplier::lastMessage() const
{
    return m_lastMessage;
}

bool PreviewApplier::lastApplySucceeded() const
{
    return m_lastApplySucceeded;
}

bool PreviewApplier::applyToPlasma(const QString &sourceMode,
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
                                   int fillMode)
{
    // Find the Plasma Shell D-Bus interface
    QDBusInterface plasma(QStringLiteral("org.kde.plasmashell"),
                          QStringLiteral("/PlasmaShell"),
                          QStringLiteral("org.kde.PlasmaShell"),
                          QDBusConnection::sessionBus());

    if (!plasma.isValid()) {
        setResult(false, QStringLiteral("Plasma Shell is not available on the session bus."));
        return false;
    }

    // Write config to KDE plasma desktops via js eval
    const QStringList writes = {
        QStringLiteral("d.writeConfig(\"SourceMode\", %1);").arg(jsString(sourceMode)),
        QStringLiteral("d.writeConfig(\"ShaderMode\", %1);").arg(jsString(shaderMode)),
        QStringLiteral("d.writeConfig(\"ImageSource\", %1);").arg(jsString(urlString(imageSource))),
        QStringLiteral("d.writeConfig(\"VideoSource\", %1);").arg(jsString(urlString(videoSource))),
        QStringLiteral("d.writeConfig(\"VertexShader\", %1);").arg(jsString(urlString(vertexShader))),
        QStringLiteral("d.writeConfig(\"FragmentShader\", %1);").arg(jsString(urlString(fragmentShader))),
        QStringLiteral("d.writeConfig(\"PostProcessShader\", %1);").arg(jsString(urlString(postProcessShader))),
        QStringLiteral("d.writeConfig(\"FpsCap\", %1);").arg(qBound(1, fpsCap, 240)),
        QStringLiteral("d.writeConfig(\"PostProcessEnabled\", %1);").arg(jsBool(postProcessEnabled)),
        QStringLiteral("d.writeConfig(\"MouseUniforms\", %1);").arg(jsBool(mouseUniforms)),
        QStringLiteral("d.writeConfig(\"EffectOpacity\", %1);").arg(jsNumber(qBound(0.0, effectOpacity, 1.0))),
        QStringLiteral("d.writeConfig(\"FillMode\", %1);").arg(fillMode),
    };

    QString script;
    script += QStringLiteral("var pluginId = %1;\n").arg(jsString(QString::fromLatin1(pluginId)));
    script += QStringLiteral("var desktops = desktops();\n");
    script += QStringLiteral("for (var i = 0; i < desktops.length; ++i) {\n");
    script += QStringLiteral("  var d = desktops[i];\n");
    script += QStringLiteral("  d.wallpaperPlugin = pluginId;\n");
    script += QStringLiteral("  d.currentConfigGroup = Array(\"Wallpaper\", pluginId, \"General\");\n");
    for (const QString &write : writes) {
        script += QStringLiteral("  ") + write + u'\n';
    }
    script += QStringLiteral("}\n");

    const QDBusMessage reply = plasma.call(QStringLiteral("evaluateScript"), script);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        setResult(false, reply.errorMessage().isEmpty() ? reply.errorName() : reply.errorMessage());
        return false;
    }

    setResult(true, QStringLiteral("Applied to Plasma desktops."));
    return true;
}

QUrl PreviewApplier::compileShader(const QUrl &source)
{
    if (!source.isLocalFile()) {
        setResult(false, QStringLiteral("Only local shader source files can be compiled."));
        return {};
    }

    const QFileInfo sourceInfo(source.toLocalFile());
    if (!sourceInfo.exists() || !sourceInfo.isFile()) {
        setResult(false, QStringLiteral("Shader source file does not exist."));
        return {};
    }

    const QString suffix = sourceInfo.suffix().toLower();
    if (suffix != QLatin1String("frag") && suffix != QLatin1String("vert")) {
        setResult(false, QStringLiteral("Shader source must use .frag or .vert."));
        return {};
    }

    // Qt Shader baker to compile the shaders into .qsb
    QString qsb = QStandardPaths::findExecutable(QStringLiteral("qsb-qt6"));
    if (qsb.isEmpty()) {
        qsb = QStandardPaths::findExecutable(QStringLiteral("qsb"));
    }
    if (qsb.isEmpty() && QFileInfo::exists(QStringLiteral("/usr/lib64/qt6/bin/qsb"))) {
        qsb = QStringLiteral("/usr/lib64/qt6/bin/qsb");
    }
    if (qsb.isEmpty()) {
        setResult(false, QStringLiteral("Qt Shader Baker was not found."));
        return {};
    }

    // Write compiled shader to cache
    const QString cacheRoot = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir cacheDir(cacheRoot.isEmpty() ? QDir::tempPath() : cacheRoot);
    if (!cacheDir.mkpath(QStringLiteral("compiled-shaders")) || !cacheDir.cd(QStringLiteral("compiled-shaders"))) {
        setResult(false, QStringLiteral("Could not create shader cache directory."));
        return {};
    }

    QString inputPath = sourceInfo.absoluteFilePath();
    QString outputStem = sourceInfo.fileName();

    if (suffix == QLatin1String("frag")) {
        QFile sourceFile(sourceInfo.absoluteFilePath());
        if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            setResult(false, QStringLiteral("Could not read shader source."));
            return {};
        }

        const QString sourceText = QString::fromUtf8(sourceFile.readAll());
        if (looksLikeShadertoyFragment(sourceText)) {
            QByteArray hashInput = sourceInfo.absoluteFilePath().toUtf8();
            hashInput += sourceText.toUtf8();
            const QByteArray hash = QCryptographicHash::hash(hashInput, QCryptographicHash::Sha1).toHex().left(12);
            outputStem = sourceInfo.completeBaseName() + u'-' + QString::fromLatin1(hash) + QStringLiteral(".frag");
            inputPath = cacheDir.filePath(outputStem);

            QFile convertedFile(inputPath);
            if (!convertedFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                setResult(false, QStringLiteral("Could not write converted shader source."));
                return {};
            }
            convertedFile.write(convertShadertoyFragment(sourceText).toUtf8());
            convertedFile.close();
        }
    }

    const QString outputPath = cacheDir.filePath(outputStem + QStringLiteral(".qsb"));
    QStringList args;
    args << QStringLiteral("--qt6");
    if (suffix == QLatin1String("vert")) {
        args << QStringLiteral("--batchable");
    }
    args << QStringLiteral("--output") << outputPath << inputPath;

    // Begin compile
    QProcess process;
    process.start(qsb, args);
    if (!process.waitForStarted()) {
        setResult(false, QStringLiteral("Could not start Qt Shader Baker."));
        return {};
    }
    if (!process.waitForFinished(30000)) {
        process.kill();
        process.waitForFinished();
        setResult(false, QStringLiteral("Shader compilation timed out."));
        return {};
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        const QString error = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
        setResult(false, error.isEmpty() ? QStringLiteral("Shader compilation failed.") : error);
        return {};
    }

    setResult(true, QStringLiteral("Compiled shader to %1.").arg(outputPath));
    return QUrl::fromLocalFile(outputPath);
}

void PreviewApplier::setResult(bool succeeded, const QString &message)
{
    if (m_lastApplySucceeded != succeeded) {
        m_lastApplySucceeded = succeeded;
        Q_EMIT lastApplySucceededChanged();
    }

    if (m_lastMessage != message) {
        m_lastMessage = message;
        Q_EMIT lastMessageChanged();
    }
}
// We need the functions below to convert to js since we are using js eval to send the config to plasma.
QString PreviewApplier::jsString(const QString &value)
{
    QString escaped = value;
    escaped.replace(u'\\', QStringLiteral("\\\\"));
    escaped.replace(u'"', QStringLiteral("\\\""));
    escaped.replace(u'\n', QStringLiteral("\\n"));
    escaped.replace(u'\r', QStringLiteral("\\r"));
    return u'"' + escaped + u'"';
}

QString PreviewApplier::jsBool(bool value)
{
    return value ? QStringLiteral("true") : QStringLiteral("false");
}

QString PreviewApplier::jsNumber(double value)
{
    return QLocale::c().toString(value, 'g', 12);
}

QString PreviewApplier::urlString(const QUrl &url)
{
    return url.isEmpty() ? QString() : url.toString();
}

QString PreviewApplier::convertShadertoyFragment(const QString &source)
{
    QString body = source;
    body.replace(QRegularExpression(QStringLiteral("^\\s*#version[^\\n]*(\\n|$)"),
                                    QRegularExpression::MultilineOption),
                 QString());
    body.replace(QRegularExpression(QStringLiteral("^\\s*precision\\s+(lowp|mediump|highp)\\s+\\w+\\s*;\\s*(\\n|$)"),
                                    QRegularExpression::MultilineOption),
                 QString());
    body.replace(QRegularExpression(QStringLiteral("^\\s*uniform\\s+[^;]*(iResolution|iTime|iFrame|iMouse|iChannel[0-3])\\s*;\\s*(\\n|$)"),
                                    QRegularExpression::MultilineOption),
                 QString());

    // This seems so retarded but idk what else to do
    QString header = QStringLiteral(
        "#version 440\n\n"
        "layout(location = 0) in vec2 qt_TexCoord0;\n"
        "layout(location = 0) out vec4 fragColor;\n\n");

    for (int channel = 0; channel < 4; ++channel) {
        const QString name = QStringLiteral("iChannel%1").arg(channel);
        if (body.contains(name)) {
            header += QStringLiteral("layout(binding = %1) uniform sampler2D %2;\n").arg(channel + 1).arg(name);
        }
    }
    // Same here. I'm thinking maybe I should have read up more on how these work
    header += QStringLiteral(
        "\n"
        "layout(std140, binding = 0) uniform buf {\n"
        "    mat4 qt_Matrix;\n"
        "    float qt_Opacity;\n"
        "    float iTime;\n"
        "    int iFrame;\n"
        "    vec3 iResolution;\n"
        "    vec4 iMouse;\n"
        "    float effectOpacity;\n"
        "} ubuf;\n\n"
        "#define iTime ubuf.iTime\n"
        "#define iFrame ubuf.iFrame\n"
        "#define iResolution ubuf.iResolution\n"
        "#define iMouse ubuf.iMouse\n\n");

    QString footer;
    if (!body.contains(QRegularExpression(QStringLiteral("\\bvoid\\s+main\\s*\\(")))) {
        footer = QStringLiteral(
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);\n"
            "    vec2 fragCoord = vec2(qt_TexCoord0.x, 1.0 - qt_TexCoord0.y) * iResolution.xy;\n"
            "    mainImage(color, fragCoord);\n"
            "    fragColor = vec4(color.rgb, color.a * ubuf.qt_Opacity * ubuf.effectOpacity);\n"
            "}\n");
    }

    return header + body + footer;
}

// This is a very dumb check to see if the shader source looks like shadertoy. If it ain't broke, don't fix it i guess.
bool PreviewApplier::looksLikeShadertoyFragment(const QString &source)
{
    return source.contains(QRegularExpression(QStringLiteral("\\bmainImage\\s*\\(")))
        || source.contains(QStringLiteral("iResolution"))
        || source.contains(QStringLiteral("iTime"))
        || source.contains(QStringLiteral("iMouse"))
        || source.contains(QStringLiteral("iChannel0"));
}
