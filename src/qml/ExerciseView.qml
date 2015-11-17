import QtQuick 2.5

Item {
    id: exerciseView

    property Item minuetMenu
    property string chosenExercise;
    
    Timer {
        id: timer
        interval: 4000; running: false; repeat: false
        onTriggered: {
            messageText.text = qsTr("Hear the interval and then choose an answer from options below!<br/>Click 'play' if you want to hear again!")
            chosenExercise = exerciseController.randomlyChooseExercise()
            exerciseController.playChoosenExercise()
        }
    }

    function clearExerciseGrid() {
        exerciseView.visible = false
        for (var i = 0; i < answerGrid.children.length; ++i)
            answerGrid.children[i].destroy()
    }
    function itemChanged(model) {
        exerciseView.visible = false
        for (var i = 0; i < answerGrid.children.length; ++i)
            answerGrid.children[i].destroy()
        chosenExercise = exerciseController.randomlyChooseExercise()
        var length = model.length
        answerGrid.columns = Math.min(5, length)
        answerGrid.rows = Math.ceil(length/5)
        var colors = ["#8dd3c7", "#ffffb3", "#bebada", "#fb8072", "#80b1d3", "#fdb462", "#b3de69", "#fccde5", "#d9d9d9", "#bc80bd", "#ccebc5", "#ffed6f"]
        for (var i = 0; i < length; ++i)
            answerOption.createObject(answerGrid, {text: model[i].name, color: colors[i%12]})
        exerciseView.visible = true;
    }

    visible: false

    Column {
        anchors.centerIn: parent
        spacing: 20
        Text {
            id: messageText

            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 20
            textFormat: Text.RichText
            text: qsTr("Hear the interval and then choose an answer from options below!<br/>Click 'play' if you want to hear again!")
        }
        Row {
            anchors { horizontalCenter: parent.horizontalCenter }
            spacing: 20
            Rectangle {
                width: 120; height: 40; color: "white"; border.color: "black"; border.width: 2; radius: 5
                Text { anchors.centerIn: parent; color: "black"; text: qsTr("play") }
                MouseArea { anchors.fill: parent; onClicked: exerciseController.playChoosenExercise() }
            }
            Rectangle {
                width: 120; height: 40; color: "gray"; border.color: "black"; border.width: 2; radius: 5
                Text { anchors.centerIn: parent; color: "white"; text: qsTr("give up") }
                MouseArea { anchors.fill: parent; onClicked: { chosenExercise = exerciseController.randomlyChooseExercise(); exerciseController.playChoosenExercise() } }
            }
        }
        Rectangle {
            width: answerGrid.columns*140; height: answerGrid.rows*60
            color: "#475057"
            radius: 5
            anchors { horizontalCenter: parent.horizontalCenter }
            Grid {
                id: answerGrid

                anchors.centerIn: parent
                spacing: 20; columns: 2; rows: 1
                Component {
                    id: answerOption
                    Rectangle {
                        property alias text: option.text
                        width: 120; height: 40; border.color: "white"; border.width: 2; radius: 5
                        Text { id: option; anchors.centerIn: parent; width: parent.width; horizontalAlignment: Qt.AlignHCenter; color: "black"; wrapMode: Text.Wrap }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (option.text == chosenExercise) {
                                    messageText.text = "Congratulations!<br/>You answered correctly!"
                                } else {
                                    messageText.text = "Ops, not this time!<br/>Try again!"
                                }
                                timer.start()
                            }
                        }
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        minuetMenu.onBackspacePressed.connect(clearExerciseGrid)
        minuetMenu.onItemChanged.connect(itemChanged)
    }
}