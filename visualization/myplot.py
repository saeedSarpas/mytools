"""myplot.py """

from __future__ import print_function
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import numpy as np
from ..visualization import mycolordict as cdict
from mpl_toolkits.mplot3d import Axes3D
# from matplotlib.ticker import NullFormatter


class MyPlot(object):
    """MyPlot class"""

    def __init__(self, aspect=1):
        """Constructor of MyPlot

        Parameters
        ----------
        aspect: float, optional
            The aspect ratio of the axes
        """
        self.plt = plt
        self.plt.figure(1, figsize=self.plt.figaspect(aspect))

    def new2daxes(self, pos='111', sharex=None, sharey=None):
        """Creatign new 3d axes for plotting

        Parameters
        ----------
        pos : str, optional
            Position of the subplot inside the figure
        sharex, sharey : Axes, optional
            Sharing axes with other plots
        """

        return self.plt.gcf().add_subplot(pos, sharex=sharex, sharey=sharey)

    def errorbar(self, plotparams, pos="111", ax=None, **kwargs):
        """Plotting with error bars

        Parameters
        ----------
        plotparams : dict
            A dictionary containing these keys: 'x' : list, 'y' : list,
            'xerr' : int or list, 'yerr' : int or list
        pos : str, optional
            Position of the subplot inside the figure
        ax : Axes obj, optional
            Axes generated by new2daxes
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

        if ax is None:
            ax = self.plt.subplot(pos)

        params = _getparams(**kwargs)

        if not params['silent']:
            _printargs("\"errorbar\"", **kwargs)

        self._setscales(**kwargs)
        self._setlabels(**kwargs)
        self._setarea(**kwargs)
        self._stylespines(**kwargs)
        self._setaxiscolor(**kwargs)
        self._setgrid(**kwargs)

        ax.errorbar(
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
                    facecolor=params['shadow'],
                    edgecolor=params['shadow'],
                    interpolate=True)


    def plot(self, plotparams, pos="111", ax=None, **kwargs):
        """plot

        Parameters
        ----------
        plotparams : dict
            A dictionary containing these keys: 'x' : list, 'y' : list
        pos : str, optional
            Position of the subplot inside the figure
        ax : Axes obj, optional
            Axes generated by new2daxes
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

        if ax is None:
            ax = self.plt.subplot(pos)

        params = _getparams(**kwargs)

        if not params['silent']:
            _printargs("\"plot\"", **kwargs)

        self._setscales(**kwargs)
        self._setlabels(**kwargs)
        self._setarea(**kwargs)
        self._stylespines(**kwargs)
        self._setaxiscolor(**kwargs)
        self._setgrid(**kwargs)

        ax.plot(
            plotparams['x'], plotparams['y'],
            color=params['color'],
            linestyle=params['linestyle'],
            label=params['label'])


    def density(self, plotdata, pos="111", ax=None, vmin=None, vmax=None,
                **kwargs):
        """Density plot

        Parameters:
        ----------
        plotdata : 2D Array
        pos : str, optional
            Position of the subplot inside the figure
        ax : Axes obj, optional
            Axes generated by new2daxes
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

        if ax is None:
            ax = self.plt.subplot(pos)

        params = _getparams(**kwargs)

        if not params['silent']:
            _printargs("\"density\"", **kwargs)
            # TODO: add vmin and vmax to the list of kwargs too

        if vmin is None: vmin = np.amin(plotdata)
        if vmax is None: vmax = np.amax(plotdata)

        self._setscales(**kwargs)
        self._setlabels(**kwargs)
        self._setarea(**kwargs)
        self._stylespines(**kwargs)
        self._setaxiscolor(**kwargs)
        self._setgrid(**kwargs)

        cmap = mcolors.LinearSegmentedColormap(
            'CustomMap', cdict.get(params['scheme'])['cdict'])

        ax.imshow(plotdata, cmap=cmap, vmin=vmin, vmax=vmax)

    def new3daxes(self, pos='111'):
        """Creatign new 3d axes for plotting

        Parameters
        ----------
        pos : str, optional
            Position of the subplot inside the figure
        """

        return self.plt.gcf().add_subplot(pos, projection='3d')

    def scatter3d(self, xs, ys, zs, pos='111', ax=None, s=10, **kwargs):
        """Scatter3d plot

        Parameters:
        ----------
        xs, ys, zs : array of floats
            Positions of the points
        pos : str, optional
            Position of the subplot inside the figure
        axes : plt.Axes3D obj, optional
            Axes3D object generated by new3daxes
        s : int, optional
            surface of dots
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
        alpha : float
            The transparency of the markers. 1 opaque, 0 transparent

        Examples
        --------
        >>> plot = MyPlot()
        >>> axes = plot.new3daxes(pos="211")
        >>> plot.scatter3d(xs, ys, zs, axes)
        >>> plot.save('/path')
        """

        if ax is None:
            ax = self.plt.gcf().add_subplot(pos, projection='3d')

        params = _getparams(**kwargs)

        if not params['silent']:
            _printargs("\"scatter3d\"", **kwargs)

        k = kwargs.get
        alpha = k('alpha') if 'alpha' in kwargs else None

        ax.scatter(xs, ys, zs, c=params['color'], marker='o', s=s,
                   edgecolors='none', lw=0, alpha=alpha)

        if 'xlabel' in kwargs: ax.set_xlabel(k('xlabel'))
        if 'ylabel' in kwargs: ax.set_ylabel(k('ylabel'))
        if 'zlabel' in kwargs: ax.set_zlabel(k('zlabel'))

        if 'xmin' in kwargs and 'xmax' in kwargs:
            ax.set_xlim(k('xmin'), k('xmax'))

        if 'ymin' in kwargs and 'ymax' in kwargs:
            ax.set_ylim(k('ymin'), k('ymax'))

        if 'zmin' in kwargs and 'zmax' in kwargs:
            ax.set_zlim(k('zmin'), k('zmax'))

        self._setscales(**kwargs)
        self._setaxiscolor3d(**kwargs)


    def scatter(self, xs, ys, pos="111", ax=None, s=10, **kwargs):
        """Scatter plot with histogram

        Parameters:
        ----------
        xs, ys : array of floats
            Positions of the points
        pos : str, optional
            Position of the subplot inside the figure
        ax : Axes obj, optional
            Axes generated by new2daxes
        s : int, optional
            surface of dots
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
        alpha : float
            The transparency of the markers. 1 opaque, 0 transparent
        """

        if ax is None:
            ax = self.plt.subplot(pos)

        params = _getparams(**kwargs)

        if not params['silent']:
            _printargs("\"scatter\"", **kwargs)

        k = kwargs.get
        alpha = k('alpha') if 'alpha' in kwargs else None

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

        self.plt.scatter(xs, ys, c=params['color'], marker='o', s=s,
                         edgecolors='none', lw=0, alpha=alpha)

        self._setscales(**kwargs)
        self._setlabels(**kwargs)
        self._setaxiscolor(**kwargs)
        self._setgrid(**kwargs)
        self._stylespines(**kwargs)
        self._setarea(**kwargs)


    def legend(self, pos="111", ax=None,
               bgcolor=cdict.get('RAINBOW')['gridcolor']):
        """Add legend to plot

        Parameters
        ----------
        pos : str, optional
            Position of the subplot inside the figure
        ax : Axes obj, optional
            Axes generated by new2daxes or new3daxes
        bgcolor : str, optional
            Background color of the legend
        """

        if ax is None:
            ax = self.plt.subplot(pos)

        legend = ax.legend()
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


    def _setarea(self, **kwargs):
        """Setting plot area if available"""

        if 'xmin' in kwargs: self.plt.gca().set_xlim(left=kwargs.get('xmin'))
        if 'xmax' in kwargs: self.plt.gca().set_xlim(right=kwargs.get('xmax'))
        if 'ymin' in kwargs: self.plt.gca().set_ylim(bottom=kwargs.get('ymin'))
        if 'ymax' in kwargs: self.plt.gca().set_ylim(top=kwargs.get('ymax'))

    def _stylespines(self, **kwargs):
        """Removing plot top and right spines"""

        cscheme = _getcscheme(**kwargs)

        self.plt.gca().spines["top"].set_visible(False)
        self.plt.gca().xaxis.set_ticks_position('bottom')
        self.plt.gca().spines["right"].set_visible(False)
        self.plt.gca().yaxis.set_ticks_position('left')
        self.plt.gca().spines['bottom'].set_color(cscheme['axiscolor'])
        self.plt.gca().spines['left'].set_color(cscheme['axiscolor'])


    def _setaxiscolor(self, **kwargs):
        """Setting plot axis color if available"""

        cscheme = _getcscheme(**kwargs)

        self.plt.gca().xaxis.label.set_color(cscheme['axiscolor'])
        self.plt.gca().tick_params(axis='x', colors=cscheme['axiscolor'])
        for tick in self.plt.gca().xaxis.get_major_ticks():
            tick.label.set_fontsize(8)

        self.plt.gca().yaxis.label.set_color(cscheme['axiscolor'])
        self.plt.gca().tick_params(axis='y', colors=cscheme['axiscolor'])
        for tick in self.plt.gca().yaxis.get_major_ticks():
            tick.label.set_fontsize(8)


    def _setaxiscolor3d(self, **kwargs):
        """Setting plot axis color if available"""

        cscheme = _getcscheme(**kwargs)

        self._setaxiscolor(**kwargs)

        self.plt.gca().zaxis.label.set_color(cscheme['axiscolor'])
        self.plt.gca().tick_params(axis='z', colors=cscheme['axiscolor'])
        for tick in self.plt.gca().zaxis.get_major_ticks():
            tick.label.set_fontsize(8)


    def _setgrid(self, **kwargs):
        """Setting plot grid and background"""

        cscheme = _getcscheme(**kwargs)

        self.plt.gca().set_axis_bgcolor(cscheme['background'])
        self.plt.gca().grid(
            color=cscheme['gridcolor'], linestyle='solid', linewidth=1)

        # set grid lines behind the plot
        self.plt.gca().set_axisbelow(True)

def _getcscheme(**kwargs):
    """Returning the colorscheme"""

    if 'scheme' in kwargs:
        return cdict.get(kwargs.get('scheme'))
    else:
        return cdict.get('RAINBOW')

def _printargs(title, **kwargs):
    """Printing used keyword arguments for a specific plot"""

    if len(dict(kwargs).keys()) > 0:
        print(str(title) + ' is plotting using following parameters:')
        for key, value in dict(kwargs).iteritems():
            print('\t {:15s}'.format(str(key)) + str(value))
            print('')


def _getparams(**kwargs):
    """Extract keyword parameters"""

    k = kwargs.get

    scheme = k('scheme') if 'scheme' in kwargs else 'RAINBOW'

    primarycolor = cdict.primarycolors(scheme)
    color = primarycolor.next()

    helpercolor = cdict.helpercolors(scheme)
    ecolor = helpercolor.next()

    primaryshadow = cdict.primaryshadows(scheme)
    shadow = primaryshadow.next()

    defaults = [
        ('label', ''),
        ('color', color),
        ('ecolor', ecolor),
        ('linestyle', 'solid'),
        ('scheme', 'RAINBOW'),
        ('shadow', shadow),
        ('silent', False)]

    params = {}
    for elem in defaults:
        params[elem[0]] = k(elem[0]) if elem[0] in kwargs else elem[1]

    return params
