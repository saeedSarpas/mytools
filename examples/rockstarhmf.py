"""rockstarhmf.py
Generating halo mass function from the output of Rockstar halofinder
"""

from __future__ import print_function

from ..halofinder.rockstar import Rockstar
from ..halomassfunction.myhmf import MyHMF
from ..halomassfunction.tinker08 import Tinker08
from ..visualization.myplot import MyPlot
from ..visualization.mycolordict import helpercolors, primarycolors


class RockstarHMF(object):
    """RockstarHMF class"""

    def __init__(self, paths):
        """Initializing RockstarHMF class and loading rockstar file

        Make sure to path the output Rockstar file containing parents ids

        Parameters
        ----------
        paths : str or array of str
            Path to Rockstar file(s)

        Examples
        --------
        >>> from mytools.examples.rockstarhmf import RockstarHMF as RHMF

        >>> rhmf = RHMF('/path/to/rockstar/file')
        ... /path/to/rockstar/file:
        ... label: <some_label>
        ... color: <some_color>

        >>> rhmf.hmf()

        >>> rhmf.plot('/path/to/output/file/with/extension')
        """

        self.params = {}

        if isinstance(paths, str):
            paths = [paths]

        for path in paths:
            print(path + ':')
            self.params[path] = {}
            self.params[path]['label'] = raw_input('label:')
            self.params[path]['rockstar'] = Rockstar(path)
            self.params[path]['rockstar'].load(
                only=['mbound_vir', 'PID'], onlyhosts=True)

    def hmf(self):
        """Generating histogram from loaded rockstar data"""

        nbins = 20

        for _, value in self.params.iteritems():
            value['rockstar'].setheader('Number_of_bins', nbins)
            value['hmf'] = MyHMF(value['rockstar'].halos['mbound_vir'], 100)
            value['hmf'].hmf(nbins)

    def plot(self, name):
        """Plotting halo mass function"""

        kws = {}
        kws['xscale'] = 'log'
        kws['yscale'] = 'log'
        kws['xlabel'] = '$M\\ [h^{-1}M_{\\odot}]$'
        kws['ylabel'] = '$dn / d\\ln(M) dV\\ [h^3Mpc^{-3}]$'

        myplot = MyPlot()
        primarycolor = primarycolors('RAINBOW')
        helpercolor = helpercolors('RAINBOW')

        for _, value in self.params.iteritems():
            kws['color'] = primarycolor.next()
            myplot.plot({'x': value['hmf'].m, 'y': value['hmf'].dndlnmdv},
                        label=value['label'],
                        **dict(kws))

        tinkerplot = Tinker08()
        kws['color'] = helpercolor.next()
        myplot.plot(tinkerplot, label='Tinker 2008', **dict(kws))

        myplot.legend()

        myplot.save(name)
