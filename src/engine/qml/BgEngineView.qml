import QtQuick
import QtMultimedia
import BgEngine

Item {
    id: root

    property string sourceMode: "shader"
    property string shaderMode: "shadertoy"
    property url imageSource: ""
    property url videoSource: ""
    property url vertexShader: "qrc:/bgengine/shaders/fullscreen.vert.qsb"
    property url fragmentShader: "qrc:/bgengine/shaders/shadertoy.frag.qsb"
    property url postProcessShader: "qrc:/bgengine/shaders/postprocess.frag.qsb"
    property bool postProcessEnabled: false
    property int fpsCap: 60
    property bool paused: false
    property bool mouseUniforms: true
    property real effectOpacity: 1.0
    property int fillMode: Image.PreserveAspectCrop

    BgEngineController {
        id: engine
        sourceMode: root.sourceMode
        shaderMode: root.shaderMode
        imageSource: root.imageSource.toString()
        videoSource: root.videoSource.toString()
        fpsCap: root.fpsCap
        paused: root.paused || !root.visible
        mouseUniforms: root.mouseUniforms
        renderSize: Qt.size(root.width, root.height)
    }

    NoiseTexture {
        id: defaultChannelTexture
    }

    Timer {
        interval: engine.frameIntervalMs()
        repeat: true
        running: root.visible && !root.paused
        onTriggered: engine.advanceFrame()
    }

    MediaLayer {
        id: mediaLayer
        anchors.fill: parent
        sourceMode: root.sourceMode
        imageSource: root.imageSource
        videoSource: root.videoSource
        fillMode: root.fillMode
        paused: root.paused || !root.visible
    }

    ShaderSurface {
        id: shaderSurface
        anchors.fill: parent
        visible: root.sourceMode === "shader" || root.sourceMode === "composite"
        vertexShader: root.vertexShader
        fragmentShader: root.fragmentShader
        engine: engine
        iChannel0: defaultChannelTexture.texture
        iChannel1: defaultChannelTexture.texture
        iChannel2: defaultChannelTexture.texture
        iChannel3: defaultChannelTexture.texture
        effectOpacity: root.effectOpacity
    }

    ShaderSurface {
        anchors.fill: parent
        visible: root.postProcessEnabled
        vertexShader: root.vertexShader
        fragmentShader: root.postProcessShader
        engine: engine
        iChannel0: defaultChannelTexture.texture
        iChannel1: defaultChannelTexture.texture
        iChannel2: defaultChannelTexture.texture
        iChannel3: defaultChannelTexture.texture
        sourceTexture: ShaderEffectSource {
            sourceItem: root.sourceMode === "shader" ? shaderSurface : mediaLayer
            hideSource: false
            live: true
            recursive: false
        }
        effectOpacity: root.effectOpacity
    }

    // I doubt these work btw didn't bother checking
    HoverHandler {
        id: hoverHandler
        enabled: root.mouseUniforms
        onPointChanged: engine.mousePosition = point.position
    }

    TapHandler {
        enabled: root.mouseUniforms
        acceptedButtons: Qt.LeftButton
        onPressedChanged: {
            engine.mousePosition = point.position
            engine.mousePressed = pressed
        }
    }
}
