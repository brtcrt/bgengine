import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import BgEngine
import BgEnginePreview

ApplicationWindow {
    id: window

    readonly property url defaultVertexShader: "qrc:/bgengine/shaders/fullscreen.vert.qsb"
    readonly property url defaultFragmentShader: "qrc:/bgengine/shaders/shadertoy.frag.qsb"
    readonly property url defaultPostProcessShader: "qrc:/bgengine/shaders/postprocess.frag.qsb"

    property url imageSource: ""
    property url videoSource: ""
    property url vertexShaderSource: defaultVertexShader
    property url fragmentShaderSource: defaultFragmentShader
    property url postProcessShaderSource: defaultPostProcessShader
    property string statusMessage: ""

    width: 1280
    height: 720
    visible: true
    title: "bgengine preview"
    color: "#111318"

    function setComboValue(combo, value) {
        for (let i = 0; i < combo.model.length; ++i) {
            if (combo.model[i].value === value) {
                combo.currentIndex = i
                return
            }
        }
    }

    function lowerUrl(url) {
        return String(url).toLowerCase()
    }

    function importFile(url) {
        const text = lowerUrl(url)
        if (text.endsWith(".vert") || text.endsWith(".frag")) {
            const compiled = applier.compileShader(url)
            if (compiled.toString().length === 0) {
                return
            }
            url = compiled
        }

        const shaderText = lowerUrl(url)
        if (shaderText.endsWith(".vert.qsb")) {
            vertexShaderSource = url
            setComboValue(shaderModeBox, "standard")
            setComboValue(sourceModeBox, "shader")
            statusMessage = "Imported vertex shader"
            return
        }

        if (shaderText.endsWith(".frag.qsb") || shaderText.endsWith(".qsb")) {
            fragmentShaderSource = url
            setComboValue(sourceModeBox, "shader")
            statusMessage = "Imported fragment shader"
            return
        }

        if (text.match(/\.(png|jpg|jpeg|webp|bmp|gif|avif|heif|svg|svgz|tif|tiff)$/)) {
            imageSource = url
            setComboValue(sourceModeBox, "image")
            statusMessage = "Imported image"
            return
        }

        if (text.match(/\.(mp4|webm|mkv|mov|m4v|avi)$/)) {
            videoSource = url
            setComboValue(sourceModeBox, "video")
            statusMessage = "Imported video"
            return
        }

        statusMessage = "Unsupported file. Use image, video, or .qsb shader files."
    }

    BgEngineView {
        id: background
        anchors.fill: parent
        sourceMode: sourceModeBox.currentValue
        shaderMode: shaderModeBox.currentValue
        imageSource: window.imageSource
        videoSource: window.videoSource
        vertexShader: window.vertexShaderSource
        fragmentShader: window.fragmentShaderSource
        postProcessShader: window.postProcessShaderSource
        fpsCap: fpsCap.value
        postProcessEnabled: postProcess.checked
        mouseUniforms: mouseUniforms.checked
        effectOpacity: opacitySlider.value
    }

    PreviewApplier {
        id: applier
        onLastMessageChanged: window.statusMessage = lastMessage
    }

    FileDialog {
        id: importDialog
        title: "Import media or shader"
        fileMode: FileDialog.OpenFile
        nameFilters: [
            "Supported files (*.png *.jpg *.jpeg *.webp *.bmp *.gif *.avif *.heif *.svg *.svgz *.tif *.tiff *.mp4 *.webm *.mkv *.mov *.m4v *.avi *.qsb *.frag *.vert)",
            "Images (*.png *.jpg *.jpeg *.webp *.bmp *.gif *.avif *.heif *.svg *.svgz *.tif *.tiff)",
            "Videos (*.mp4 *.webm *.mkv *.mov *.m4v *.avi)",
            "Qt shader binaries (*.qsb)",
            "GLSL shader sources (*.frag *.vert)"
        ]
        onAccepted: window.importFile(selectedFile)
    }

    Pane {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 16
        width: 360

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            RowLayout {
                Layout.fillWidth: true

                Button {
                    Layout.fillWidth: true
                    text: "Import"
                    onClicked: importDialog.open()
                }

                Button {
                    Layout.fillWidth: true
                    text: "Apply"
                    onClicked: applier.applyToPlasma(
                        sourceModeBox.currentValue,
                        shaderModeBox.currentValue,
                        window.imageSource,
                        window.videoSource,
                        window.vertexShaderSource,
                        window.fragmentShaderSource,
                        window.postProcessShaderSource,
                        fpsCap.value,
                        postProcess.checked,
                        mouseUniforms.checked,
                        opacitySlider.value,
                        background.fillMode)
                }
            }

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

            CheckBox {
                id: mouseUniforms
                checked: true
                text: "Mouse uniforms"
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

            Label {
                Layout.fillWidth: true
                text: window.statusMessage
                visible: text.length > 0
                wrapMode: Text.Wrap
                color: applier.lastApplySucceeded ? "#8fda75" : "#e8eaed"
            }
        }
    }
}
