"""myplot.py """

from __future__ import print_function
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import numpy as np
from ..visualization import mycolordict as cdict


class MyPlot(object):
    """MyPlot class"""

    def __init__(self):
        """Constructor of MyPlot"""
        self.plt = plt

    def errorbar(self, plotparams, **kwargs):
        """Plotting with error bars

        Parameters
        ----------
        plotparams : dict
            A dictionary containing these keys: 'x' : list, 'y' : list,
            'xerr' : int or list, 'yerr' : int or list
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

        if len(dict(kwargs).keys()) > 0:
            print('Plotting using following parameters:')
            for key, value in dict(kwargs).iteritems():
                print('\t {:15s}'.format(str(key)) + str(value))
            print('')

        params = _getparams(**kwargs)

        self._setplotattrs(**kwargs)

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


    def plot(self, plotparams, **kwargs):
        """plot

        Parameters
        ----------
        plotparams : dict
            A dictionary containing these keys: 'x' : list, 'y' : list
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

        params = _getparams(**kwargs)

        if len(dict(kwargs).keys()) > 0:
            print('Plotting using following parameters:')
            for key, value in dict(kwargs).iteritems():
                print('\t {:15s}'.format(str(key)) + str(value))
            print('')

        self._setplotattrs(**kwargs)

        self.plt.plot(
            plotparams['x'], plotparams['y'],
            color=params['color'],
            linestyle=params['linestyle'],
            label=params['label'])


    def density(self, plotdata, vmin=None, vmax=None, **kwargs):
        """Density plot

        Parameters:
        ----------
        plotdata : 2D Array
        vmin : float or None
            Minumum value of colormapping
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
        """

        params = _getparams(**kwargs)

        if vmin is None:
            vmin = np.amin(plotdata)
        if vmax is None:
            vmax = np.amax(plotdata)

        if len(dict(kwargs).keys()) > 0:
            print('Plotting using following parameters:')
            for key, value in dict(kwargs).iteritems():
                print('\t {:15s}'.format(str(key)) + str(value))
            print('\t {:15s}'.format('vmin: ' + str(vmin)))
            print('\t {:15s}'.format('vmax: ' + str(vmax)))
            print('')

        self._setplotattrs(**kwargs)

        cm = mcolors.LinearSegmentedColormap(
            'CustomMap', cdict.get(params['scheme'])['cdict'])

        self.plt.imshow(plotdata, cmap=cm, vmin=vmin, vmax=vmax)


    def legend(self, bgcolor=cdict.get('AUTUMN_COLORSCHEME')['gridcolor']):
        """Add legend to plot"""

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


    def _setplotattrs(self, **kwargs):
        """Setting plot parameters if available"""

        k = kwargs.get

        cscheme = cdict.get(k('scheme')) if 'scheme' in kwargs else cdict.get(
            'AUTUMN_COLORSCHEME')

        if 'xscale' in kwargs: self.plt.xscale(k('xscale'))
        if 'yscale' in kwargs: self.plt.yscale(k('yscale'))

        if 'xlabel' in kwargs: self.plt.xlabel(k('xlabel'))
        if 'ylabel' in kwargs: self.plt.ylabel(k('ylabel'))

        if 'xmin' in kwargs: self.plt.gca().set_xlim(left=k('xmin'))
        if 'xmax' in kwargs: self.plt.gca().set_xlim(right=k('xmax'))
        if 'ymin' in kwargs: self.plt.gca().set_ylim(bottom=k('ymin'))
        if 'ymax' in kwargs: self.plt.gca().set_ylim(top=k('ymax'))

        self.plt.gca().spines["top"].set_visible(False)
        self.plt.gca().spines["right"].set_visible(False)

        self.plt.gca().get_xaxis().tick_bottom()
        self.plt.gca().spines['bottom'].set_color(cscheme['axiscolor'])
        self.plt.gca().yaxis.label.set_color(cscheme['axiscolor'])
        self.plt.gca().tick_params(axis='y', colors=cscheme['axiscolor'])

        self.plt.gca().get_yaxis().tick_left()
        self.plt.gca().spines['left'].set_color(cscheme['axiscolor'])
        self.plt.gca().xaxis.label.set_color(cscheme['axiscolor'])
        self.plt.gca().tick_params(axis='x', colors=cscheme['axiscolor'])

        self.plt.gca().set_axis_bgcolor(cscheme['background'])
        self.plt.grid(
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
