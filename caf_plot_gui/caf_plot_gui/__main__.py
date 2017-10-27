import sys
import argparse
from PyQt5 import QtGui, QtWidgets
from caf_plot_gui import controller
from caf_plot_gui import model 

config_file = "caf_plot_gui_conf.json"

def usage():
    print('''caf_plot_gui [CONFIG_FILE]
          CONFIG_FILE:    stores the work done (default:{})
    '''.format(config_file))
    exit()

def parse_all_args():
    parser = argparse.ArgumentParser(description="CAF Plot Gui") 
    parser.add_argument("--plot-all", dest="plot_all", action="store_true")
    parser.add_argument(dest="config_file", nargs="*")
    return parser.parse_args()

def main():
    global config_file
    args = parse_all_args()
    if args.plot_all:
        for file in args.config_file:
            m = model.model(file)
            for data in m.plots.values():
                m.plot_data(data)
    else:
        if len(args.config_file) == 1:
            config_file = args.config_file[0]
        app = QtWidgets.QApplication(sys.argv)
        form = controller.controller(model.model(config_file))
        form.show()
        app.exec_()

if __name__ == '__main__':
    main()
