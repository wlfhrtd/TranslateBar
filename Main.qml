import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import com.application.inputvalidator 1.0
import com.application.translater 1.0
import com.application.settingsmanager 1.0

import "qrc:///"


Window {
    id: root
    flags: Qt.Window | Qt.WindowTitleHint | Qt.WindowSystemMenuHint | Qt.WindowCloseButtonHint

    width: 278
    height: 44
    visible: true
    title: "TranslateBar"

    property bool is_running: false

    function lockUI() {
        is_running = true

        btnSubmit.enabled = false
    }
    function unlockUI() {
        is_running = false

        btnSubmit.enabled = true
    }
    function switchInputMethod() {
        let currentLanguageFromIndexTemporary = dialogSettings.languageFromIndex
        dialogSettings.languageFromIndex = dialogSettings.languageToIndex
        dialogSettings.languageToIndex = currentLanguageFromIndexTemporary
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
                        "AlwaysOnTop" : checkBoxAlwaysOnTop.checked ? 1 : 0, // win32 registry has no boolean type; |0 or ^0 should be slower
                        "LanguageFromIndex" : comboBoxLanguageFrom.currentIndex,
                        "LanguageToIndex" : comboBoxLanguageTo.currentIndex
                    })
    }
    //                                 => error                                          => error
    //  app flow: ui => InputValidator => result => Translater {Network => error/result} => result
    Component.onCompleted: {
        Qt.inputMethod.localeChanged.connect(switchInputMethod)

        btnSubmit.clicked.connect(btnSubmit.submit)

        inputValidator.resultReady.connect(inputValidator.passInputToTranslater)
        inputValidator.errorOccurred.connect(inputValidator.stopOnError)

        translater.translationReady.connect(translater.showResult)
        translater.errorOccurred.connect(translater.stopOnError)

        Qt.application.aboutToQuit.connect(saveAppSettings)

        let settings = settingsManager.loadSettings(Qt.application.name, "Settings")
        root.x = settings["WindowX"]
        root.y = settings["WindowY"]
        root.width = settings["WindowWidth"]
        root.height = settings["WindowHeight"]
        checkBoxAlwaysOnTop.checked = settings["AlwaysOnTop"]
        comboBoxLanguageFrom.currentIndex = settings["LanguageFromIndex"]
        comboBoxLanguageTo.currentIndex  = settings["LanguageToIndex"]
    }

    InputValidator {
        id: inputValidator

        function passInputToTranslater() {
            translater.doTranslation(
                        listModelLanguages.get(dialogSettings.languageFromIndex).value,
                        listModelLanguages.get(dialogSettings.languageToIndex).value,
                        inputValidator.result
                        )
        }

        function stopOnError() {
            popupMsg.messageText = inputValidator.errorMessage
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

                onClicked: {
                    popupMsg.close()
                    textInput.focus = true
                    textInput.selectAll()
                }
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

    Dialog {        
        id: dialogSettings
        title: qsTr("Settings")
        standardButtons: Dialog.Ok | Dialog.Cancel
        modal: true
        focus: true

        anchors.centerIn: parent
        width: root.width / 2
        height: root.height / 2
        parent: Overlay.overlay
        closePolicy: Popup.CloseOnEscape

        property int languageFromIndex: 0
        property int languageToIndex: 0
        property bool alwaysOnTop: false

        Component.onCompleted: {
            dialogSettings.languageFromIndex = comboBoxLanguageFrom.currentIndex
            dialogSettings.languageToIndex = comboBoxLanguageTo.currentIndex
            dialogSettings.alwaysOnTop = checkBoxAlwaysOnTop.checked
        }

        onAccepted: {
            dialogSettings.languageFromIndex = comboBoxLanguageFrom.currentIndex
            dialogSettings.languageToIndex = comboBoxLanguageTo.currentIndex
            dialogSettings.alwaysOnTop = checkBoxAlwaysOnTop.checked
        }

        onRejected: {
            comboBoxLanguageFrom.currentIndex = dialogSettings.languageFromIndex
            comboBoxLanguageTo.currentIndex = dialogSettings.languageToIndex
            checkBoxAlwaysOnTop.checked = dialogSettings.alwaysOnTop
        }

        onAlwaysOnTopChanged: {
            if(!dialogSettings.alwaysOnTop) {
                root.flags ^= Qt.WindowStaysOnTopHint

                return;
            }

            root.flags |= Qt.WindowStaysOnTopHint
        }

        ColumnLayout {
            RowLayout {
                Label {
                    text: "FROM"
                }

                ComboBox {
                    id: comboBoxLanguageFrom
                    textRole: "text"
                    valueRole: "value"

                    model: listModelLanguages
                }

                Label {
                    text: "TO"
                }

                ComboBox {
                    id: comboBoxLanguageTo
                    textRole: "text"
                    valueRole: "value"
                    currentIndex: 0
                    model: listModelLanguages
                }
            }

            CheckBox {
                id: checkBoxAlwaysOnTop
                text: qsTr("Always On Top")
            }

            RowLayout {
                Label {
                    text: qsTr("Current window position: ")
                }

                Text {
                    text: root.x
                }

                Text {
                    text: root.y
                }
            }

            RowLayout {
                Label {
                    text: qsTr("Current window size: ")
                }

                Text {
                    text: root.width
                }

                Text {
                    text: root.height
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

            MouseArea {
                anchors.fill: textInput
                acceptedButtons: Qt.RightButton

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
                        text: "Settings"

                        onClicked: {
                            dialogSettings.open()
                        }
                    }
                }
            }
        }

        Button {
            id: btnSubmit

            implicitWidth: root.width * 0.2 + 20
            implicitHeight: parent.height

            font.pixelSize: 36

            text: listModelLanguages.get(dialogSettings.languageFromIndex).value.toUpperCase() + " >> " + listModelLanguages.get(dialogSettings.languageToIndex).value.toUpperCase()

            function submit() {
                lockUI()

                inputValidator.validate(textInput.text)
            }

            MouseArea {
                anchors.fill: btnSubmit
                acceptedButtons: Qt.RightButton

                onClicked: mouse => {
                               if(mouse.button === Qt.RightButton) {
                                   switchInputMethod()
                               }
                           }

                onPressAndHold: mouse => {
                                    if (mouse.source === Qt.MouseEventNotSynthesized) {
                                        switchInputMethod()
                                    }
                                }
            }
        }

        Keys.onEnterPressed: {
            btnSubmit.clicked()
        }

        Keys.onReturnPressed: {
            btnSubmit.clicked()
        }
    }

    LanguagesListModel {
        id: listModelLanguages
    }

    SettingsManager {
        id: settingsManager
    }
}
