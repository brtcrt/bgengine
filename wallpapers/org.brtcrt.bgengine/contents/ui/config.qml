import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.FormLayout {
    id: root

    twinFormLayouts: parentLayout

    property string cfg_SourceMode: "shader"
    property string cfg_ShaderMode: "shadertoy"
    property alias cfg_ImageSource: imageSource.text
    property alias cfg_VideoSource: videoSource.text
    property alias cfg_FpsCap: fpsCap.value
    property alias cfg_PostProcessEnabled: postProcess.checked
    property alias cfg_MouseUniforms: mouseUniforms.checked
    property alias cfg_EffectOpacity: opacity.value
    property int cfg_FillMode: 2

    function indexFor(model, value) {
        for (let i = 0; i < model.length; ++i) {
            if (model[i].value === value) {
                return i
            }
        }
        return 0
    }

    QQC2.ComboBox {
        id: sourceMode
        Kirigami.FormData.label: i18n("Source:")
        textRole: "label"
        valueRole: "value"
        model: [
            { label: i18n("Shader"), value: "shader" },
            { label: i18n("Image"), value: "image" },
            { label: i18n("Video"), value: "video" },
            { label: i18n("Composite"), value: "composite" }
        ]
        onActivated: root.cfg_SourceMode = currentValue
        Component.onCompleted: currentIndex = root.indexFor(model, root.cfg_SourceMode)
    }

    QQC2.ComboBox {
        id: shaderMode
        Kirigami.FormData.label: i18n("Shader mode:")
        textRole: "label"
        valueRole: "value"
        model: [
            { label: i18n("Shadertoy"), value: "shadertoy" },
            { label: i18n("Vertex/fragment"), value: "standard" }
        ]
        onActivated: root.cfg_ShaderMode = currentValue
        Component.onCompleted: currentIndex = root.indexFor(model, root.cfg_ShaderMode)
    }

    QQC2.TextField {
        id: imageSource
        Kirigami.FormData.label: i18n("Image URL:")
        placeholderText: i18n("file:///path/to/image.png")
    }

    QQC2.TextField {
        id: videoSource
        Kirigami.FormData.label: i18n("Video URL:")
        placeholderText: i18n("file:///path/to/video.mp4")
    }

    QQC2.SpinBox {
        id: fpsCap
        Kirigami.FormData.label: i18n("FPS cap:")
        from: 1
        to: 240
    }

    QQC2.CheckBox {
        id: postProcess
        text: i18n("Post-process")
    }

    QQC2.CheckBox {
        id: mouseUniforms
        text: i18n("Mouse uniforms")
    }

    QQC2.Slider {
        id: opacity
        Kirigami.FormData.label: i18n("Opacity:")
        from: 0
        to: 1
        stepSize: 0.01
    }
}
