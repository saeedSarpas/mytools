"""plotwitherrorbars.py
Plotting curves with error bars."""

import matplotlib.pyplot as plt

from ..sharedtools import mycolordict as cdict

def _getparams(**kwargs):
    """Extract keyword parameters"""
    k = kwargs.get

    scheme = k('scheme') if 'scheme' in kwargs else 'AUTUMN_COLORSCHEME'

    params = {}
    params['label']     = k("label")     if 'label'     in kwargs else ''
    params['color']     = k("color")     if 'color'     in kwargs else cdict.get(scheme)['linecolor']
    params['ecolor']    = k("ecolor")    if 'color'     in kwargs else cdict.get(scheme)['ecolor']
    params['linestyle'] = k("linestyle") if 'linestyle' in kwargs else 'solid'
    params['scheme']    = k('scheme')    if 'scheme'    in kwargs else 'AUTUMN_COLORSCHEME'

    return params

def _setplotattrs(**kwargs):
    """Setting plot parameters if available"""
    k = kwargs.get

    if 'xscale' in kwargs: plt.xscale(k('xscale'))
    if 'yscale' in kwargs: plt.yscale(k('yscale'))

    if 'xlabel' in kwargs: plt.xlabel(k('xlabel'))
    if 'ylabel' in kwargs: plt.ylabel(k('ylabel'))

    if 'xmin' in kwargs: plt.gca().set_xlim(left=k('xmin'))
    if 'xmax' in kwargs: plt.gca().set_xlim(right=k('xmax'))
    if 'ymin' in kwargs: plt.gca().set_ylim(bottom=k('ymin'))
    if 'ymax' in kwargs: plt.gca().set_ylim(top=k('ymax'))

def errorbars(plot, **kwargs):
    """Plotting curves with error bars"""
    params = _getparams(**kwargs)

    print('Plotting using following parameters:')
    for key, value in dict(kwargs).iteritems():
        print('\t {:15s}'.format(str(key)) + str(value))

    _setplotattrs(**kwargs)

    plt.errorbar(
        plot['x'], plot['y'],
        xerr=plot['xerr'], yerr=plot['yerr'],
        color=params['color'],
        ecolor=params['ecolor'],
        linestyle=params['linestyle'],
        label=params['label'])

    if 'shaded' in kwargs and kwargs.get('shaded') is True:
        if 'yerr' in plot:
            yneg = [y - yerr for y, yerr in zip(plot['y'], plot['yerr'])]
            ypos = [y + yerr for y, yerr in zip(plot['y'], plot['yerr'])]
            plt.fill_between(
                plot['x'], yneg, ypos,
                facecolor=cdict.get(params['scheme'])['facecolor'],
                edgecolor=cdict.get(params['scheme'])['facecolor'],
                interpolate=True)

    if 'show' in kwargs and kwargs.get('show') is True:
        plt.show()

def save(fname):
    """Saving plot on disk"""
    plt.savefig(fname + 'png')
