from PyQt5 import QtGui, QtWidgets
from PyQt5.QtCore import Qt
import copy

# own imports
from caf_plot_gui import copy_format_view


class copy_format(QtWidgets.QMainWindow, copy_format_view.Ui_form_format_copy):
    class btn_proxy():
        class call_me():
            def __init__(self, fun, id):
                self.fun = fun
                self.id = id
            def __call__(self):
                self.fun(self.id)    
        def __init__(self, fun, count):
            self.funs_ = dict()
            for id in range(0, count):
                self.funs_[id] = self.call_me(fun, id) 
        def __call__(self, id):
            return self.funs_[id]

    def __init__(self, controller, parent=None):
        super(copy_format, self).__init__(parent)
        self.setupUi(self)
        self.controller = controller
        self.adopt_fun = self.btn_proxy(self.adopt_format, 3)
        self.save_fun = self.btn_proxy(self.save_format, 3)
        self.saved_format = dict()
        # set all handlers
        self.btn_save_format_1.clicked.connect(self.save_fun(0))
        self.btn_save_format_2.clicked.connect(self.save_fun(1))
        self.btn_save_format_3.clicked.connect(self.save_fun(2))
        self.btn_adopt_format_1.clicked.connect(self.adopt_fun(0))
        self.btn_adopt_format_2.clicked.connect(self.adopt_fun(1))
        self.btn_adopt_format_3.clicked.connect(self.adopt_fun(2))

    def adopt_format(self, button_id):
        if not button_id in self.saved_format:
            return
        tth = self.controller.table_tlabel_handle
        selected_row_idx = tth.get_selected_row_idx()
        if selected_row_idx != -1:
            csv_file = tth.get_row_data(selected_row_idx)[0]
            tth.selected_data[csv_file] = copy.deepcopy(self.saved_format[button_id])
            tth.redraw()

    def save_format(self, button_id):
        tth = self.controller.table_tlabel_handle
        selected_row_idx = tth.get_selected_row_idx()
        if selected_row_idx != -1:
            csv_file = tth.get_row_data(selected_row_idx)[0]
            self.saved_format[button_id] = copy.deepcopy(tth.selected_data[csv_file])


    def show_window(self):
        self.show()

    def close_window(self):
        self.close()

