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

        self.pos = np.genfromtxt(path, skip_header=7, dtype=[('x', np.float32),
                                                             ('y', np.float32),
                                                             ('z', np.float32)])

        # Convert from kpc to Mpc
        self.pos['x'] /= 1000
        self.pos['y'] /= 1000
        self.pos['z'] /= 1000

        self.myplot = MyPlot()

    def plot(self, path):
        """Plotting a scatter3d of the data alongside with 3 different scatters
        of different planes

        Parameters
        ----------
        path : str
            path of the output plot
        """

        self.myplot.plt.subplots(nrows=2, ncols=2)

        primarycolor = primarycolors('RAINBOW')
        color1 = primarycolor.next()
        color2 = primarycolor.next()

        self._plot3dscatter(color1, color2)
        self._plot2dscatter(color1, color2, pos="222", xlabel='x', ylabel='y',
                            xtag='x', ytag='y')
        self._plot2dscatter(color1, color2, pos="223", xlabel='y', ylabel='z',
                            xtag='y', ytag='z')
        self._plot2dscatter(color1, color2, pos="224", xlabel='x', ylabel='z',
                            xtag='x', ytag='z')

        self.myplot.plt.tight_layout(pad=2.5)

        self.myplot.save(path)

    def _plot3dscatter(self, color1, color2):
        """plot scatter3d part of the figure"""

        axes = self.myplot.new3daxes(pos="221")

        num_p = self.header['pri_halo_num_p']

        kws = {}
        kws['xlabel'] = 'x'
        kws['ylabel'] = 'y'
        kws['zlabel'] = 'z'
        kws['color'] = color1

        lim = self._getarea()
        kws['xmin'] = lim['x'][0]
        kws['xmax'] = lim['x'][1]

        kws['ymin'] = lim['y'][0]
        kws['ymax'] = lim['y'][1]

        kws['zmin'] = lim['z'][0]
        kws['zmax'] = lim['z'][1]

        self.myplot.scatter3d(self.pos['x'][:num_p],
                              self.pos['y'][:num_p],
                              self.pos['z'][:num_p],
                              axes, **dict(kws))

        kws['color'] = color2
        self.myplot.scatter3d(self.pos['x'][num_p:],
                              self.pos['y'][num_p:],
                              self.pos['z'][num_p:],
                              axes, **dict(kws))

    def _plot2dscatter(self, color1, color2, pos='111', xlabel='xlabel',
                       ylabel='ylabel', xtag='x', ytag='y'):
        """plotting the scatter plane"""

        num_p = self.header['pri_halo_num_p']

        kws = {}

        kws['xlabel'] = xlabel
        kws['ylabel'] = ylabel
        kws['color'] = color1

        lim = self._getarea()
        kws['xmin'] = lim[xtag][0]
        kws['xmax'] = lim[xtag][1]

        kws['ymin'] = lim[ytag][0]
        kws['ymax'] = lim[ytag][1]

        self.myplot.scatter(self.pos[xtag][:num_p], self.pos[ytag][:num_p],
                            pos=pos, **dict(kws))

        kws['color'] = color2
        self.myplot.scatter(self.pos[xtag][num_p:], self.pos[ytag][num_p:],
                            pos=pos, **dict(kws))


    def _getarea(self):
        """Returning area of the 2d plots"""
        xlim = (np.min(self.pos['x']) - 1, np.max(self.pos['x']) + 1)
        ylim = (np.min(self.pos['y']) - 1, np.max(self.pos['y']) + 1)
        zlim = (np.min(self.pos['z']) - 1, np.max(self.pos['z']) + 1)

        return {'x': xlim, 'y': ylim, 'z': zlim}



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
