import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import BgEngine

ApplicationWindow {
    id: window

    width: 1280
    height: 720
    visible: true
    title: "bgengine preview"
    color: "#111318"

    BgEngineView {
        id: background
        anchors.fill: parent
        sourceMode: sourceModeBox.currentValue
        shaderMode: shaderModeBox.currentValue
        fpsCap: fpsCap.value
        postProcessEnabled: postProcess.checked
        effectOpacity: opacitySlider.value
    }

    Pane {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 16
        width: 320

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            ComboBox {
                id: sourceModeBox
                Layout.fillWidth: true
                textRole: "label"
                valueRole: "value"
                model: [
                    { label: "Shader", value: "shader" },
                    { label: "Image", value: "image" },
                    { label: "Video", value: "video" },
                    { label: "Composite", value: "composite" }
                ]
            }

            ComboBox {
                id: shaderModeBox
                Layout.fillWidth: true
                textRole: "label"
                valueRole: "value"
                model: [
                    { label: "Shadertoy", value: "shadertoy" },
                    { label: "Vertex/Fragment", value: "standard" }
                ]
            }

            RowLayout {
                Layout.fillWidth: true

                Label {
                    text: "FPS"
                }

                SpinBox {
                    id: fpsCap
                    from: 1
                    to: 240
                    value: 60
                }
            }

            CheckBox {
                id: postProcess
                text: "Post-process"
            }

            RowLayout {
                Layout.fillWidth: true

                Label {
                    text: "Opacity"
                }

                Slider {
                    id: opacitySlider
                    Layout.fillWidth: true
                    from: 0
                    to: 1
                    value: 1
                }
            }
        }
    }
}
