"""rockstargadgethmp.py
Generating halo mass profile from the output file of Rockstar halofinder
and Gadget snapshot"""


from __future__ import print_function

from os.path import split
import numpy as np
from astropy.cosmology import Planck15
import astropy.units as units

from ..halofinder.rockstar import Rockstar
from ..simulation.gadget import Gadget
from ..halomassprofile.myhmp import MyHMP
from ..halomassprofile.nfw import NFW
from ..visualization.myplot import MyPlot
from ..visualization.mycolordict import helpercolors, primarycolors


class RockstarGadgetHMP(object):
    """RockstarGadgetHMP class"""

    def __init__(self, rockstar_paths, gadget_paths, labels):
        """Constructor of RockstarGadgetHMP class

        Parameters
        ----------
        rockstar_path : str
            Path to the rockstar output file
        gadget_path : str
            Path to the gadget snapshot

        Examples
        --------
        >>> from mytools.examples.rockstargadgethmp import RockstarGadgetHMP as RGHMP

        >>> rghmp = RGHMP('/path/to/rockstar', '/path/to/gadget/snapshot')
        ... /path/to/rockstar, /path/to/gadget/snapshot:
        ... label: <label-name>
        ... color: <some-color>

        >>> rghmp.hmp()

        >>> rghmp.plot('/path/to/output/file/with/extension')
        """

        self.params = []
        self.myplot = None

        if isinstance(rockstar_paths, str):
            rockstar_paths = [rockstar_paths]
            gadget_paths = [gadget_paths]
            labels = [labels]

        self.m_bins = np.logspace(10, 15, num=11, base=10)

        for i in range(len(rockstar_paths)):
            rpath = rockstar_paths[i]
            gpath = gadget_paths[i]
            self.params.append({})
            pointer = self.params[i]

            print(split(rpath)[1] + ' (snapshot: ' + split(gpath)[1] + '):')

            pointer['label'] = labels[i]
            lratio = 1000

            print('Loading Rockstar halos')
            pointer['rockstar'] = rockstar = Rockstar(rpath)
            rockstar.load(
                only=['x', 'y', 'z', 'rvir', 'mbound_vir', 'PID'],
                onlyhosts=False)
            rockstar.halos['x'] *= lratio
            rockstar.halos['y'] *= lratio
            rockstar.halos['z'] *= lratio
            soft_len = rockstar.header['Force_resolution_assumed'][0]
            rockstar.header['Softening_length'] = float(soft_len) * lratio

            rockstar.binning(self.m_bins)

            pointer['bin'] = _selectbin(rockstar.binnedhalos, self.m_bins)

            print('Loading and sorting Gadget snapshot (' + gpath + ')')
            pointer['gadget'] = gadget = Gadget(gpath)
            gadget.load(sorting=True, col='x')

    def hmp(self, z=0):
        """Generating halo mass profiles

        Parameters
        ----------
        z : float
            Redshift of the snapshot
        """

        rhocrit = Planck15.critical_density(z).to(units.Msun / units.kpc**3) \
                  / (Planck15.H(z) / 100)

        for i in range(len(self.params)):
            _input = self.params[i]
            halos = _input['rockstar'].binnedhalos[_input['bin']]

            _input['hmp'] = MyHMP(
                halos,
                _input['gadget'].data,
                float(_input['rockstar'].header['Particle_mass'][0]))


            rmin = _input['rockstar'].header['Softening_length'] / 2.0
            rmax = 2 * np.max(halos['rvir'])
            _input['rockstar'].binnedhalos['mean_rvir'] = sum(halos['rvir']) \
                                                          / len(halos['rvir'])
            rbins = np.logspace(np.log10(rmin), np.log10(rmax), num=50, base=10)

            _input['hmp'].hmp(rbins, rhocrit.value)

    def plot(self, nfwofs=256, loc='lower left'):
        """Plotting halo mass profile

        Parameters
        ----------
        name : str
            Name + extension of the output file
        """

        kws = {}
        kws['xscale'] = 'log'
        kws['yscale'] = 'log'
        kws['xlabel'] = '$\\log_{10}(r / kpc)$'
        kws['ylabel'] = '$\\log_{10}(\\rho / \\rho_{crit})$'
        kws['silent'] = True
        kws['ymin'] = 1e0
        kws['ymax'] = 1e6
        kws['xmin'] = 1e0
        kws['xmax'] = 1e4

        self.myplot = MyPlot()

        nfwplots = []
        values = []
        primarycolor = primarycolors('RAINBOW')

        for i in range(len(self.params)):
            value = self.params[i]
            kws['color'] = primarycolor.next()
            self.myplot.plot({'x': value['hmp'].r, 'y': value['hmp'].rho_rhocrit},
                        label=value['label'],
                        **dict(kws))

            minm = self.m_bins[value['bin']]
            maxm = self.m_bins[value['bin'] + 1]
            mass = 10**((np.log10(minm * maxm)) / 2)
            z = (1.0 / value['gadget'].headers['time']) - 1.0

            if (mass, z) not in nfwplots:
                nfwplots.append((mass, z))
                values.append(value)

        for nfw, val, i in zip(nfwplots, values, range(len(nfwplots))):
            nfwplot, _, _ = NFW(nfw[0], #mass
                                val['rockstar'].binnedhalos['mean_rvir'],
                                nfw[1], #redshift
                                nfwofs=nfwofs)

            kws['color'] = ["#000000", "#777777", "#aaaaaa"][i]
            label = r"NFW ($%1.2g\  M_{\odot} h^{-1}$)" % mass
            self.myplot.plot(nfwplot, label=label, **dict(kws))

        self.myplot.legend(loc=loc)

    def save(self, name):
        self.myplot.save(name)


def _selectbin(binnedhalos, bins):
    """Getting desired bin to profile from user

    Parameters
    ----------
    binnedhalos : dict
        A dictionary containing binned halos
    bins : array of float
        Array of mass bin edges
    """

    while True:
        print('{:3}\t{:44}\t{:10}'.format('id', 'Mass range', "# of halos"))

        validinputs = []

        for idx in binnedhalos.keys():
            nfoundhalos = len(binnedhalos[idx])
            strindex = str(idx) if nfoundhalos > 1 else ''
            validinputs.append(idx)
            print('{:3}\t({:20}, {:20})\t{:10}'.format(
                strindex, bins[idx], bins[idx+1], nfoundhalos))

        return 8

        # if index in validinputs:
        #     return index
        # else:
        #     print('Your input was invalid. Please try again.')
