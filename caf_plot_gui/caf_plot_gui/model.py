import os
import re
import json
import subprocess
from enum import Enum

class plot_type(Enum):
    memory = 0
    performance = 1

class model:
    def __init__(self, config_file):
        self.config_file = config_file
        self.csv_folder_set = set()
        self.csv_file_set = set()
        self.csv_header = dict() 
        self.plot_script = "caf_plot"
        self.csv_blacklist = set()
        self.csv_blacklist.add("tmp_data.csv") 
        self.pdf_program = "gnome-open"
        self.plots = dict()
        self.load_config_file()

    def add_csv_folder(self, folder):
        """
        Search recursive in path data_folder for *.csv files
        """
        self.csv_folder_set.add(folder)
        prog = re.compile(".*\.csv$")
        for root, dirs, files in os.walk(folder):
            for file in files:
                # filter blacklist
                if file not in self.csv_blacklist:
                    if prog.match(file):
                        self.csv_file_set.add(root + "/" + file)

    def remove_csv_folder(self, folder):
        self.csv_file_set.remove(folder)

    def get_csv_files(self, plott):
            return [x for x in self.csv_file_set if self.get_file_plot_type(x) == plott]

    def get_csv_file_header(self, csv_file):
        if csv_file in self.csv_header:
            return self.csv_header[csv_file]
        with open(csv_file, 'r') as f:
            header = [x.strip() for x in f.readline().split(",")]
            if self.get_file_plot_type(csv_file) == plot_type.memory:
                self.csv_header[csv_file] = header
                return header  
            else:
                prog = re.compile(".*_yerr$")
                tmp_header_list =[x for x in header[1:] if not prog.match(x)]
                self.csv_header[csv_file] = tmp_header_list
                return tmp_header_list

    def get_file_plot_type(self, file):
        prog = re.compile(".*/memory_.*")
        if prog.match(file):
            return plot_type.memory
        else:
            return plot_type.performance

    def refresh_csv_data(self):
        self.csv_file_set= set()
        for x in self.csv_folder_set:
            self.add_csv_folder(x)
    
    def config_to_json_str(self):
        # collect data
        data = dict()
        data["abs_path"] = os.path.dirname(os.path.abspath(__file__))
        data["csv_folder_set"] = list(self.csv_folder_set)
        data["csv_blacklist"] = list(self.csv_blacklist)
        data["plot_script"] = self.plot_script
        data["pdf_program"] = self.pdf_program
        data["plots"] = self.plots
        return  json.dumps(data, separators=(',',':'), indent=4, sort_keys=True)

    def plot_config(self):
        print(self.config_to_json_str())

    def save_config_file(self):
        with open(self.config_file, 'w') as f:
            f.write(self.config_to_json_str())

    def load_config_file(self):
        try:
            with open(self.config_file, 'r') as f:
                tmp_str = f.read()
                if tmp_str == "":
                    return
                data = json.loads(tmp_str)
                # reconstruct data
                self.csv_folder_set.update(set(data["csv_folder_set"]))
                self.refresh_csv_data()
                self.csv_blacklist.update(set(data["csv_blacklist"]))
                self.plot_script = data["plot_script"]
                self.pdf_program = data["pdf_program"]
                if "plots" in data:
                    self.plots = data["plots"]
        except FileNotFoundError:
            pass

    def show_plot(self, data):
        subprocess.call([self.pdf_program, data["out"]])

    def plot_data(self, data):
        plot = [self.plot_script]
        if data["plot_type"] ==  str(plot_type.performance):
            plot.append("performance")
        else:
            plot.append("memory")

        tlabels = ""
        for csv_file, columns in data["filelabel_data"].items():
            tmp_str = csv_file
            for column, tlabel in columns.items():
                tmp_str += "," + column
                tlabels+="," + tlabel
            plot.append(tmp_str)
        tlabels = tlabels[1:]
        plot.append('--out="' + data["out"] + '"')
        if not data["xlabel"] == "":
            plot.append('--xlabel="' + data["xlabel"] + '"')
        if not data["ylabel"] == "":
            plot.append('--ylabel="' + data["ylabel"] + '"' )
        if not data["title"] == "":
            plot.append('--title="' + data["title"] + '"')
        if not data["ydivider"] == "":
            plot.append('--ydivider="' + data["ydivider"] + '"')
        if not data["xlim"][0] == "" or not data["xlim"][1] == "":
            plot.append('--xlim="' + data["xlim"][0] + "," + data["xlim"][1] + '"')
        if not data["ylim"][0] == "" or not data["ylim"][1] == "":
            plot.append('--ylim="' + data["ylim"][0] + "," + data["ylim"][1] + '"')
        plot.append('--tlabel="' + tlabels + '"')
        print("wait for: " + str(data["out"]))
        p = subprocess.check_call(plot)
        print("finished")
