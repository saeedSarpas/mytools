"""plotmatchinghalo.py
Generating scatter plots of a given matching halos
"""

from __future__ import print_function

import numpy as np

from ...visualization.myplot import MyPlot
from ...visualization.mycolordict import primarycolors


class PlotMatchingHalo(object):
    """PlotMatchingHalo class!"""

    def __init__(self, path):
        """Initializing PlotMatchingHalo class and loading the match file

        Parameters
        ----------
        path : str
            path to the match file

        Examples
        --------
        >>> from mytools.examples.rockstargadgetmatchfinder.plotmatchinghalo import PlotMatchingHalo as PMH
        >>> pmh = PMH('/path/to/match/file')
        >>> pmh.save('/path/to/output/plot/with/extension')
        """
        self.header = {}

        with open(path) as _file:
            for i, line in enumerate(_file):
                if i > 6: break
                keyvalue = line.split(':')
                self.header[keyvalue[0]] = _2number(keyvalue[1].strip('\n'))

        dtype = [('x', np.float32), ('y', np.float32), ('z', np.float32)]
        num_p_pri = self.header['pri_halo_num_p']
        num_p_sec = self.header['sec_halo_num_p']

        self.priinitsnap = np.genfromtxt(path, skip_header=7,
                                         max_rows=num_p_pri, dtype=dtype)
        self.secinitsnap = np.genfromtxt(path, skip_header=7+num_p_pri,
                                         max_rows=num_p_sec, dtype=dtype)

        self.prilatestsnap = np.genfromtxt(
            path, skip_header=7+num_p_pri+num_p_sec,
            max_rows=num_p_pri, dtype=dtype)
        self.seclatestsnap = np.genfromtxt(
            path, skip_header=7+(2*num_p_pri)+num_p_sec,
            max_rows=num_p_sec, dtype=dtype)

        # Convert from kpc to Mpc
        for axis in ['x', 'y', 'z']:
            self.priinitsnap[axis] /= 1000
            self.secinitsnap[axis] /= 1000
            self.prilatestsnap[axis] /= 1000
            self.seclatestsnap[axis] /= 1000

        self.myplot = MyPlot(aspect=0.48)

    def plot(self, path):
        """Plotting a scatter3d of the data alongside with 3 different scatters
        of different planes

        Parameters
        ----------
        path : str
            path of the output plot
        """

        # self.myplot.plt.subplots(nrows=2, ncols=4)

        primarycolor = primarycolors('RAINBOW')
        color1 = primarycolor.next()
        color2 = primarycolor.next()

        self._init3dscatter(color1, color2, pos="241")

        self._init2dscatter(color1, color2, pos="242", xlabel='x', ylabel='y',
                            xtag='x', ytag='y')
        self._init2dscatter(color1, color2, pos="245", xlabel='y', ylabel='z',
                            xtag='y', ytag='z')
        self._init2dscatter(color1, color2, pos="246", xlabel='x', ylabel='z',
                            xtag='x', ytag='z')

        self._latest3dscatter(color1, color2, pos="122")

        self.myplot.plt.tight_layout(pad=2.5)

        self.myplot.save(path)

    def _init3dscatter(self, color1, color2, pos='111'):
        """plot scatter3d part of the figure"""

        axes = self.myplot.new3daxes(pos=pos)

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = 'x'
        kws['ylabel'] = 'y'
        kws['zlabel'] = 'z'
        kws['alpha'] = 1.0
        kws['color'] = color1

        lim = self._getarea()
        kws['xmin'] = lim['x'][0]
        kws['xmax'] = lim['x'][1]

        kws['ymin'] = lim['y'][0]
        kws['ymax'] = lim['y'][1]

        kws['zmin'] = lim['z'][0]
        kws['zmax'] = lim['z'][1]

        self.myplot.scatter3d(self.priinitsnap['x'],
                              self.priinitsnap['y'],
                              self.priinitsnap['z'],
                              axes, **dict(kws))

        kws['color'] = color2
        self.myplot.scatter3d(self.secinitsnap['x'],
                              self.secinitsnap['y'],
                              self.secinitsnap['z'],
                              axes, **dict(kws))

    def _latest3dscatter(self, color1, color2, pos='111'):
        """plot scatter3d part of the figure"""

        axes = self.myplot.new3daxes(pos=pos)

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = 'x'
        kws['ylabel'] = 'y'
        kws['zlabel'] = 'z'
        kws['alpha'] = 1.0
        kws['color'] = color1

        lim = self._getarea()
        kws['xmin'] = lim['x'][0]
        kws['xmax'] = lim['x'][1]

        kws['ymin'] = lim['y'][0]
        kws['ymax'] = lim['y'][1]

        kws['zmin'] = lim['z'][0]
        kws['zmax'] = lim['z'][1]

        self.myplot.scatter3d(self.prilatestsnap['x'],
                              self.prilatestsnap['y'],
                              self.prilatestsnap['z'],
                              axes, **dict(kws))

        kws['color'] = color2
        self.myplot.scatter3d(self.seclatestsnap['x'],
                              self.seclatestsnap['y'],
                              self.seclatestsnap['z'],
                              axes, **dict(kws))

    def _init2dscatter(self, color1, color2, pos='111', xlabel='xlabel',
                       ylabel='ylabel', xtag='x', ytag='y'):
        """plotting the scatter plane"""

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = xlabel
        kws['ylabel'] = ylabel
        kws['alpha'] = 1.0
        kws['color'] = color1

        lim = self._getarea()
        kws['xmin'] = lim[xtag][0]
        kws['xmax'] = lim[xtag][1]

        kws['ymin'] = lim[ytag][0]
        kws['ymax'] = lim[ytag][1]

        self.myplot.scatter(self.priinitsnap[xtag], self.priinitsnap[ytag],
                            pos=pos, **dict(kws))

        kws['color'] = color2
        self.myplot.scatter(self.secinitsnap[xtag], self.secinitsnap[ytag],
                            pos=pos, **dict(kws))

    def _getarea(self):
        """Returning area of the 2d plots"""
        lim = {'x': [], 'y': [], 'z': []}

        for axis in ['x', 'y', 'z']:
            lim[axis].append(np.min((np.min(self.priinitsnap[axis]),
                                     np.min(self.prilatestsnap[axis]))) - 1)

            lim[axis].append(np.max((np.max(self.priinitsnap[axis]),
                                     np.max(self.prilatestsnap[axis]))) + 1)

        return lim


def _2number(string):
    """Converting a string to int or float

    Parameters
    ----------
    string : str
        A string for converting to int or float

    Returns
    -------
    int or float
    """

    try:
        return int(string)
    except ValueError:
        return float(string)
