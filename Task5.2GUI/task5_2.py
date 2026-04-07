from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.QtCore import QTimer, Qt
import sys
import RPi.GPIO as GPIO
from gpiozero import PWMLED
from time import sleep

GPIO.setmode(GPIO.BOARD)

LIVING = PWMLED(19)
BATH = 38
CLOSET = 36

GPIO.setup(BATH, GPIO.OUT)
GPIO.setup(CLOSET, GPIO.OUT)


class MyWindow(QMainWindow):
    def __init__(self):
        super(MyWindow, self).__init__()
        self.setGeometry(200, 200, 900, 600)
        self.setWindowTitle("TASK 5.2")
        self.initUI()

    def initUI(self):
        self.label = QtWidgets.QLabel(self)
        self.label.setText("My label")
        self.label.move(50, 100)
        
        self.livingRoom = QtWidgets.QRadioButton(self)
        self.livingRoom.move(50, 50)
        self.livingRoom.setText("Living Room")
        self.livingRoom.toggled.connect(lambda: self.trigger(LIVING))

        self.bathroom = QtWidgets.QRadioButton(self)
        self.bathroom.move(200, 50)
        self.bathroom.setText("Bathroom")
        self.bathroom.toggled.connect(lambda: self.trigger(BATH))

        self.closet = QtWidgets.QRadioButton(self)
        self.closet.move(350, 50)
        self.closet.setText("Closet")
        self.closet.toggled.connect(lambda: self.trigger(CLOSET))

        self.exitBtn = QtWidgets.QPushButton(self)
        self.exitBtn.move(50, 200)
        self.exitBtn.setText("Exit")
        self.exitBtn.clicked.connect(self.close)
        
        self.adjustBrightness = QtWidgets.QSlider(Qt.Orientation.Horizontal, self)
        self.adjustBrightness.setMinimum(0)
        self.adjustBrightness.setMaximum(100)
        self.adjustBrightness.move(50, 150)
        self.adjustBrightness.setValue(0)
        self.adjustBrightness.setSingleStep(25)
        self.adjustBrightness.setPageStep(1)
        self.adjustBrightness.setTickPosition(QtWidgets.QSlider.TickPosition.TicksAbove)
        
        self.adjustBrightness.valueChanged.connect(self.update)

    def trigger(self, pin):
        if self.sender().isChecked():
            LIVING.value = 0
            GPIO.output(BATH, GPIO.LOW)
            GPIO.output(CLOSET, GPIO.LOW)
            
            if pin == LIVING:
                LIVING.value = 1
                self.label.setText(f"Triggered pin {pin}")
                self.label.adjustSize()
            else:
                GPIO.output(pin, GPIO.HIGH)
                self.label.setText(f"Triggered pin {pin}")
                self.label.adjustSize()

    def closeEvent(self, event):
        GPIO.cleanup()
        event.accept()
        
    def update(self):
        if self.livingRoom.isChecked():
            if (self.adjustBrightness.value() < 25):
                LIVING.value = self.adjustBrightness.value() / 100
            elif (self.adjustBrightness.value() < 50):
                LIVING.value = self.adjustBrightness.value() / 100
            elif (self.adjustBrightness.value() < 75):
                LIVING.value = self.adjustBrightness.value() / 100
            else:
                LIVING.value = 1


def window():
    app = QApplication(sys.argv)
    win = MyWindow()
    win.show()
    sys.exit(app.exec_())


window()
