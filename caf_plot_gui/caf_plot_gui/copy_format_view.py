# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'data/copy_format_view.ui'
#
# Created by: PyQt5 UI code generator 5.7
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_form_format_copy(object):
    def setupUi(self, form_format_copy):
        form_format_copy.setObjectName("form_format_copy")
        form_format_copy.setWindowModality(QtCore.Qt.WindowModal)
        form_format_copy.setEnabled(True)
        form_format_copy.resize(392, 111)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(form_format_copy.sizePolicy().hasHeightForWidth())
        form_format_copy.setSizePolicy(sizePolicy)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(":/Images/caf-logo"), QtGui.QIcon.Normal, QtGui.QIcon.On)
        form_format_copy.setWindowIcon(icon)
        self.btn_adopt_format_1 = QtWidgets.QPushButton(form_format_copy)
        self.btn_adopt_format_1.setGeometry(QtCore.QRect(270, 9, 113, 27))
        self.btn_adopt_format_1.setBaseSize(QtCore.QSize(100, 100))
        self.btn_adopt_format_1.setObjectName("btn_adopt_format_1")
        self.btn_save_format_2 = QtWidgets.QPushButton(form_format_copy)
        self.btn_save_format_2.setGeometry(QtCore.QRect(9, 42, 102, 27))
        self.btn_save_format_2.setObjectName("btn_save_format_2")
        self.lineEdit_2 = QtWidgets.QLineEdit(form_format_copy)
        self.lineEdit_2.setGeometry(QtCore.QRect(117, 42, 146, 27))
        self.lineEdit_2.setObjectName("lineEdit_2")
        self.lineEdit_3 = QtWidgets.QLineEdit(form_format_copy)
        self.lineEdit_3.setGeometry(QtCore.QRect(117, 75, 146, 27))
        self.lineEdit_3.setObjectName("lineEdit_3")
        self.btn_adopt_format_3 = QtWidgets.QPushButton(form_format_copy)
        self.btn_adopt_format_3.setGeometry(QtCore.QRect(270, 75, 113, 27))
        self.btn_adopt_format_3.setObjectName("btn_adopt_format_3")
        self.lineEdit = QtWidgets.QLineEdit(form_format_copy)
        self.lineEdit.setGeometry(QtCore.QRect(117, 9, 146, 27))
        self.lineEdit.setObjectName("lineEdit")
        self.btn_save_format_1 = QtWidgets.QPushButton(form_format_copy)
        self.btn_save_format_1.setGeometry(QtCore.QRect(9, 9, 102, 27))
        self.btn_save_format_1.setObjectName("btn_save_format_1")
        self.btn_adopt_format_2 = QtWidgets.QPushButton(form_format_copy)
        self.btn_adopt_format_2.setGeometry(QtCore.QRect(270, 42, 113, 27))
        self.btn_adopt_format_2.setObjectName("btn_adopt_format_2")
        self.btn_save_format_3 = QtWidgets.QPushButton(form_format_copy)
        self.btn_save_format_3.setGeometry(QtCore.QRect(9, 75, 102, 27))
        self.btn_save_format_3.setObjectName("btn_save_format_3")

        self.retranslateUi(form_format_copy)
        QtCore.QMetaObject.connectSlotsByName(form_format_copy)

    def retranslateUi(self, form_format_copy):
        _translate = QtCore.QCoreApplication.translate
        form_format_copy.setWindowTitle(_translate("form_format_copy", "Copy Format Toolbox"))
        self.btn_adopt_format_1.setText(_translate("form_format_copy", "Adopt Format"))
        self.btn_save_format_2.setText(_translate("form_format_copy", "Save Format"))
        self.lineEdit_2.setPlaceholderText(_translate("form_format_copy", "Note"))
        self.lineEdit_3.setPlaceholderText(_translate("form_format_copy", "Note"))
        self.btn_adopt_format_3.setText(_translate("form_format_copy", "Adopt Format"))
        self.lineEdit.setPlaceholderText(_translate("form_format_copy", "Note"))
        self.btn_save_format_1.setText(_translate("form_format_copy", "Save Format"))
        self.btn_adopt_format_2.setText(_translate("form_format_copy", "Adopt Format"))
        self.btn_save_format_3.setText(_translate("form_format_copy", "Save Format"))

import resource_file_rc
