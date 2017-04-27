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

    def __init__(self, paths, labels):
        """Initializing RockstarHMF class and loading rockstar file

        Make sure the Rockstar file contains parents ids

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

        for i in range(len(paths)):
            print(paths[i] + ':')
            self.params[i] = {}
            self.params[i]['label'] = labels[i]
            self.params[i]['rockstar'] = Rockstar(paths[i])
            self.params[i]['rockstar'].load(
                only=['mbound_vir'], onlyhosts=False)

        self.myplot = None

    def hmf(self):
        """Generating histogram from loaded rockstar data"""

        nbins = 20

        for _, value in self.params.iteritems():
            value['rockstar'].setheader('Number_of_bins', nbins)
            value['hmf'] = MyHMF(value['rockstar'].halos['mbound_vir'], 100)
            value['hmf'].hmf(nbins)

    def plot(self, z=0, loc='upper right'):
        """Plotting halo mass function"""

        kws = {}
        kws['xscale'] = 'log'
        kws['ymin'] = 1e-7
        kws['ymax'] = 1e0
        kws['yscale'] = 'log'
        kws['xlabel'] = '$M\\ [h^{-1}M_{\\odot}]$'
        kws['ylabel'] = '$dn / d\\ln(M) dV\\ [h^3Mpc^{-3}]$'
        kws['silent'] = True

        self.myplot = MyPlot()
        primarycolor = primarycolors('RAINBOW')

        for i in range(len(self.params)):
            kws['color'] = primarycolor.next()
            self.myplot.plot({
                'x': self.params[i]['hmf'].m,
                'y': self.params[i]['hmf'].dndlnmdv
            }, label=self.params[i]['label'], **dict(kws))

        tinkerplot = Tinker08(z=z)
        kws['color'] = "#000000"
        kws['label'] = r"$Tinker 08 (z = " + str(z) + r")$"
        self.myplot.plot(tinkerplot, **dict(kws))

        self.myplot.legend(loc=loc)

    def save(self, name):
        """Saving the hmf plot"""
        self.myplot.save(name)
