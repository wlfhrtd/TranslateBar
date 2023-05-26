import QtQuick
import QtQuick.Window
import QtQuick.Controls 6.3
import QtQuick.Layouts 6.3

import com.application.languageguesser 1.0
import com.application.translater 1.0
import com.application.settingsmanager 1.0


Window {
    id: root
    flags: Qt.Window | Qt.WindowTitleHint | Qt.WindowSystemMenuHint | Qt.WindowCloseButtonHint

    width: 278
    height: 44
    visible: true
    title: qsTr("TranslateBar")

    property bool is_running: false

    function lockUI() {
        is_running = true

        btnSubmit.enabled = false
    }
    function unlockUI() {
        is_running = false

        btnSubmit.enabled = true
    }
    function saveAppSettings() {
        settingsManager.saveSettings(
                    Qt.application.name,
                    "Settings",
                    {
                        "WindowX" : root.x,
                        "WindowY" : root.y,
                        "WindowWidth" : root.width,
                        "WindowHeight" : root.height,
                        "AlwaysOnTop" : alwaysOnTop.checked ? 1 : 0 // win32 registry has no boolean type; |0 or ^0 should be slower
                    })
    }
    //                                 => error                                          => error
    // app flow: ui => LanguageGuesser => result => Translater {Network => error/result} => result
    Component.onCompleted: {
        btnSubmit.clicked.connect(btnSubmit.submit)

        languageGuesser.resultReady.connect(languageGuesser.passInputToTranslater)
        languageGuesser.errorOccurred.connect(languageGuesser.stopOnError)

        translater.translationReady.connect(translater.showResult)
        translater.errorOccurred.connect(translater.stopOnError)

        Qt.application.aboutToQuit.connect(saveAppSettings)

        let settings = settingsManager.loadSettings(Qt.application.name, "Settings")
        root.x = settings["WindowX"]
        root.y = settings["WindowY"]
        root.width = settings["WindowWidth"]
        root.height = settings["WindowHeight"]
        alwaysOnTop.checked = settings["AlwaysOnTop"]
    }

    LanguageGuesser {
        id: languageGuesser

        function passInputToTranslater() {
            btnSubmit.text = languageGuesser.result.toLocaleUpperCase()

            translater.doTranslation(languageGuesser.result, languageGuesser.input)
        }

        function stopOnError() {
            popupMsg.messageText = languageGuesser.errorMessage
            popupMsg.open()

            unlockUI()
        }
    }

    Popup {
        id: popupMsg

        property alias messageText: innerText.text

        anchors.centerIn: parent

        focus: true
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        Column {
            spacing: 10

            Text {
                id: innerText
            }

            Button {
                id: popupOkButton

                width: popupMsg.width * 0.33

                text: qsTr("OK")
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: popupMsg.close()
            }
        }
    }

    Translater {
        id: translater

        function showResult() {
            textInput.text = translater.translation
            textInput.focus = true
            textInput.selectAll()

            unlockUI()
        }

        function stopOnError() {
            popupMsg.messageText = translater.errorMessage
            popupMsg.open()

            unlockUI()
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: mouse => {
                       if(mouse.button === Qt.RightButton) {
                           contextMenu.popup()
                       }
                   }

        onPressAndHold: mouse => {
                            if (mouse.source === Qt.MouseEventNotSynthesized) {
                                contextMenu.popup()
                            }
                        }

        Menu {
            id: contextMenu

            MenuItem {
                id: alwaysOnTop

                checkable: true
                text: "Always on top"
                onCheckedChanged: {
                    if(!checked) {
                        root.flags ^= Qt.WindowStaysOnTopHint

                        return;
                    }

                    root.flags |= Qt.WindowStaysOnTopHint
                }
            }
        }
    }

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        spacing: 6

        TextInput {
            id: textInput

            height: parent.height
            Layout.fillWidth: true
            Layout.leftMargin: 5

            text: ""
            font.pixelSize: root.height * 0.5
            font.capitalization: Font.AllLowercase
            horizontalAlignment: TextInput.AlignLeft

            focus: true
        }

        Button {
            id: btnSubmit

            implicitWidth: root.width * 0.2 + 20
            implicitHeight: parent.height

            font.pixelSize: 36

            text: qsTr("N/A")

            function submit() {
                lockUI()

                languageGuesser.doGuess(textInput.text)
            }
        }

        Keys.onEnterPressed: {
            btnSubmit.clicked()
        }

        Keys.onReturnPressed: {
            btnSubmit.clicked()
        }
    }

    SettingsManager {
        id: settingsManager
    }
}
