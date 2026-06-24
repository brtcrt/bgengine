import QtQuick
import BgEngine

ShaderEffect {
    id: root

    required property BgEngineController engine

    property variant sourceTexture
    property real iTime: engine.time
    property int iFrame: engine.frame
    property vector3d iResolution: Qt.vector3d(Math.max(width, 1), Math.max(height, 1), 1)
    property vector4d iMouse: engine.iMouse
    property real effectOpacity: 1.0

    vertexShader: "qrc:/bgengine/shaders/fullscreen.vert.qsb"
    fragmentShader: "qrc:/bgengine/shaders/shadertoy.frag.qsb"
}
