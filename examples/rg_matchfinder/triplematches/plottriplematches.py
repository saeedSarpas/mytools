"""plot-singlematches.py
Generating scatter plots of a given matching halos
"""

from __future__ import print_function

import numpy as np

from ....visualization.myplot import MyPlot
from ....visualization.mycolordict import primarycolors, primaryshadows


class PlotSingleMatches(object):
    """PlotMatchingHalo class!"""

    def __init__(self, path, aspect=0.48):
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


    def plotsinglehalo(self, path):
        """Plotting dark matter halo"""

        primarycolor = primarycolors('RAINBOW')
        _ = primarycolor.next()
        _ = primarycolor.next()
        color3 = primarycolor.next()

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = 'x'
        kws['ylabel'] = 'y'
        kws['alpha'] = 1.0

        lim = self._getarea(which='final')
        kws['xmin'] = lim['x'][0]
        kws['xmax'] = lim['x'][1]

        kws['ymin'] = lim['y'][0]
        kws['ymax'] = lim['y'][1]

        kws['color'] = color3
        self.myplot.scatter(self.triSnap['x'], self.triSnap['y'],
                            pos='111', s=1, **dict(kws))

        self.myplot.save(path, dpi=720)



    def plottrace(self, path):
        """Tracing back halo particles to their initial positions

        TODO: This is a disaster, not only this method, but the whole module
        (like these days) I should definitely make this module more modular
        at least by using inheritance!
        """

        ax = self.myplot.new3daxes()

        primarycolor = primarycolors('RAINBOW')
        color1 = primarycolor.next()
        color2 = primarycolor.next()

        primaryshadow = primaryshadows('RAINBOW')
        shadow1 = primaryshadow.next()
        shadow2 = primaryshadow.next()

        a = {'ic': 0.01960784313, 'snap': 1.0}

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = 'x'
        kws['ylabel'] = 'y'
        kws['zlabel'] = '$a$'
        kws['alpha'] = 1.0

        liminit = self._getarea(which='init')
        limfinal = self._getarea(which='final')

        kws['xmin'] = min(liminit['x'][0], limfinal['x'][0])
        kws['xmax'] = max(liminit['x'][1], limfinal['x'][1])

        kws['ymin'] = min(liminit['y'][0], limfinal['y'][0])
        kws['ymax'] = max(liminit['y'][1], limfinal['y'][1])

        kws['zmin'] = 0.0
        kws['zmax'] = 1.0

        # Primary
        kws['color'] = shadow1
        kws['alpha'] = 0.75
        self.myplot.scatter3d(self.priIc['x'],
                              self.priIc['y'],
                              [a['ic']] * self.num_p['pri'],
                              ax, s=48, **dict(kws))

        kws['color'] = color1
        kws['alpha'] = 0.75
        self.myplot.scatter3d(self.priSnap['x'],
                              self.priSnap['y'],
                              [a['snap']] * self.num_p['pri'],
                              ax, s=48, **dict(kws))

        kws['color'] = shadow1
        kws['alpha'] = 0.125
        length = min(1000, self.num_p['pri'])
        for i in range(length):
            plotparams = {
                'x': (self.priIc['x'][i], self.priSnap['x'][i]),
                'y': (self.priIc['y'][i], self.priSnap['y'][i]),
                'z': (a['ic'], a['snap'] - 0.02)
            }
            self.myplot.plot3d(plotparams, ax=ax, **dict(kws))

        ax.w_xaxis.set_pane_color((1.0, 1.0, 1.0, 0.0))
        ax.w_yaxis.set_pane_color((1.0, 1.0, 1.0, 0.0))

        # for ii in xrange(0,360,1):
        ii = 320
        ax.view_init(elev=10., azim=ii)
        self.myplot.save(path + ('-256-%03d' % ii) + '.png', dpi=120)
        self.myplot.plt.cla()

        # Secondary
        kws['color'] = shadow2
        kws['alpha'] = 0.75
        self.myplot.scatter3d(self.secIc['x'],
                              self.secIc['y'],
                              [a['ic']] * self.num_p['sec'],
                              ax, s=8, **dict(kws))

        kws['color'] = color2
        kws['alpha'] = 0.75
        self.myplot.scatter3d(self.secSnap['x'],
                              self.secSnap['y'],
                              [a['snap']] * self.num_p['sec'],
                              ax, s=8, **dict(kws))

        kws['color'] = shadow2
        kws['alpha'] = 0.25
        length = min(1000, self.num_p['sec'])


        for i in range(length):
            plotparams = {
                'x': (self.secIc['x'][i], self.secSnap['x'][i]),
                'y': (self.secIc['y'][i], self.secSnap['y'][i]),
                'z': (a['ic'], a['snap'] - 0.02)
            }
            self.myplot.plot3d(plotparams, ax=ax, **dict(kws))

        ax.w_xaxis.set_pane_color((1.0, 1.0, 1.0, 0.0))
        ax.w_yaxis.set_pane_color((1.0, 1.0, 1.0, 0.0))

        # for ii in xrange(0,360,2):
        ii = 320
        ax.view_init(elev=10., azim=ii)
        self.myplot.save(path + ('-512-%03d' % ii) + '.png', dpi=120)
        self.myplot.plt.cla()
        self.myplot.plt.clf()
        self.myplot.plt.close()

    def plotictraces(self, path):
        # Plotting 2d ICs
        mmm = MyPlot(aspect=1)
        ax2d = mmm.new2daxes()

        primaryshadow = primaryshadows('RAINBOW')
        shadow1 = primaryshadow.next()
        shadow2 = primaryshadow.next()

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = 'x'
        kws['ylabel'] = 'y'
        kws['zlabel'] = '$a$'
        kws['alpha'] = 1.0

        liminit = self._getarea(which='init')
        limfinal = self._getarea(which='final')

        kws['xmin'] = min(liminit['x'][0], limfinal['x'][0])
        kws['xmax'] = max(liminit['x'][1], limfinal['x'][1])

        kws['ymin'] = min(liminit['y'][0], limfinal['y'][0])
        kws['ymax'] = max(liminit['y'][1], limfinal['y'][1])

        kws['color'] = shadow1
        kws['alpha'] = 1
        mmm.scatter(self.priIc['x'], self.priIc['y'],
                            ax=ax2d, s=48, **dict(kws))
        mmm.save(path + '-256-ic' + '.png')
        mmm.plt.cla()

        kws['color'] = shadow2
        kws['alpha'] = 1
        mmm.scatter(self.secIc['x'], self.secIc['y'],
                            ax=ax2d, s=8, **dict(kws))
        mmm.save(path + '-512-ic' + '.png')
        mmm.plt.cla()

    def _init3dscatter(self, scheme, pos='111'):
        """plot scatter3d part of the figure"""

        ax = self.myplot.new3daxes(pos=pos)

        primarycolor = primarycolors(scheme)
        color1 = primarycolor.next()
        color2 = primarycolor.next()
        color3 = primarycolor.next()

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = 'x'
        kws['ylabel'] = 'y'
        kws['zlabel'] = 'z'
        kws['alpha'] = 1.0

        lim = self._getarea(which='init')
        kws['xmin'] = lim['x'][0]
        kws['xmax'] = lim['x'][1]

        kws['ymin'] = lim['y'][0]
        kws['ymax'] = lim['y'][1]

        kws['zmin'] = lim['z'][0]
        kws['zmax'] = lim['z'][1]

        kws['color'] = color1
        self.myplot.scatter3d(self.priIc['x'],
                              self.priIc['y'],
                              self.priIc['z'],
                              ax, s=48, **dict(kws))

        kws['color'] = color2
        self.myplot.scatter3d(self.secIc['x'],
                              self.secIc['y'],
                              self.secIc['z'],
                              ax, s=8, **dict(kws))

        kws['color'] = color3
        self.myplot.scatter3d(self.triIc['x'],
                              self.triIc['y'],
                              self.triIc['z'],
                              ax, s=2, **dict(kws))

    def _latest3dscatter(self, scheme, pos='111'):
        """plot scatter3d part of the figure"""

        ax = self.myplot.new3daxes(pos=pos)

        primarycolor = primarycolors(scheme)
        color1 = primarycolor.next()
        color2 = primarycolor.next()
        color3 = primarycolor.next()

        primaryshadow = primaryshadows(scheme)
        shadow1 = primaryshadow.next()
        shadow2 = primaryshadow.next()
        shadow3 = primaryshadow.next()

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
        self.myplot.scatter3d(self.priSnap['x'],
                              self.priSnap['y'],
                              self.priSnap['z'],
                              ax, s=48, **dict(kws))

        kws['alpha'] = 0.25
        kws['color'] = shadow1
        self.myplot.scatter3d(self.priSnap['x'],
                              self.priSnap['y'],
                              [lim['z'][0]] * self.num_p['pri'],
                              ax, s=48, **dict(kws))

        self.myplot.scatter3d(self.priSnap['x'],
                              [lim['y'][1]] * self.num_p['pri'],
                              self.priSnap['z'],
                              ax, s=48, **dict(kws))

        self.myplot.scatter3d([lim['x'][0]] * self.num_p['pri'],
                              self.priSnap['y'],
                              self.priSnap['z'],
                              ax, s=48, **dict(kws))

        kws['alpha'] = 1.0
        kws['color'] = color2
        self.myplot.scatter3d(self.secSnap['x'],
                              self.secSnap['y'],
                              self.secSnap['z'],
                              ax, s=8, **dict(kws))

        kws['alpha'] = 0.25
        kws['color'] = shadow2
        self.myplot.scatter3d(self.secSnap['x'],
                              self.secSnap['y'],
                              [lim['z'][0]] * self.num_p['sec'],
                              ax, s=8, **dict(kws))

        self.myplot.scatter3d(self.secSnap['x'],
                              [lim['y'][1]] * self.num_p['sec'],
                              self.secSnap['z'],
                              ax, s=8, **dict(kws))

        self.myplot.scatter3d([lim['x'][0]] * self.num_p['sec'],
                              self.secSnap['y'],
                              self.secSnap['z'],
                              ax, s=8, **dict(kws))

        kws['alpha'] = 1.0
        kws['color'] = color3
        self.myplot.scatter3d(self.triSnap['x'],
                              self.triSnap['y'],
                              self.triSnap['z'],
                              ax, s=2, **dict(kws))

        kws['alpha'] = 0.25
        kws['color'] = shadow3
        self.myplot.scatter3d(self.triSnap['x'],
                              self.triSnap['y'],
                              [lim['z'][0]] * self.num_p['tri'],
                              ax, s=2, **dict(kws))

        self.myplot.scatter3d(self.triSnap['x'],
                              [lim['y'][1]] * self.num_p['tri'],
                              self.triSnap['z'],
                              ax, s=2, **dict(kws))

        self.myplot.scatter3d([lim['x'][0]] * self.num_p['tri'],
                              self.triSnap['y'],
                              self.triSnap['z'],
                              ax, s=2, **dict(kws))


    def _init2dscatter(self, scheme, pos='111', xlabel='xlabel',
                       ylabel='ylabel', xtag='x', ytag='y'):
        """plotting the scatter plane"""

        primarycolor = primarycolors(scheme)
        color1 = primarycolor.next()
        color2 = primarycolor.next()
        color3 = primarycolor.next()

        kws = {}
        kws['silent'] = True
        kws['xlabel'] = xlabel
        kws['ylabel'] = ylabel
        kws['alpha'] = 1.0

        lim = self._getarea(which='init')
        kws['xmin'] = lim[xtag][0]
        kws['xmax'] = lim[xtag][1]

        kws['ymin'] = lim[ytag][0]
        kws['ymax'] = lim[ytag][1]

        kws['color'] = color1
        self.myplot.scatter(self.priIc[xtag], self.priIc[ytag],
                            pos=pos, s=48, **dict(kws))

        kws['color'] = color2
        self.myplot.scatter(self.secIc[xtag], self.secIc[ytag],
                            pos=pos, s=8, **dict(kws))

        kws['color'] = color3
        self.myplot.scatter(self.triIc[xtag], self.triIc[ytag],
                            pos=pos, s=2, **dict(kws))

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
                lim[axis].append(np.min((np.min(self.priIc[axis]),
                                         np.min(self.secIc[axis]),
                                         np.min(self.triIc[axis]))))
                lim[axis].append(np.max((np.max(self.priIc[axis]),
                                         np.max(self.secIc[axis]),
                                         np.max(self.triIc[axis]))))
            elif which == 'final':
                lim[axis].append(np.min((np.min(self.priSnap[axis]),
                                         np.min(self.secSnap[axis]),
                                         np.min(self.triSnap[axis]))))
                lim[axis].append(np.max((np.max(self.priSnap[axis]),
                                         np.max(self.secSnap[axis]),
                                         np.max(self.triSnap[axis]))))

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
