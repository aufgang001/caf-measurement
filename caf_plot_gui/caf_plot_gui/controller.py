from PyQt5 import QtGui, QtWidgets
from PyQt5.QtCore import Qt
import sys
import os
import copy
import re

# own imports
from caf_plot_gui import view 
from caf_plot_gui import model 

class controller(QtWidgets.QMainWindow, view.Ui_MainWindow):
    def __init__(self, model, parent=None):
        super(controller, self).__init__(parent)
        self.setupUi(self)
        self.model = model # model
        self.selected_data= dict()
        self.modifing_on_tabel_tlabel = False
        # set all handler
        self.radiobtn_performance.clicked.connect(self.lst_csv_file_refresh)
        self.radiobtn_memory.clicked.connect(self.lst_csv_file_refresh)
        self.closeEvent = self.main_window_close_event
        self.lst_csv.keyPressEvent = self.lst_csv_keyPressEvent
        self.lst_csv.clicked.connect(self.lst_csv_clicked)
        self.lst_label.doubleClicked.connect(self.lst_label_double_clicked)
        self.table_tlabel.keyPressEvent = self.table_tlabel_keyPressEvent
        self.table_tlabel.itemChanged.connect(self.table_tlabel_cellChanged)
        self.btn_preview_plot.clicked.connect(self.preview_current_plot)
        self.action_preview.triggered.connect(self.preview_current_plot)
        self.action_plot_all.triggered.connect(self.plot_all_plots)
        self.action_add_cvs_folder.triggered.connect(self.add_cvs_folder)
        self.action_set_plot_script.triggered.connect(self.set_plot_script)
        self.action_set_pdf_program.triggered.connect(self.set_pdf_program)
        self.bnt_save_plot.clicked.connect(self.btn_save_plot_clicked)
        self.cbox_plots.currentIndexChanged.connect(self.cbox_plots_current_index_changed)
        self.btn_delete_plot.clicked.connect(self.btn_delete_clicked)
        self.btn_new_plot.clicked.connect(self.btn_new_clicked)
        self.edit_label_filter.textChanged.connect(self.lst_label_refresh)
        # set mode performacne or memory and update csv_list
        self.lst_csv_file_refresh()
        self.cbox_plots_refresh(index=0)

    def btn_delete_clicked(self):
        plot_name = self.cbox_plots.currentText()
        quit_msg = "Delete " + plot_name + "?"
        reply = QtWidgets.QMessageBox.question(self, 'Message', quit_msg, QtWidgets.QMessageBox.Yes, QtWidgets.QMessageBox.No)
        if reply == QtWidgets.QMessageBox.Yes:
            del self.model.plots[plot_name]
            self.cbox_plots_refresh(index=0)

    def btn_new_clicked(self):
        self.load_plot_data(self.get_new_data())

    def btn_save_plot_clicked(self):
        data = self.collect_plot_data()
        if not data == None:
            self.model.plots[data["out"]] = data
            self.cbox_plots_refresh(text=data["out"])

    def cbox_plots_current_index_changed(self):
        plot_name = self.cbox_plots.currentText()
        if plot_name in self.model.plots:
            self.load_plot_data(self.model.plots[plot_name])

    def cbox_plots_refresh(self, text=None, index=None):
        self.cbox_plots.clear()
        self.cbox_plots.addItems(self.model.plots.keys())
        if not index == None:
            self.cbox_plots.setCurrentIndex(index)
        elif not text == None:
            index = self.cbox_plots.findText(text)
            if not index == -1:
                self.cbox_plots.setCurrentIndex(index)

    def main_window_close_event(self, event):
        quit_msg = "Save changes?"
        reply = QtWidgets.QMessageBox.question(self, 'Message', quit_msg, QtWidgets.QMessageBox.Yes, QtWidgets.QMessageBox.No)

        if reply == QtWidgets.QMessageBox.Yes:
            self.btn_save_plot_clicked()
            self.model.save_config_file()
            
    def lst_csv_file_refresh(self):
        if self.radiobtn_memory.isChecked():
            self.mode = model.plot_type.memory
        else:
            self.mode = model.plot_type.performance
        self.lst_csv.clear()
        self.lst_csv.addItems(self.model.get_csv_files(self.mode))
        self.lst_csv.sortItems()
        #all other fileds
        self.lst_label.clear()

    def lst_label_refresh(self):
        def label_filter(str, filters):
            for filter in filters: 
                if not filter in str:
                    return False 
            return True
        csv_file = self.get_selected_csv_file()
        if csv_file:
            filters = self.edit_label_filter.text().split()
            labels = self.model.get_csv_file_header(csv_file)
            labels = [label for label in labels if label_filter(label, filters)]
            self.lst_label.clear()
            self.lst_label.addItems(labels)
            self.lst_label.sortItems()

    def lst_csv_clicked(self):
        self.lst_label_refresh()

    def lst_csv_keyPressEvent(self, event):
        if event.matches(QtGui.QKeySequence.Delete):
            items = self.lst_csv.selectedItems()
            if len(items) > 0:
                self.model.remove_csv_folder(items[0].text())
                self.lst_csv_file_refresh()

    def get_selected_csv_file(self):
        items = self.lst_csv.selectedItems()
        if len(items) > 0:
            return items[0].text()
        else:
            return None

    def get_selected_label(self):
        items = self.lst_label.selectedItems()
        if len(items) > 0:
            return items[0].text()
        else:
            return None

    def preview_current_plot(self):
        data = self.collect_plot_data()
        if not data == None:
            self.model.plot_data(data)
            self.model.show_plot(data)

    def plot_all_plots(self):
        for data in self.model.plots.values():
            self.model.plot_data(data)

    def lst_label_double_clicked(self):
        csv_file = self.get_selected_csv_file()
        label = self.get_selected_label()
        self.table_tlabel_add_label(csv_file, label)

        index = self.table_tlabel_get_row_index(csv_file, label)
        self.table_tlabel_select_row(index)

    def table_tlabel_cellChanged(self, witem):
        if not self.modifing_on_tabel_tlabel:
            index = witem.row()
            to_tlabel = self.table_tlabel.model().index(index, 1).data()
            csv_file, label = self.table_tlabel_get_row_data(index)
            self.selected_data[csv_file][label] = to_tlabel

    def table_tlabel_keyPressEvent(self, event):
        if event.matches(QtGui.QKeySequence.Delete):
            rows = self.table_tlabel.selectionModel().selectedRows()
            if len(rows) > 0:
                self.table_tlabel_remove_label(rows[0].row())
                self.table_tlabel_redraw()

    def table_tlabel_add_label(self, csv_file, label):
        if csv_file and label:
            if csv_file in self.selected_data:
                if not label in self.selected_data[csv_file]:
                    self.selected_data[csv_file][label] = label
            else:
                self.selected_data[csv_file] = dict()
                self.selected_data[csv_file][label] = label
        self.table_tlabel_redraw()
    
    def table_tlabel_get_row_index(self, csv_file, label):
        y = 0
        for csv_file_it in self.selected_data.keys():
            y += 1
            if csv_file == csv_file_it:
                for label_it in self.selected_data[csv_file].keys():
                    if label == label_it:
                        return y
                    y += 1

    def table_tlabel_get_row_data(self, index):
        y = 0
        for csv_file in self.selected_data.keys():
            if index == y:
                return (csv_file, None)
            y += 1
            for label in self.selected_data[csv_file].keys():
                if index == y:
                    return (csv_file, label)
                y += 1
        return (None, None)

    def table_tlabel_redraw(self):
        self.modifing_on_tabel_tlabel = True
        header_font = QtGui.QFont()
        header_font.setBold(True)
        csv_file_dict = dict()
        self.table_tlabel.clear()
        self.table_tlabel.clearSpans()
        self.table_tlabel.setColumnCount(2)
        self.table_tlabel.setRowCount(sum([1 + len(csv_file_labels) for csv_file_labels in self.selected_data.values()]))
        y = 0
        for csv_file, labels in self.selected_data.items():
            csv_file_dict[y] = csv_file
            y += 1
            for label, tlabel in labels.items():
                # self.table_tlabel.setSpan(y, 0, 1, 1)
                self.table_tlabel.setItem(y, 0, QtWidgets.QTableWidgetItem(label))
                self.table_tlabel.setItem(y, 1, QtWidgets.QTableWidgetItem(tlabel))
                y += 1
            
        self.table_tlabel.setHorizontalHeaderLabels(["Label", "TLabel"])
        self.table_tlabel.resizeColumnsToContents()

        for y, csv_file in csv_file_dict.items():
            self.table_tlabel.setSpan(y, 0, 1, 2)
            self.table_tlabel.setItem(y, 0, QtWidgets.QTableWidgetItem(csv_file))
            self.table_tlabel.item(y, 0).setFont(header_font)

        self.modifing_on_tabel_tlabel = False
  
    def table_tlabel_remove_label(self, index):
        csv_file, label = self.table_tlabel_get_row_data(index)
        if label:
            del self.selected_data[csv_file][label]
            if len(self.selected_data[csv_file]) == 0:
                del self.selected_data[csv_file]
        else:
            del self.selected_data[csv_file]

    def table_tlabel_select_row(self, index):
        range = QtWidgets.QTableWidgetSelectionRange(index, 0, index, 1) 
        self.table_tlabel.setRangeSelected(range, True)

    def add_cvs_folder(self):
        folder = QtWidgets.QFileDialog.getExistingDirectory(None, 'Select a folder:', '.', QtWidgets.QFileDialog.ShowDirsOnly)
        self.model.add_csv_folder(folder)
        self.lst_csv_file_refresh()

    def set_plot_script(self):
        plot_script = QtWidgets.QFileDialog.getOpenFileName(None, 'Select plot script:', '.')
        self.model.plot_script = plot_script[0]

    def set_pdf_program(self):
        pdf_program = QtWidgets.QInputDialog.getText(None, 'Set PDF program:', "PDF Program:")
        self.model.plot_data = pdf_program[0]

    def collect_plot_data(self):
        if not self.edit_filename.text() == "":
            data = dict()   
            data["plot_type"] = str(self.mode)
            out_file = self.edit_filename.text();
            if not re.match(".*\.pdf$", out_file):
                out_file += ".pdf"
                self.edit_label_filter.setText(out_file)
            data["out"] = out_file 
            data["title"] = self.edit_title.text()
            data["ylabel"] = self.edit_ylabel.text()
            data["xlabel"] = self.edit_xlabel.text()
            data["filelabel_data"] = copy.deepcopy(self.selected_data)
            return data
        else:
            return None

    def get_new_data(self):
        data = dict()   
        data["plot_type"] = str(self.mode)
        data["out"] = "new.pdf"
        data["title"] = ""
        data["ylabel"] = self.edit_ylabel.text()
        data["xlabel"] = self.edit_xlabel.text()
        data["filelabel_data"] = dict()
        return data

    def load_plot_data(self, data):
        if not data == None:
            if data["plot_type"] == str(model.plot_type.memory):
                self.radiobtn_memory.setChecked(True)
            else:
                self.radiobtn_performance.setChecked(True)
            self.edit_filename.setText(data["out"])
            self.edit_title.setText(data["title"])
            self.edit_xlabel.setText(data["xlabel"])
            self.edit_ylabel.setText(data["ylabel"])
            self.selected_data = copy.deepcopy(data["filelabel_data"])
            self.table_tlabel_redraw()