import QtQuick
import QtMultimedia

Item {
    id: root

    property string sourceMode: "shader"
    property url imageSource: ""
    property url videoSource: ""
    property int fillMode: Image.PreserveAspectCrop
    property bool paused: false

    Image {
        anchors.fill: parent
        visible: root.sourceMode === "image" || root.sourceMode === "composite"
        source: root.imageSource
        fillMode: root.fillMode
        asynchronous: true
        cache: true
        smooth: true
    }

    MediaPlayer {
        id: player
        source: root.videoSource
        videoOutput: videoOutput
        loops: MediaPlayer.Infinite
        audioOutput: AudioOutput {
            muted: true
        }
        onSourceChanged: {
            if (source !== "" && !root.paused) {
                play()
            }
        }
    }

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        visible: root.sourceMode === "video" || root.sourceMode === "composite"
        fillMode: VideoOutput.PreserveAspectCrop
    }

    onPausedChanged: {
        if (root.paused) {
            player.pause()
        } else if (player.source !== "") {
            player.play()
        }
    }
}
