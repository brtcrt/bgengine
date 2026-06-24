import QtQuick
import org.kde.plasma.plasmoid
import BgEngine

WallpaperItem {
    id: root

    BgEngineView {
        anchors.fill: parent
        sourceMode: root.configuration.SourceMode
        shaderMode: root.configuration.ShaderMode
        imageSource: root.configuration.ImageSource
        videoSource: root.configuration.VideoSource
        vertexShader: root.configuration.VertexShader
        fragmentShader: root.configuration.FragmentShader
        postProcessShader: root.configuration.PostProcessShader
        fpsCap: root.configuration.FpsCap
        postProcessEnabled: root.configuration.PostProcessEnabled
        mouseUniforms: root.configuration.MouseUniforms
        effectOpacity: root.configuration.EffectOpacity
        fillMode: root.configuration.FillMode
    }
}
