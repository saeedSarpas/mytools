""" halofassfunction.py
    Plotting the halo mass function
"""

import matplotlib.pyplot as plt
import numpy             as np

from ..sharedtools.mycolordict import AUTUMN_COLORSCHEME as cscheme
from ..sharedtools             import headerextractor

class Rockstar(object):
    """Handling generation of the halo mass function of a rockstar output"""

    def __init__(self, fname, **kwargs):
        """Loading necessary data for plotting halo mass function
        Note that the normal output of Rockstar does not contain the parent
        id of the halos, for generating outputs with parent ids, run:
        `./util/find_parent <normal rockstar output path>`
        from the root directory of the Rockstar project"""

        self.fname = fname

        k = kwargs.get

        param = {}
        param['skiprows']  = k('skiprows')   if 'skiprows'   in kwargs else 19
        param['num_p_col'] = k('num_p_col')  if 'num_p_col'  in kwargs else 1
        param['mass_col']  = k('mass_col')   if 'mass_col'   in kwargs else 3
        param['pid_col']   = k('pid_col')    if 'pid_col'    in kwargs else 36
        param['nbins']     = k('nbins')      if 'nbins'      in kwargs else 20

        print('Extracting data using following parameters:')
        print('\tskiprows:  \t' + str(param['skiprows']))
        print('\tnum_p_col: \t' + str(param['num_p_col']))
        print('\tmass_col:  \t' + str(param['mass_col']))
        print('\tpid_col:   \t' + str(param['pid_col']))
        print('\tnbins:     \t' + str(param['nbins']))
        print('')

        # Extracting data from rockstar output
        data = np.loadtxt(
            fname,
            unpack='true',
            skiprows=param['skiprows'],
            usecols=(param['num_p_col'], param['mass_col'], param['pid_col']),
            dtype=[('num_p', np.int32), ('mass', np.float64), ('pid', np.int32)])

        # Extracting header information
        self.headers = headerextractor.run(fname, 'rockstar')
        self.headers['Number_of_bins'] = str(param['nbins'])

        # Selecting host halos (pid = -1)
        hosts = [data[1][i] for i in range(len(data[0])) if data[2][i] == -1]
        self.headers['Number_of_found_hosts'] = [len(hosts), "out of", len(data[0])]

        self.hist = _histogram(hosts, param['nbins'])

        # dn / dlog(M)dV
        boxvolume = float(self.headers['Box_size'][0])**3
        for i in range(len(self.hist['n'])):
            bins = self.hist['bin_edges']
            dlogMdV = (np.log10(bins[i+1]) - np.log10(bins[i])) * boxvolume
            self.hist['n'][i] /= dlogMdV

        for key, value in self.headers.iteritems():
            print(key, value)

    def plot(self, **kwargs):
        """Plotting histogram """
        save = False if 'save' not in kwargs or kwargs.get('save') != True else True
        if not save:
            print('\tIn case you want to save your plot, you should pass,')
            print('\t`save=True`')
            print('\tto the `plot` function.')

        _plot(self.fname, self.hist, save, **kwargs)


def _histogram(hosts, nbins):
    """Calculating a logarithmic histogram for hosts"""
    result = {'n': [], 'hist': [], 'bin_edges': []}

    result['bin_edges'] = np.logspace(
        np.log10(np.min(hosts)),
        np.log10(np.max(hosts)),
        num=nbins+1,
        base=10)

    for bmin, bmax in zip(result['bin_edges'][:-1], result['bin_edges'][1:]):
        binned_hosts = [h for h in hosts[:] if h >= bmin and h < bmax]
        if len(binned_hosts) > 0:
            result['n'].append(len(binned_hosts))
            result['hist'].append(np.mean(binned_hosts))
        else:
            result['n'].append(0)
            result['hist'].append(0)

    return result


def _plot(fname, hist, beSaved, **kwargs):
    """Plotting halo mass function (fname: filename, hist: should contain
    'bin_edges' and 'n')"""
    k = kwargs.get

    param = {}
    param['label']     = k("label")     if 'label'     in kwargs else ''
    param['color']     = k("color")     if 'color'     in kwargs else cscheme['linecolor']
    param['ecolor']    = k("ecolor")    if 'color'     in kwargs else cscheme['ecolor']
    param['linestyle'] = k("linestyle") if 'linestyle' in kwargs else 'solid'
    param['headers']   = k("headers")   if 'headers'   in kwargs else False

    if 'xscale' in kwargs: plt.xscale(k('xscale'))
    if 'yscale' in kwargs: plt.yscale(k('yscale'))

    if 'xmin'   in kwargs: plt.gca().set_xlim(left=k('xmin'))
    if 'xmax'   in kwargs: plt.gca().set_xlim(right=k('xmax'))
    if 'ymin'   in kwargs: plt.gca().set_ylim(bottom=k('ymin'))
    if 'ymax'   in kwargs: plt.gca().set_ylim(top=k('ymax'))

    # Center of bins
    bin_ranges = zip(hist['bin_edges'][1:], hist['bin_edges'][:-1])
    centers = [(f + i) / 2 for i, f in bin_ranges]

    # Preparing errors
    xerr    = [(f - i) / 2 for i, f in bin_ranges]

    plt.errorbar(
        centers, hist['n'],
        xerr=xerr,
        color=param['color'],
        ecolor=param['ecolor'],
        linestyle=param['linestyle'],
        label=param['label'])

    plt.xlabel('$M\ [h^{-1}M_{\odot}]$')
    plt.ylabel('$dn / d\log(M) dV\ [h^3Mpc^{-3}]$')

    plt.savefig(fname + '.png') if beSaved else plt.show()
