"""plot-singlematches.py
Generating scatter plots of a given matching halos
"""

from __future__ import print_function

import numpy as np

from ....visualization.myplot import MyPlot
from ....visualization.mycolordict import primarycolors, primaryshadows, CDICT


class PlotTriplesAlone(object):
    """PlotMatchingHalo class!"""

    def __init__(self, path, aspect=1.0):
        """Initializing PlotMatchingHalo class and loading the match file

        Parameters
        ----------
        path : str
            path to the match file
        aspect : float
            Aspect ratio of the plot

        Examples
        --------
        >>> from mytools.examples.rg_matchfinder.triplematches.plottriplematches \
        import PlotSingleMatches as PSM
        >>> psm = PSM('/path/to/match/file')
        >>> psm.save('/path/to/output/plot/with/extension')
        """
        self.header = {}

        with open(path) as _file:
            for i, line in enumerate(_file):
                if i > 10: break
                keyvalue = line.split(':')
                self.header[keyvalue[0]] = _2number(keyvalue[1].strip('\n'))

        dtype = [('x', np.float32), ('y', np.float32), ('z', np.float32)]
        num_p_pri = self.header['pri_halo_num_p']
        num_p_sec = self.header['sec_halo_num_p']
        num_p_tri = self.header['tri_halo_num_p']

        self.num_p = {'pri': num_p_pri, 'sec': num_p_sec, 'tri': num_p_tri}

        self.priIc = np.genfromtxt(path, skip_header=11,
                                   max_rows=num_p_pri, dtype=dtype)
        self.secIc = np.genfromtxt(path, skip_header=11+num_p_pri,
                                   max_rows=num_p_sec, dtype=dtype)
        self.triIc = np.genfromtxt(path, skip_header=11+num_p_pri+num_p_sec,
                                   max_rows=num_p_tri, dtype=dtype)

        self.priSnap = np.genfromtxt(
            path, skip_header=11+num_p_pri+num_p_sec+num_p_tri,
            max_rows=num_p_pri, dtype=dtype)
        self.secSnap = np.genfromtxt(
            path, skip_header=11+(2*num_p_pri)+num_p_sec+num_p_tri,
            max_rows=num_p_sec, dtype=dtype)
        self.triSnap = np.genfromtxt(
            path, skip_header=11+(2*num_p_pri)+(2*num_p_sec)+num_p_tri,
            max_rows=num_p_tri, dtype=dtype)

        # Convert from kpc to Mpc
        for axis in ['x', 'y', 'z']:
            self.priIc[axis] /= 1000
            self.secIc[axis] /= 1000
            self.triIc[axis] /= 1000
            self.priSnap[axis] /= 1000
            self.secSnap[axis] /= 1000
            self.triSnap[axis] /= 1000

        self.myplot = MyPlot(aspect=aspect)


    def plot(self, path):
        """Plotting a scatter3d of the data alongside with 3 different scatters
        of different planes

        Parameters
        ----------
        path : str
            path of the output plot
        """

        self._scatter(path + '-init-',
                      self.priIc, self.secIc, self.triIc,
                      xlabel='x', ylabel='y', xtag='x', ytag='y')
        self._scatter(path + '-init-',
                      self.priIc, self.secIc, self.triIc,
                      xlabel='y', ylabel='z', xtag='y', ytag='z')
        self._scatter(path + '-init-',
                      self.priIc, self.secIc, self.triIc,
                      xlabel='x', ylabel='z', xtag='x', ytag='z')

        self._scatter(path + '-final-',
                      self.priSnap, self.secSnap, self.triSnap,
                      xlabel='x', ylabel='y', xtag='x', ytag='y')
        self._scatter(path + '-final-',
                      self.priSnap, self.secSnap, self.triSnap,
                      xlabel='y', ylabel='z', xtag='y', ytag='z')
        self._scatter(path + '-final-',
                      self.priSnap, self.secSnap, self.triSnap,
                      xlabel='x', ylabel='z', xtag='x', ytag='z')



    def _scatter(self, path, pri, sec, tri, xlabel='xlabel', ylabel='ylabel',
                       xtag='x', ytag='y'):
        """plotting the scatter plane"""

        color1 = CDICT['RAINBOW']['primary_colors'][0]
        color2 = CDICT['RAINBOW']['primary_colors'][1]
        color3 = CDICT['RAINBOW']['primary_colors'][2]

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = xlabel
        kws['ylabel'] = ylabel
        kws['alpha'] = 1.0

        lim = _getarea(pri, sec, tri)
        kws['xmin'] = lim[xtag][0]
        kws['xmax'] = lim[xtag][1]

        kws['ymin'] = lim[ytag][0]
        kws['ymax'] = lim[ytag][1]

        kws['color'] = color1
        self.myplot.scatter(pri[xtag], pri[ytag], s=48, **dict(kws))

        kws['color'] = color2
        self.myplot.scatter(sec[xtag], sec[ytag], s=8, **dict(kws))

        kws['color'] = color3
        self.myplot.scatter(tri[xtag], tri[ytag], s=2, **dict(kws))

        output = path + xtag + '-' + ytag
        self.myplot.save(output + '.eps')
        self.myplot.save(output + '.png')
        self.myplot.plt.cla()


def _getarea(pri, sec, tri):
    """Returning area of the 2d plots

    Parameters
    ----------
    which : str, optional
        which snaphot ('init' or 'final') should be used for calculating
        the limits
    """
    lim = {'x': [], 'y': [], 'z': []}

    for axis in ['x', 'y', 'z']:
        lim[axis].append(np.min((np.min(pri[axis]),
                                    np.min(sec[axis]),
                                    np.min(tri[axis]))))
        lim[axis].append(np.max((np.max(pri[axis]),
                                    np.max(sec[axis]),
                                    np.max(tri[axis]))))

    dist = np.max((lim['x'][1]-lim['x'][0],
                    lim['y'][1]-lim['y'][0],
                    lim['z'][1]-lim['z'][0]))

    dist *= 1.5

    for axis in ['x', 'y', 'z']:
        ofs = (dist - (lim[axis][1]-lim[axis][0])) / 2
        lim[axis][0] = lim[axis][0]-ofs if lim[axis][0]-ofs > 0 else 0
        lim[axis][1] = lim[axis][1]+ofs if lim[axis][1]+ofs < 100 else 100

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
