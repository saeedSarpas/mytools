"""myplot.py """


from __future__ import print_function
from ..visualization import mycolordict as cdict


def myerrorbar(plt, plotparams, **kwargs):
    """Plotting with error bars

    Parameters
    ----------
    plt : matplotlib.pyplot object
        pyplot object to draw a new plot in it
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

    Returns
    -------
    plt : matplotlib.pyplot object

    Examples
    --------
    >>> import matplotlib.pyplot as plt
    >>> from mytools.visualization.plot import myerrorbar, save
    >>> points = {
    ...     'x': range(1, 100),
    ...     'y': range(1, 100),
    ...     'xerr': 1,
    ...     'yerr': 1}
    >>> updatedplt = myerrorbar(plt, points, lable='Plot')
    >>> updatedpl.show()
    >>> save(updatedplt, 'updatedplot.png')

    .. note:: Don't use plt.show() before savig the plot
              Using pyplot show() function before saving the plot, resets the
              current plot object and as a result your saved plot will be just
              a blank page.
              For more information, see http://stackoverflow.com/questions/21875356/saving-a-figure-after-invoking-pyplot-show-results-in-an-empty-file
    """

    print('Plotting using following parameters:')

    for key, value in dict(kwargs).iteritems():
        print('\t {:15s}'.format(str(key)) + str(value))
    print('')

    params = _getparams(**kwargs)

    _setplotattrs(plt, **kwargs)

    plt.errorbar(
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
            plt.fill_between(
                plotparams['x'], yneg, ypos,
                facecolor=cdict.get(params['scheme'])['facecolor'],
                edgecolor=cdict.get(params['scheme'])['facecolor'],
                interpolate=True)

    return plt


def myplot(plt, plotparams, **kwargs):
    """Plotting

    Parameters
    ----------
    plt : matplotlib.pyplot object
        pyplot object to draw a new plot in it
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

    Returns
    -------
    plt : matplotlib.pyplot object

    Examples
    --------
    >>> import matplotlib.pyplot as plt
    >>> from mytools.visualization.plot import myplot, save
    >>> points = {
    ...     'x': range(1, 100),
    ...     'y': range(1, 100)}
    >>> updatedplt = myplot(plt, points, lable='Plot')
    >>> save(updatedplt, 'updatedplot.png')

    .. note:: Don't use plt.show() before savig the plot
              Using pyplot show() function before saving the plot, resets the
              current plot object and as a result your saved plot will be just
              a blank page.
              For more information, see http://stackoverflow.com/questions/21875356/saving-a-figure-after-invoking-pyplot-show-results-in-an-empty-file
    """

    params = _getparams(**kwargs)

    print('Plotting using following parameters:')
    for key, value in dict(kwargs).iteritems():
        print('\t {:15s}'.format(str(key)) + str(value))
    print('')

    _setplotattrs(plt, **kwargs)

    plt.plot(
        plotparams['x'], plotparams['y'],
        color=params['color'],
        linestyle=params['linestyle'],
        label=params['label'])

    return plt


def save(plt, name):
    """Saving plot

    Parameters
    ----------
    plt : matplotlib.pyplot object
    name : str
        name and extension of the plot
    """

    plt.savefig(name)

    plt.clf()
    plt.close()


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


def _setplotattrs(plt, **kwargs):
    """Setting plot parameters if available"""

    k = kwargs.get

    cscheme = cdict.get(k('scheme')) if 'scheme' in kwargs else cdict.get(
        'AUTUMN_COLORSCHEME')

    if 'xscale' in kwargs: plt.xscale(k('xscale'))
    if 'yscale' in kwargs: plt.yscale(k('yscale'))

    if 'xlabel' in kwargs: plt.xlabel(k('xlabel'))
    if 'ylabel' in kwargs: plt.ylabel(k('ylabel'))

    if 'xmin' in kwargs: plt.gca().set_xlim(left=k('xmin'))
    if 'xmax' in kwargs: plt.gca().set_xlim(right=k('xmax'))
    if 'ymin' in kwargs: plt.gca().set_ylim(bottom=k('ymin'))
    if 'ymax' in kwargs: plt.gca().set_ylim(top=k('ymax'))

    plt.gca().spines["top"].set_visible(False)
    plt.gca().spines["right"].set_visible(False)

    plt.gca().get_xaxis().tick_bottom()
    plt.gca().spines['bottom'].set_color(cscheme['axiscolor'])
    plt.gca().yaxis.label.set_color(cscheme['axiscolor'])
    plt.gca().tick_params(axis='y', colors=cscheme['axiscolor'])

    plt.gca().get_yaxis().tick_left()
    plt.gca().spines['left'].set_color(cscheme['axiscolor'])
    plt.gca().xaxis.label.set_color(cscheme['axiscolor'])
    plt.gca().tick_params(axis='x', colors=cscheme['axiscolor'])

    plt.gca().set_axis_bgcolor(cscheme['background'])
    plt.grid(color=cscheme['gridcolor'], linestyle='solid', linewidth=1)

    # set grid lines behind the plot
    plt.gca().set_axisbelow(True)
