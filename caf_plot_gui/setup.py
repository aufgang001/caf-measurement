try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

with open('README.rst') as src:
    long_description = src.read()

with open('requirements.txt') as src:
    requirments = src.read().split('\n')

setup(
    name='caf_plot_gui',
    packages=['caf_plot_gui'],
    scripts=['bin/caf_plot_gui'],
    version='0.0.1',
    install_requires=requirments,
    description='CAF Plot GUI',
    long_description=long_description,
    classifiers=[
        'Programming Language :: Python :: 3.5'
    ]
)

