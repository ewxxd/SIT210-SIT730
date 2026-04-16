from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.QtCore import QTimer
import sys
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)

GPIO.setup(40, GPIO.OUT)
GPIO.setup(38, GPIO.OUT)
GPIO.setup(36, GPIO.OUT)

class MyWindow(QMainWindow):
    def __init__(self):
        super(MyWindow, self).__init__()
        self.setGeometry(200, 200, 800, 500)
        self.setWindowTitle("Smart Home Lighting")
        self.initUI()

    def initUI(self):
        self.title = QtWidgets.QLabel(self)
        self.title.setText("Smart Home Lighting Control")
        self.title.setStyleSheet("font-size: 22px; font-weight: bold;")
        self.title.move(200, 20)
        self.title.adjustSize()
        
        self.livingRoom = QtWidgets.QRadioButton(self)
        self.livingRoom.move(50, 60)
        self.livingRoom.setText("Living Room")
        self.livingRoom.toggled.connect(lambda: self.trigger(40))

        self.bathroom = QtWidgets.QRadioButton(self)
        self.bathroom.move(200, 60)
        self.bathroom.setText("Bathroom")
        self.bathroom.toggled.connect(lambda: self.trigger(38))

        self.closet = QtWidgets.QRadioButton(self)
        self.closet.move(350, 60)
        self.closet.setText("Closet")
        self.closet.toggled.connect(lambda: self.trigger(36))

        self.label = QtWidgets.QLabel(self)
        self.label.setText("Select a room to turn ON the light")
        self.label.setStyleSheet("font-size: 14px;")
        self.label.move(220, 280)
        self.label.adjustSize()

        self.status = QtWidgets.QLabel(self)
        self.status.setGeometry(350, 320, 100, 40)
        self.status.setStyleSheet("background-color: grey; border-radius: 10px;")

        self.exitBtn = QtWidgets.QPushButton(self)
        self.exitBtn.setGeometry(350, 400, 100, 40)
        self.exitBtn.setText("Exit")
        self.exitBtn.clicked.connect(self.close)

    def trigger(self, pin):
        if self.sender().isChecked():
            GPIO.output(pin, GPIO.HIGH)
            self.label.setText(f"Triggered pin {pin}")
            self.label.adjustSize()

    def closeEvent(self, event):
        GPIO.cleanup()
        event.accept()


def window():
    app = QApplication(sys.argv)
    win = MyWindow()
    win.show()
    sys.exit(app.exec_())


window()
