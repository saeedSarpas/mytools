"""plotmatches.py
Generating scatter plots of a given matching halos
"""

from __future__ import print_function

import numpy as np

from ...visualization.myplot import MyPlot
from ...visualization.mycolordict import primarycolors, primaryshadows


class PlotMatches(object):
    """PlotMatchingHalo class!"""

    def __init__(self, path):
        """Initializing PlotMatches class and loading the match file

        Parameters
        ----------
        path : str
            path to the match file

        Examples
        --------
        >>> from mytools.examples.rg_matchfinder.plotmatches \
        import PlotMatches as PM
        >>> pm = PM('/path/to/match/file')
        >>> pm.plot('/path/to/output/plot/with/extension')
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

        self.num_p = {'pri': num_p_pri, 'sec': num_p_sec}

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

        self._init3dscatter('RAINBOW', pos="241")

        self._init2dscatter('RAINBOW', pos="242", xlabel='x', ylabel='y',
                            xtag='x', ytag='y')
        self._init2dscatter('RAINBOW', pos="245", xlabel='y', ylabel='z',
                            xtag='y', ytag='z')
        self._init2dscatter('RAINBOW', pos="246", xlabel='x', ylabel='z',
                            xtag='x', ytag='z')

        self._latest3dscatter('RAINBOW', pos="122")

        self.myplot.plt.tight_layout(pad=2.5)

        self.myplot.save(path)
        self.myplot.plt.clf()
        self.myplot.plt.close()

    def _init3dscatter(self, scheme, pos='111'):
        """plot scatter3d part of the figure"""

        ax = self.myplot.new3daxes(pos=pos)

        primarycolor = primarycolors(scheme)
        color1 = primarycolor.next()
        color2 = primarycolor.next()

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = 'x'
        kws['ylabel'] = 'y'
        kws['zlabel'] = 'z'
        kws['alpha'] = 1.0
        kws['color'] = color1

        lim = self._getarea(which='init')
        kws['xmin'] = lim['x'][0]
        kws['xmax'] = lim['x'][1]

        kws['ymin'] = lim['y'][0]
        kws['ymax'] = lim['y'][1]

        kws['zmin'] = lim['z'][0]
        kws['zmax'] = lim['z'][1]

        self.myplot.scatter3d(self.priinitsnap['x'],
                              self.priinitsnap['y'],
                              self.priinitsnap['z'],
                              ax, s=40, **dict(kws))

        kws['color'] = color2
        self.myplot.scatter3d(self.secinitsnap['x'],
                              self.secinitsnap['y'],
                              self.secinitsnap['z'],
                              ax, s=10, **dict(kws))

    def _latest3dscatter(self, scheme, pos='111'):
        """plot scatter3d part of the figure"""

        ax = self.myplot.new3daxes(pos=pos)

        primarycolor = primarycolors(scheme)
        color1 = primarycolor.next()
        color2 = primarycolor.next()

        primaryshadow = primaryshadows(scheme)
        shadow1 = primaryshadow.next()
        shadow2 = primaryshadow.next()

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = 'x'
        kws['ylabel'] = 'y'
        kws['zlabel'] = 'z'

        lim = self._getarea(which='final')
        kws['xmin'] = lim['x'][0]
        kws['xmax'] = lim['x'][1]

        kws['ymin'] = lim['y'][0]
        kws['ymax'] = lim['y'][1]

        kws['zmin'] = lim['z'][0]
        kws['zmax'] = lim['z'][1]

        kws['alpha'] = 1.0
        kws['color'] = color1
        self.myplot.scatter3d(self.prilatestsnap['x'],
                              self.prilatestsnap['y'],
                              self.prilatestsnap['z'],
                              ax, s=40, **dict(kws))

        kws['alpha'] = 0.3
        kws['color'] = shadow1
        self.myplot.scatter3d(self.prilatestsnap['x'],
                              self.prilatestsnap['y'],
                              [lim['z'][0]] * self.num_p['pri'],
                              ax, s=40, **dict(kws))

        self.myplot.scatter3d(self.prilatestsnap['x'],
                              [lim['y'][1]] * self.num_p['pri'],
                              self.prilatestsnap['z'],
                              ax, s=40, **dict(kws))

        self.myplot.scatter3d([lim['x'][0]] * self.num_p['pri'],
                              self.prilatestsnap['y'],
                              self.prilatestsnap['z'],
                              ax, s=40, **dict(kws))

        kws['alpha'] = 1.0
        kws['color'] = color2
        self.myplot.scatter3d(self.seclatestsnap['x'],
                              self.seclatestsnap['y'],
                              self.seclatestsnap['z'],
                              ax, s=10, **dict(kws))

        kws['alpha'] = 0.3
        kws['color'] = shadow2
        self.myplot.scatter3d(self.seclatestsnap['x'],
                              self.seclatestsnap['y'],
                              [lim['z'][0]] * self.num_p['sec'],
                              ax, s=10, **dict(kws))

        self.myplot.scatter3d(self.seclatestsnap['x'],
                              [lim['y'][1]] * self.num_p['sec'],
                              self.seclatestsnap['z'],
                              ax, s=10, **dict(kws))

        self.myplot.scatter3d([lim['x'][0]] * self.num_p['sec'],
                              self.seclatestsnap['y'],
                              self.seclatestsnap['z'],
                              ax, s=10, **dict(kws))


    def _init2dscatter(self, scheme, pos='111', xlabel='xlabel',
                       ylabel='ylabel', xtag='x', ytag='y'):
        """plotting the scatter plane"""

        primarycolor = primarycolors(scheme)
        color1 = primarycolor.next()
        color2 = primarycolor.next()

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = xlabel
        kws['ylabel'] = ylabel
        kws['alpha'] = 1.0
        kws['color'] = color1

        lim = self._getarea(which='init')
        kws['xmin'] = lim[xtag][0]
        kws['xmax'] = lim[xtag][1]

        kws['ymin'] = lim[ytag][0]
        kws['ymax'] = lim[ytag][1]

        self.myplot.scatter(self.priinitsnap[xtag], self.priinitsnap[ytag],
                            pos=pos, s=40, **dict(kws))

        kws['color'] = color2
        self.myplot.scatter(self.secinitsnap[xtag], self.secinitsnap[ytag],
                            pos=pos, s=10, **dict(kws))

    def _getarea(self, which='init'):
        """Returning area of the 2d plots

        Parameters
        ----------
        which : str, optional
            which snaphot ('init' or 'final') should be used for calculating
            the limits
        """
        lim = {'x': [], 'y': [], 'z': []}

        for axis in ['x', 'y', 'z']:
            if which == 'init':
                lim[axis].append(np.min((np.min(self.priinitsnap[axis]),
                                         np.min(self.secinitsnap[axis]))))
                lim[axis].append(np.max((np.max(self.priinitsnap[axis]),
                                         np.max(self.secinitsnap[axis]))))
            elif which == 'final':
                lim[axis].append(np.min((np.min(self.prilatestsnap[axis]),
                                         np.min(self.seclatestsnap[axis]))))
                lim[axis].append(np.max((np.max(self.prilatestsnap[axis]),
                                         np.max(self.seclatestsnap[axis]))))

        dist = np.max((lim['x'][1]-lim['x'][0],
                       lim['y'][1]-lim['y'][0],
                       lim['z'][1]-lim['z'][0]))

        dist *= 1.25 if which == 'init' else 1.75

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
