#!/bin/bash


# Reference
# https://nikolak.com/pyqt-qt-designer-getting-started/

#sudo apt install python-qt4 pyqt4-dev-tools qt4-designer
sudo apt install pyqt5-dev-tools 
sudo apt install qttools5-dev-tools

# python environment
sudo pip install virtualenv
virtualenv venv
virtualenv -p python3 venv
echo 'source venv/bin/activate/' > .env

# set qt-version
#qtchooser -list-versions
#qtchooser -print-env 
echo 'export QT_SELECT="qt5"' >> .env


# run
designer view.ui
# do stuff
# save stuff as design.py
# creaty desing.py
pyuic5 view.ui -o view.py
