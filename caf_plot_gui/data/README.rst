My Notes
========

Reference
--------
* https://nikolak.com/pyqt-qt-designer-getting-started/

Install
-------
* sudo apt install pyqt5-dev-tools 
* sudo apt install qttools5-dev-tools

Set qt-version
--------------
* export QT_SELECT="qt5"

Run designer
------------
* designer data/view.ui

Gernerate py-files
------------------
* pyuic5 data/view.ui -o caf_plot_gui/view.py
* pyrcc5 data/resource_file.qrc -o caf_plot_gui/resource_file_rc.py
