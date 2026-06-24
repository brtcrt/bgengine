import QtQuick

Item {
    id: root

    width: 256
    height: 256

    property alias texture: textureSource

    Canvas {
        id: noiseCanvas
        width: root.width
        height: root.height
        visible: true

        onPaint: {
            const ctx = getContext("2d")
            const image = ctx.createImageData(width, height)
            for (let y = 0; y < height; ++y) {
                for (let x = 0; x < width; ++x) {
                    const i = (y * width + x) * 4
                    let n = x * 374761393 + y * 668265263
                    n = (n ^ (n >> 13)) * 1274126177
                    n = (n ^ (n >> 16)) & 255
                    image.data[i] = n
                    image.data[i + 1] = (n * 73 + x) & 255
                    image.data[i + 2] = (n * 151 + y) & 255
                    image.data[i + 3] = 255
                }
            }
            ctx.putImageData(image, 0, 0)
        }

        Component.onCompleted: requestPaint()
    }

    ShaderEffectSource {
        id: textureSource
        sourceItem: noiseCanvas
        hideSource: true
        live: false
        textureSize: Qt.size(root.width, root.height)
        wrapMode: ShaderEffectSource.Repeat
    }
}
