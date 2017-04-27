"""plotdelta.py"""


from __future__ import print_function
import numpy as np
from ...visualization.myplot import MyPlot


class PlotDelta(object):
    """Plotting a density contrast field"""

    def __init__(self, addr):
        """Initializing the object by loading the density file"""
        self.delta = np.loadtxt(addr)

    def plot(self, output):
        """Plotting the density contrast field"""
        myplot = MyPlot()
        myplot.density(self.delta)
        myplot.save(output)
