"""plotwitherrorbars.py
Plotting curves with error bars."""

import matplotlib.pyplot as plt
from ..sharedtools import mycolordict as cdict

def errorbars(plot, fname, save, **kwargs):
    """Plotting curves with error bars
    Parameters:
    plot    : Dictionary ('x', 'y', 'xerr', 'yerr')
            Necessary info for plotting
    fname   : String
            filename
    save    : Bool
            true if you want to save the plot on disk"""

    k = kwargs.get

    scheme = k('scheme') if 'scheme' in kwargs else 'AUTUMN_COLORSCHEME'

    param = {}
    param['label']     = k("label")     if 'label'     in kwargs else ''
    param['color']     = k("color")     if 'color'     in kwargs else cdict.get(scheme)['linecolor']
    param['ecolor']    = k("ecolor")    if 'color'     in kwargs else cdict.get(scheme)['ecolor']
    param['linestyle'] = k("linestyle") if 'linestyle' in kwargs else 'solid'

    if 'xscale' in kwargs:
        plt.xscale(k('xscale'))

    if 'yscale' in kwargs:
        plt.yscale(k('yscale'))

    if 'xmin'   in kwargs: plt.gca().set_xlim(left=k('xmin'))
    if 'xmax'   in kwargs: plt.gca().set_xlim(right=k('xmax'))
    if 'ymin'   in kwargs: plt.gca().set_ylim(bottom=k('ymin'))
    if 'ymax'   in kwargs: plt.gca().set_ylim(top=k('ymax'))

    plt.errorbar(
        plot['x'], plot['y'],
        xerr=plot['xerr'], yerr=plot['yerr'],
        color=param['color'],
        ecolor=param['ecolor'],
        linestyle=param['linestyle'],
        label=param['label'])

    if 'xlabel' in kwargs:
        plt.xlabel(k('xlabel'))

    if 'ylabel' in kwargs:
        plt.ylabel(k('ylabel'))

    if save:
        plt.savefig(fname + '.png')
    else:
        plt.show()
