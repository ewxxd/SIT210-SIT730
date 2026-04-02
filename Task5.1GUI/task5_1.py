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
        self.setGeometry(200, 200, 900, 600)
        self.setWindowTitle("TASK 5.1")
        self.initUI()

    def initUI(self):
        self.label = QtWidgets.QLabel(self)
        self.label.setText("My label")
        self.label.move(50, 100)
        
        self.livingRoom = QtWidgets.QRadioButton(self)
        self.livingRoom.move(50, 50)
        self.livingRoom.setText("Living Room")
        self.livingRoom.toggled.connect(lambda: self.trigger(40))

        self.bathroom = QtWidgets.QRadioButton(self)
        self.bathroom.move(200, 50)
        self.bathroom.setText("Bathroom")
        self.bathroom.toggled.connect(lambda: self.trigger(38))

        self.closet = QtWidgets.QRadioButton(self)
        self.closet.move(350, 50)
        self.closet.setText("Closet")
        self.closet.toggled.connect(lambda: self.trigger(36))

        self.exitBtn = QtWidgets.QPushButton(self)
        self.exitBtn.move(50, 200)
        self.exitBtn.setText("Exit")
        self.exitBtn.clicked.connect(self.close)

    def trigger(self, pin):
        if self.sender().isChecked():
            GPIO.output(40, GPIO.LOW)
            GPIO.output(38, GPIO.LOW)
            GPIO.output(36, GPIO.LOW)

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
