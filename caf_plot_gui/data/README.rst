My Notes
========

Reference
--------
* https://nikolak.com/pyqt-qt-designer-getting-started/

Install
-------
sudo apt install pyqt5-dev-tools 
sudo apt install qttools5-dev-tools

Set qt-version
--------------
export QT_SELECT="qt5"

Run designer
------------
designer data/main_view.ui
designer data/copy_format_view.ui

Gernerate py-files
------------------
pyuic5 data/main_view.ui -o caf_plot_gui/main_view.py
pyuic5 data/copy_format_view.ui -o caf_plot_gui/copy_format_view.py
pyrcc5 data/resource_file.qrc -o resource_file_rc.py
