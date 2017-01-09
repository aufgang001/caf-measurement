import sys
from caf_plot_gui import controller
from caf_plot_gui import model 
from PyQt5 import QtGui, QtWidgets

config_file = "caf_plot_gui_conf.json"

def usage():
    print('''caf_plot_gui [CONFIG_FILE]
          CONFIG_FILE:    stores the work done (default:{})
    '''.format(config_file))
    exit()

def main():
    global config_file
    if len(sys.argv) > 2:
        usage() 
    if len(sys.argv) == 2:
        if sys.argv[1] in ["-h", "--help", "-?", "--?"]:
            usage()
        else:
            config_file = sys.argv[1]
    app = QtWidgets.QApplication(sys.argv)
    form = controller.controller(model.model(config_file))
    form.show()
    app.exec_()

if __name__ == '__main__':
    main()
