"""plotwitherrorbars.py
Plotting curves with error bars."""

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

def _setplotattrs(plt, **kwargs):
    """Setting plot parameters if available"""
    k = kwargs.get

    cscheme = cdict.get(k('scheme')) if 'scheme' in kwargs else cdict.get('AUTUMN_COLORSCHEME')

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
    plt.grid(
        color=cscheme['gridcolor'], linestyle='solid', linewidth=1, zorder=0)

def errorbars(plt, plotparams, **kwargs):
    """Plotting curves with error bars"""
    params = _getparams(**kwargs)

    print('Plotting using following parameters:')
    for key, value in dict(kwargs).iteritems():
        print('\t {:15s}'.format(str(key)) + str(value))
    print('')

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

def plotline(plt, plotparams, **kwargs):
    """Plotting line"""
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

    if 'show' in kwargs and kwargs.get('show') is True:
        plt.show()

    return plt

def save(plt, fname, extension):
    """Saving plot on disk"""
    plt.savefig(fname + extension)
