import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import Qt.labs.qmlmodels 1.0
import QtQuick.Dialogs 1.3
import com.cpmpany.cpu 1.0





Window {
    id: mainWindow
    visible: true
    width: 1240
    height: 490
    title: "WLIV cpu emulator"
    Component.onCompleted: view.update()

    Item {
        id: view
        property var file
        property var editorpath: "C:/Program Files (x86)/Notepad++/notepad++.exe"
        property bool opened: false
        property bool exectn: false
        property bool running: false
        property var speed: 1000

        function update(){

            regA.text = ""
            regB.text = ""
            regC.text = ""
            regD.text = ""
            regSP.text = ""
            regIP.text = ""
            regFO.text = ""
            regFP.text = ""
            regFZ.text = ""
            regR0.text = ""
            regR1.text = ""
            regR2.text = ""
            regR3.text = ""
            regR4.text = ""
            regR5.text = ""
            memory.text = ""
            desasm.text = ""
            cls.enabled = false
            reload.enabled = false
            exec.enabled = false
            ex.enabled = false
            autoEx.enabled = false
            if(opened == true){
                cls.enabled = true
                reload.enabled = true
                openInEditor.enabled = true
                exec.enabled = true
                regA.text = cpu.getA()
                regB.text = cpu.getB()
                regC.text = cpu.getC()
                regD.text = cpu.getD()
                regSP.text = cpu.getSP()
                regIP.text = cpu.getIP()
                regFO.text = cpu.getFO()
                regFP.text = cpu.getFP()
                regFZ.text = cpu.getFZ()
                regR0.text = cpu.getR0()
                regR1.text = cpu.getR1()
                regR2.text = cpu.getR2()
                regR3.text = cpu.getR3()
                regR4.text = cpu.getR4()
                regR5.text = cpu.getR5()
                var t = cpu.getRawMemory();
                for(var i = 0; i < 64;i++){
                    memory.text += t[i];
                    if(exectn == true){
                        if(i*4 == parseInt(regSP.text,16))
                            memory.text += "<-SP";
                        if(i*4 == parseInt(regIP.text,16))
                            memory.text += "<-IP";
                    }
                    if(i%4 == 3)
                        memory.text += "\n";
                    memory.text += "\n";
                }
                t = cpu.getDesasmbl();
                for(i = 0; i < 64;i++){
                    if(i % 4 == 0)
                        desasm.text += (i*4).toString(16).padStart(4, "0") + ": "
                    desasm.text += t[i];
                    if(i % 4 == 3)
                        desasm.text += "\n";
                }
                if (exectn == true){
                    exec.text = "Остановить выполнение (F5)"
                    ex.enabled = true
                    autoEx.enabled = true
                }
                if (exectn == false){
                    exec.text = "Начать выполнение (F5)"
                    ex.enabled = false
                    autoEx.enabled = false
                }

                }
            }
        Timer{
            id: timer
            interval: view.speed;
            repeat: true
            onTriggered:{
                if(cpu.tick() == false){
                    view.exectn = false
                    timer.stop()

                }
                view.update()
            }
        }
    }

        CPU_WLIV_EMU{
            id: cpu
        }

        Shortcut{
            sequence: "F5"
            onActivated: exec.trigger()
        }
        Shortcut{
            sequence: "F10"
            onActivated: ex.trigger()
        }
        Shortcut{
            sequence: "Ctrl+r"
            onActivated: reload.trigger()
        }
        Shortcut{
            sequence: "Ctrl+o"
            onActivated: opn.trigger()
        }

        Shortcut{
            sequence: "Ctrl+q"
            onActivated: cls.trigger()
        }



        MenuBar{
            width: parent.width
            height: 40

            Menu{
                title: qsTr("Файл")
                Action {
                    id: newFile
                    text: qsTr("Новый (Ctrl+N)")
                    onTriggered:{
                        view.file = cpu.getOpenEditor(view.editorpath)
                        cpu.getDataFromFile(view.file)
                        view.opened = true;
                        view.update()
                    }
                }

                Action {
                    id:opn
                    text: qsTr("Открыть (Ctrl+O)")
                    onTriggered: fileDialog.open()
                }


                Action {
                    id: openInEditor
                    text: qsTr("Открыть в редакторе (Ctrl+E)")
                    enabled: false

                    onTriggered:{
                        cpu.getOpenEditor(view.editorpath,view.file)

                        view.update()
                    }

                }

                Action {
                    id: reload
                    text: qsTr("Перезагрузить код (Ctrl+R)")

                    onTriggered:{
                        cpu.getDataFromFile(view.file)
                        view.update()
                    }

                }
                Action {
                    id:cls
                    text: qsTr("Закрыть (Ctrl+Q)")
                    onTriggered: mb_cls.open()
                }
                MenuSeparator{}
                Action {
                    text: qsTr("Выход")
                    onTriggered: Qt.quit()
                }
            }
            Menu{
                title: qsTr("Выполнить")
                Action {
                    id: exec
                    onTriggered: {
                        if(view.opened == true){
                            view.exectn = true
                            view.update()
                            if(view.running == true){
                                timer.stop()
                                view.running = false
                            }
                        }
                    }
                }
                Action {
                    id: ex
                    text: qsTr("Выполнить шаг (F10)")
                    enabled: false
                    onTriggered:{
                        if(cpu.tick()){
                            console.log("Tick");
                            view.update()

                        } else {
                            view.exectn = false
                            view.update()
                            //mb_pfp.open()
                        }

                    }
                }



                Action {
                    id: autoEx
                    text: qsTr("Автоматическое выполнение (F9)")
                    enabled: false
                    onTriggered: {
                        timer.start()
                        view.running = true
                    }

                    }
                }

                Menu {
                    title: qsTr("Настройки")
                }
            }



        Rectangle {
            id: rawMemArea
            x: 624
            y: 46
            width: 268
            height: 444
            color: "#f9f9f9"

            Label {
                id: memLabel
                x: 70
                y: 8
                text: qsTr("Память")
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 12
            }

            ScrollView {
                id: scrollView
                anchors.topMargin: 33
                anchors.fill: parent

                TextArea {
                    id: memory
                    anchors.rightMargin: -236
                    anchors.bottomMargin: -377
                    //text: ""
                    anchors.fill: parent
                    font.capitalization: Font.AllUppercase
                    wrapMode: Text.NoWrap
                    renderType: Text.QtRendering
                    font.family: "Courier"
                    font.pixelSize: 12
                }
            }
        }

        Rectangle {
            id: disMemArea
            x: 0
            y: 46
            width: 626
            height: 444
            color: "#f9f9f9"
            Label {
                id: disLabel
                x: 70
                y: 8
                text: qsTr("Дизассемблер")
                font.pointSize: 12
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ScrollView {
                id: scrollView1
                anchors.rightMargin: 0
                TextArea {
                    id: desasm
                    anchors.rightMargin: -591
                    anchors.bottomMargin: -377
                    anchors.fill: parent
                    wrapMode: Text.NoWrap
                    font.pixelSize: 12
                    renderType: Text.QtRendering
                    font.capitalization: Font.AllUppercase
                    font.family: "Courier"
                }
                anchors.fill: parent
                anchors.topMargin: 33
            }
        }

        FileDialog {
            id: fileDialog
            title: "Please choose a file"
            nameFilters: [ "Program files (*.prg)", "All files (*)" ]
            onAccepted: {
                view.file = fileDialog.fileUrl.toString().split("///")[1]
                cpu.getDataFromFile(view.file)
                view.opened = true;
                view.update()
                fileDialog.close()
            }
            onRejected: {
                fileDialog.close()
            }
        }

        MessageDialog{
            id: mb_rte
            title: "Runtime error"
            text: "Ошибка исполнения"
        }

        MessageDialog{
            id: mb_pfp
            title: ""
            text: "Программа завершила свою работу корректно"
        }

        MessageDialog{
            id: mb_cls
            title: "Закрыть файл"
            text: "Вы действительно хотите закрыть файл, никакие изминения не будут записаны?"
            onAccepted: {
                view.file = ""
                view.opened = false
                view.update()
            }


        }

        Rectangle {
            id: rectangle1
            x: 891
            y: 46
            width: 348
            height: 444
            color: "#ffffff"

            Label {
                id: label10
                text: qsTr("Блок регистров")
                anchors.bottomMargin: 420
                anchors.fill: parent
                font.pointSize: 9
                horizontalAlignment: Text.AlignHCenter
            }

            TextField {
                id: regR0
                x: 195
                y: 32
                width: 150
                height: 40
                //text: cpu.getR0()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label11
                x: 139
                y: 36
                width: 50
                text: qsTr("R0")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regR1
                x: 195
                y: 78
                width: 150
                height: 40
                //text: cpu.getR1()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label12
                x: 139
                y: 82
                width: 50
                text: qsTr("R1")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regR2
                x: 195
                y: 128
                width: 150
                height: 40
                //text: cpu.getR2()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label13
                x: 139
                y: 128
                width: 50
                text: qsTr("R2")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regR3
                x: 196
                y: 173
                width: 150
                height: 40
                //text: cpu.getR3()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label14
                x: 139
                y: 174
                width: 50
                text: qsTr("R3")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regR4
                x: 196
                y: 224
                width: 150
                height: 40
                //text: cpu.getR4()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label15
                x: 140
                y: 227
                width: 50
                text: qsTr("R4")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regR5
                x: 196
                y: 275
                width: 150
                height: 40
                //text: cpu.getR5()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label16
                x: 140
                y: 278
                width: 50
                text: qsTr("R5")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            Label {
                id: label
                x: -9
                y: 36
                width: 50
                text: qsTr("A")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regA
                x: 35
                y: 35
                width: 80
                height: 40
                //text: cpu.getA()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
            }

            Label {
                id: label1
                x: -9
                y: 82
                width: 50
                text: qsTr("B")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regB
                x: 35
                y: 81
                width: 80
                height: 40
                //text: cpu.getB()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label2
                x: -9
                y: 128
                width: 50
                text: qsTr("C")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regC
                x: 35
                y: 127
                width: 80
                height: 40
                //text: cpu.getC()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label3
                x: -9
                y: 174
                width: 50
                text: qsTr("D")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regD
                x: 35
                y: 173
                width: 80
                height: 40
                //text: cpu.getD()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label4
                x: -9
                y: 278
                width: 50
                text: qsTr("SP")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regSP
                x: 35
                y: 272
                width: 80
                height: 40
                //text: cpu.getSP()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label8
                x: -9
                y: 227
                width: 50
                text: qsTr("IP")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regIP
                x: 35
                y: 221
                width: 80
                height: 40
                //text: cpu.getIP()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            TextField {
                id: regFZ
                x: 52
                y: 351
                width: 34
                height: 40
                //text: cpu.getFZ()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label20
                x: 44
                y: 323
                width: 50
                text: qsTr("FZ")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regFP
                x: 237
                y: 351
                width: 34
                height: 40
                //text: cpu.getFP()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label21
                x: 229
                y: 323
                width: 50
                text: qsTr("FP")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

            TextField {
                id: regFO
                x: 147
                y: 351
                width: 34
                height: 40
                //text: cpu.getFO()
                font.pointSize: 9
                font.capitalization: Font.AllUppercase
                horizontalAlignment: Text.AlignRight
                font.family: "Courier"
            }

            Label {
                id: label22
                x: 140
                y: 323
                width: 50
                text: qsTr("FO")
                font.pointSize: 19
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
            }

        }



}

/*##^##
Designer {
    D{i:14;anchors_height:474;anchors_width:200;anchors_x:0;anchors_y:0}D{i:15;anchors_height:386;anchors_width:250;anchors_x:4;anchors_y:50}
D{i:19;anchors_height:386;anchors_width:250;anchors_x:4;anchors_y:50}D{i:18;anchors_height:474;anchors_width:200;anchors_x:0;anchors_y:0}
D{i:50;anchors_height:219;anchors_width:398;anchors_x:"-150";anchors_y:25}D{i:51;anchors_height:219;anchors_width:398;anchors_x:"-150";anchors_y:25}
D{i:52;anchors_height:219;anchors_width:398;anchors_x:"-150";anchors_y:25}D{i:53;anchors_height:474;anchors_width:200;anchors_x:0;anchors_y:0}
D{i:54;anchors_height:386;anchors_width:250;anchors_x:4;anchors_y:50}D{i:55;anchors_height:386;anchors_width:250;anchors_x:4;anchors_y:50}
D{i:58;anchors_height:219;anchors_width:398;anchors_x:"-150";anchors_y:25}
}
##^##*/
