"""myplot.py """

from __future__ import print_function
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import numpy as np
from ..visualization import mycolordict as cdict
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.ticker import NullFormatter


class MyPlot(object):
    """MyPlot class"""

    def __init__(self):
        """Constructor of MyPlot"""
        self.plt = plt
        self.plt.figure(1)

    def errorbar(self, plotparams, pos="111", **kwargs):
        """Plotting with error bars

        Parameters
        ----------
        plotparams : dict
            A dictionary containing these keys: 'x' : list, 'y' : list,
            'xerr' : int or list, 'yerr' : int or list
        pos : str, optional
            Position of the subplot inside the figure
        scheme : str, optional
            Color scheme name
        label : str, optional
            Plot label
        color : str, optional
            Color of the plot
        ecolor : str, optional
            Color of the error bars
        shaded : bool, optional
            Shadow under the y-error bars
        linestyle : str, optional
            Style of the line, e.g., 'solid'
        xscale, yscale : str, optional
            Scale of the axis, e.g., 'log', 'linear'
        xmin, xmax, ymin, ymax : float, optional
            Axis boundries
        xlabel, ylabel : str, optional
            Axis labels

        Examples
        --------
        >>> from mytools.visualization.plot import MyPlot
        >>> points = {
        ...     'x': range(1, 100),
        ...     'y': range(1, 100),
        ...     'xerr': 1,
        ...     'yerr': 1}
        >>> MyPlot.errorbar(points, lable='Plot')
        """

        self.plt.subplot(pos)

        if len(dict(kwargs).keys()) > 0:
            print('Plotting using following parameters:')
            for key, value in dict(kwargs).iteritems():
                print('\t {:15s}'.format(str(key)) + str(value))
            print('')

        params = _getparams(**kwargs)

        self._setscales(**kwargs)
        self._setlabels(**kwargs)
        self._setarea(**kwargs)
        self._removetoprightspines()
        self._setaxiscolor(**kwargs)

        self.plt.errorbar(
            plotparams['x'], plotparams['y'],
            xerr=plotparams['xerr'], yerr=plotparams['yerr'],
            color=params['color'],
            ecolor=params['ecolor'],
            linestyle=params['linestyle'],
            label=params['label'])

        if 'shaded' in kwargs and kwargs.get('shaded') is True:
            if 'yerr' in plotparams:
                yyerr = zip(plotparams['y'], plotparams['yerr'])
                yneg = [y - yerr for y, yerr in yyerr]
                ypos = [y + yerr for y, yerr in yyerr]
                self.plt.fill_between(
                    plotparams['x'], yneg, ypos,
                    facecolor=cdict.get(params['scheme'])['facecolor'],
                    edgecolor=cdict.get(params['scheme'])['facecolor'],
                    interpolate=True)


    def plot(self, plotparams, pos="111", **kwargs):
        """plot

        Parameters
        ----------
        plotparams : dict
            A dictionary containing these keys: 'x' : list, 'y' : list
        pos : str, optional
            Position of the subplot inside the figure
        scheme : str, optional
            Color scheme name
        label : str, optional
            Plot label
        color : str, optional
            Color of the plot
        ecolor : str, optional
            Color of the error bars
        shaded : bool, optional
            Shadow under the y-error bars
        linestyle : str, optional
            Style of the line, e.g., 'solid'
        xscale, yscale : str, optional
            Scale of the axis, e.g., 'log', 'linear'
        xmin, xmax, ymin, ymax : float, optional
            Axis boundries
        xlabel, ylabel : str, optional
            Axis labels

        Examples
        --------
        >>> from mytools.visualization.plot import MyPlot
        >>> points = {
        ...     'x': range(1, 100),
        ...     'y': range(1, 100)}
        >>> MyPlot.plot(points, lable='Plot')
        """

        self.plt.subplot(pos)

        params = _getparams(**kwargs)

        if len(dict(kwargs).keys()) > 0:
            print('Plotting using following parameters:')
            for key, value in dict(kwargs).iteritems():
                print('\t {:15s}'.format(str(key)) + str(value))
            print('')

        self._setscales(**kwargs)
        self._setlabels(**kwargs)
        self._setarea(**kwargs)
        self._removetoprightspines()
        self._setaxiscolor(**kwargs)

        self.plt.plot(
            plotparams['x'], plotparams['y'],
            color=params['color'],
            linestyle=params['linestyle'],
            label=params['label'])


    def density(self, plotdata, pos="111", vmin=None, vmax=None, **kwargs):
        """Density plot

        Parameters:
        ----------
        plotdata : 2D Array
        pos : str, optional
            Position of the subplot inside the figure
        vmin : float or None
            Minumum value of colormapping
        scheme : str, optional
            Color scheme name
        label : str, optional
            Plot label
        xscale, yscale : str, optional
            Scale of the axis, e.g., 'log', 'linear'
        xmin, xmax, ymin, ymax : float, optional
            Axis boundries
        xlabel, ylabel : str, optional
            Axis labels
        """

        self.plt.subplot(pos)

        params = _getparams(**kwargs)

        if vmin is None: vmin = np.amin(plotdata)
        if vmax is None: vmax = np.amax(plotdata)

        if len(dict(kwargs).keys()) > 0:
            print('Plotting using following parameters:')
            for key, value in dict(kwargs).iteritems():
                print('\t {:15s}'.format(str(key)) + str(value))
            print('\t {:15s}'.format('vmin: ' + str(vmin)))
            print('\t {:15s}'.format('vmax: ' + str(vmax)))
            print('')

        self._setscales(**kwargs)
        self._setlabels(**kwargs)
        self._setarea(**kwargs)
        self._removetoprightspines()
        self._setaxiscolor(**kwargs)

        cmap = mcolors.LinearSegmentedColormap(
            'CustomMap', cdict.get(params['scheme'])['cdict'])

        self.plt.imshow(plotdata, cmap=cmap, vmin=vmin, vmax=vmax)


    def scatter3d(self, xs, ys, zs, pos="111", zdir='z', **kwargs):
        """Scatter3d plot

        Parameters:
        ----------
        xs, ys, zs : array of floats
            Positions of the points
        pos : str, optional
            Position of the subplot inside the figure
        zdir : str, optional
        scheme : str, optional
            Color scheme name
        label : str, optional
            Plot label
        xscale, yscale : str, optional
            Scale of the axis, e.g., 'log', 'linear'
        xmin, xmax, ymin, ymax : float, optional
            Axis boundries
        xlabel, ylabel, zlabel : str, optional
            Axis labels
        """

        axes = self.plt.subplot(pos, projection='3d')

        if len(dict(kwargs).keys()) > 0:
            print('Plotting using following parameters:')
            for key, value in dict(kwargs).iteritems():
                print('\t {:15s}'.format(str(key)) + str(value))

        self._setscales(**kwargs)
        self._setlabels(**kwargs)
        self._setarea(**kwargs)

        axes.scatter(xs, ys, zs, zdir=zdir)

        if 'xlabel' in kwargs: axes.set_xlabel(kwargs.get('xlabel'))
        if 'ylabel' in kwargs: axes.set_ylabel(kwargs.get('ylabel'))
        if 'zlabel' in kwargs: axes.set_zlabel(kwargs.get('zlabel'))


    def scatter(self, xs, ys, pos="111", hist=True, nbins=20, **kwargs):
        """Scatter plot with histogram

        Parameters:
        ----------
        xs, ys : array of floats
            Positions of the points
        pos : str, optional
            Position of the subplot inside the figure
        hist : bool, optional
            Switch histogram on or off
        nbins : int, optional
            Number of bins in each histogram
        scheme : str, optional
            Color scheme name
        label : str, optional
            Plot label
        color : str, optional
            Color of the plot
        xscale, yscale : str, optional
            Scale of the axis, e.g., 'log', 'linear'
        xmin, xmax, ymin, ymax : float, optional
            Axis boundries
        xlabel, ylabel, zlabel : str, optional
            Axis labels
        """

        self.plt.subplot(pos)

        params = _getparams(**kwargs)

        k = kwargs.get

        xmin = k('xmin') if 'xmin' in kwargs else np.min(xs)
        xmax = k('xmax') if 'xmax' in kwargs else np.max(xs)
        ymin = k('ymin') if 'ymin' in kwargs else np.min(ys)
        ymax = k('ymax') if 'ymax' in kwargs else np.max(ys)

        xlim = (np.min([xmin, self.plt.gca().get_xlim()[0]]),
                np.max([xmax, self.plt.gca().get_xlim()[1]]))

        ylim = (np.min([ymin, self.plt.gca().get_ylim()[0]]),
                np.max([ymax, self.plt.gca().get_ylim()[1]]))

        self.plt.gca().set_xlim(xlim)
        self.plt.gca().set_ylim(ylim)

        self._setscales(**kwargs)
        self._setlabels(**kwargs)
        self._setaxiscolor(**kwargs)

        self.plt.scatter(xs, ys, c=params['color'], marker='.')


    def legend(self, pos="111",
               bgcolor=cdict.get('AUTUMN_COLORSCHEME')['gridcolor']):
        """Add legend to plot"""

        self.plt.subplot(pos)

        legend = self.plt.gca().legend()
        frame = legend.get_frame()
        frame.set_linewidth(0.0)
        frame.set_facecolor(bgcolor)
        frame.set_alpha(0.7)

    def save(self, name):
        """Saving plot

        Parameters
        ----------
        name : str
            name and extension of the plot
        """

        self.plt.savefig(name)


    def _setscales(self, **kwargs):
        """Setting plot scale parameters if available"""

        if 'xscale' in kwargs: self.plt.gca().set_xscale(kwargs.get('xscale'))
        if 'yscale' in kwargs: self.plt.gca().set_yscale(kwargs.get('yscale'))


    def _setlabels(self, **kwargs):
        """Setting plot labels parameters if available"""

        if 'xlabel' in kwargs: self.plt.gca().set_xlabel(kwargs.get('xlabel'))
        if 'ylabel' in kwargs: self.plt.gca().set_ylabel(kwargs.get('ylabel'))
        if 'zlabel' in kwargs: self.plt.gca().set_zlabel(kwargs.get('zlabel'))


    def _setarea(self, **kwargs):
        """Setting plot area if available"""

        if 'xmin' in kwargs: self.plt.gca().set_xlim(left=kwargs.get('xmin'))
        if 'xmax' in kwargs: self.plt.gca().set_xlim(right=kwargs.get('xmax'))
        if 'ymin' in kwargs: self.plt.gca().set_ylim(bottom=kwargs.get('ymin'))
        if 'ymax' in kwargs: self.plt.gca().set_ylim(top=kwargs.get('ymax'))


    def _removetoprightspines(self):
        """Removing plot top and right spines"""

        self.plt.gca().spines["top"].set_visible(False)
        self.plt.gca().spines["right"].set_visible(False)


    def _setaxiscolor(self, **kwargs):
        """Setting plot axis color if available"""

        cscheme = cdict.get(kwargs.get('scheme')) if 'scheme' in kwargs \
                  else cdict.get('AUTUMN_COLORSCHEME')

        self.plt.gca().get_xaxis().tick_bottom()
        self.plt.gca().spines['bottom'].set_color(cscheme['axiscolor'])
        self.plt.gca().yaxis.label.set_color(cscheme['axiscolor'])
        self.plt.gca().tick_params(axis='y', colors=cscheme['axiscolor'])

        self.plt.gca().get_yaxis().tick_left()
        self.plt.gca().spines['left'].set_color(cscheme['axiscolor'])
        self.plt.gca().xaxis.label.set_color(cscheme['axiscolor'])
        self.plt.gca().tick_params(axis='x', colors=cscheme['axiscolor'])

        self.plt.gca().set_axis_bgcolor(cscheme['background'])
        self.plt.gca().grid(
            color=cscheme['gridcolor'], linestyle='solid', linewidth=1)

        # set grid lines behind the plot
        self.plt.gca().set_axisbelow(True)


def _getparams(**kwargs):
    """Extract keyword parameters"""

    k = kwargs.get

    scheme = k('scheme') if 'scheme' in kwargs else 'AUTUMN_COLORSCHEME'

    defaults = [
        ('label', ''),
        ('color', cdict.get(scheme)['linecolor']),
        ('ecolor', cdict.get(scheme)['ecolor']),
        ('linestyle', 'solid'),
        ('scheme', 'AUTUMN_COLORSCHEME')]

    params = {}
    for elem in defaults:
        params[elem[0]] = k(elem[0]) if elem[0] in kwargs else elem[1]

    return params
