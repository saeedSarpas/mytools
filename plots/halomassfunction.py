"""halofassfunction.py
Plotting the halo mass function"""

import matplotlib.pyplot as plt
import numpy             as np

from ..sharedtools.mycolordict import AUTUMN_COLORSCHEME as cscheme
from ..sharedtools             import headerextractor

class Rockstar(object):
    """Handling the generation of the halo mass function of a rockstar output"""

    def __init__(self, fname, **kwargs):
        """Loading necessary data for plotting halo mass function
        Note that the normal output of Rockstar does not contain the parent
        id of the halos, for generating outputs with parent ids, run:
        `./util/find_parent <normal rockstar output path>`
        from the root directory of the Rockstar project"""

        self.fname = fname
        self.hist = {}

        k = kwargs.get

        # Extracting header information
        self.headers = headerextractor.run(fname, 'rockstar')

        skiprows  = k('skiprows')   if 'skiprows'  in kwargs else 19
        num_p_col = k('num_p_col')  if 'num_p_col' in kwargs else 1
        mass_col  = k('mass_col')   if 'mass_col'  in kwargs else 3
        pid_col   = k('pid_col')    if 'pid_col'   in kwargs else -1

        print('Extracting data using following parameters:')
        print('\tskiprows:  \t' + str(skiprows))
        print('\tnum_p_col: \t' + str(num_p_col) +
              '\t' + self.headers['column_tags'][num_p_col])
        print('\tmass_col:  \t' + str(mass_col)  +
              '\t' + self.headers['column_tags'][mass_col])
        print('\tpid_col:   \t' + str(pid_col)   +
              '\t' + self.headers['column_tags'][pid_col])
        print('')

        # Extracting data from rockstar output
        data = np.genfromtxt(
            fname,
            unpack='true',
            skiprows=skiprows,
            usecols=(num_p_col, mass_col, pid_col),
            dtype=[
                ('num_p', np.int),
                ('mass',  np.float),
                ('pid',   np.int)
            ])

        self.hosts = [d['mass'] for d in data if d['pid'] == -1]

        self.headers['Number_of_found_hosts'] = [
            len(self.hosts), "out of", len(data['mass'])]

    def histogram(self, **kwargs):
        """Calling _histogram function for generating halo mass function"""
        k = kwargs.get

        nbins = k('nbins') if 'nbins' in kwargs else 20

        self.headers['Number_of_bins'] = str(nbins)

        print('Generating histogram using following parameters')
        print('\tnbins:     \t' + str(nbins))
        print('')

        self.hist = _histogram(self.hosts, nbins)

        # dn / dlog(M)dV
        boxvolume = float(self.headers['Box_size'][0])**3
        for i in range(len(self.hist['n'])):
            bins = self.hist['bin_edges']
            dlogMdV = (np.log10(bins[i+1]) - np.log10(bins[i])) * boxvolume
            self.hist['n'][i] /= dlogMdV

        for key, value in self.headers.iteritems():
            if key is not 'column_tags': print(key, value)

    def plot(self, **kwargs):
        """Calling _plot function for plotting the halo mass function"""
        if not self.hist:
            print('[error] Histogram data is not available for plotting.')
            print('        Make sure to run `Rockstar.histogram()` first.')
            return

        k = kwargs.get

        save = k('save') if 'save' in kwargs else False
        name = k('name') if 'name' in kwargs else self.fname + '.png'

        if not save:
            print('In case you want to save your plot, you should pass,')
            print('\t`save=True`')
            print('to the `plot` function.')

        _plot(name, self.hist, save, **kwargs)


def _histogram(hosts, nbins):
    """Calculating a logarithmic histogram for hosts"""
    result = {'n': [], 'hist': [], 'bin_edges': []}

    result['bin_edges'] = np.logspace(
        np.log10(np.min(hosts)),
        np.log10(np.max(hosts)),
        num=nbins+1,
        base=10)

    for bmin, bmax in zip(result['bin_edges'][:-1], result['bin_edges'][1:]):
        binned_hosts = [h for h in hosts if h >= bmin and h < bmax]
        result['n'].append(len(binned_hosts))
        result['hist'].append(np.mean(binned_hosts))

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

    if 'xscale' in kwargs:
        plt.xscale(k('xscale'))
    else:
        plt.xscale('log')

    if 'yscale' in kwargs:
        plt.yscale(k('yscale'))
    else:
        plt.yscale('log')

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
