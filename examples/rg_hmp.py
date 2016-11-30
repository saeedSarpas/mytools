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

    def __init__(self, rockstar_path, gadget_path):
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

        self.params = {}

        if isinstance(rockstar_path, str) and isinstance(gadget_path, str):
            rockstar_path = [rockstar_path]
            gadget_path = [gadget_path]

        self.m_bins = np.logspace(10, 15, num=12+1, base=10)

        for rpath, gpath in zip(rockstar_path, gadget_path):
            self.params[rpath] = pointer = {}

            print(split(rpath)[1] + ' (snapshot: ' + split(gpath)[1] + '):')

            pointer['label'] = raw_input('label: ')
            lratio = float(
                raw_input('Rockstar2Gadget length conversion: '))

            print('Loading Rockstar halos')
            pointer['rockstar'] = rockstar = Rockstar(rpath)
            rockstar.load(
                only=['x', 'y', 'z', 'rvir', 'mbound_vir', 'PID'],
                onlyhosts=True)
            rockstar.halos['x'] *= lratio
            rockstar.halos['y'] *= lratio
            rockstar.halos['z'] *= lratio
            soft_len = rockstar.headers['Force_resolution_assumed'][0]
            rockstar.headers['Softening_length'] = float(soft_len) * lratio

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

        for _, _input in self.params.iteritems():
            halos = _input['rockstar'].binnedhalos[_input['bin']]

            _input['hmp'] = MyHMP(
                halos,
                _input['gadget'].data,
                float(_input['rockstar'].headers['Particle_mass'][0]))


            rmin = _input['rockstar'].headers['Softening_length'] / 2.0
            rmax = 2 * np.max(halos['rvir'])
            _input['rockstar'].binnedhalos['mean_rvir'] = sum(halos['rvir']) \
                                                          / len(halos['rvir'])
            rbins = np.logspace(np.log10(rmin), np.log10(rmax), num=50, base=10)

            _input['hmp'].hmp(rbins, rhocrit.value)

    def plot(self, name):
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

        myplot = MyPlot()

        nfwplots = []
        primarycolor = primarycolors('RAINBOW')
        helpercolor = helpercolors('RAINBOW')

        for _, value in self.params.iteritems():
            kws['color'] = primarycolor.next()
            myplot.plot({'x': value['hmp'].r, 'y': value['hmp'].rho_rhocrit},
                        label=value['label'],
                        **dict(kws))

            minm = self.m_bins[value['bin']]
            maxm = self.m_bins[value['bin'] + 1]
            mass = 10**((np.log10(minm * maxm)) / 2)
            z = (1.0 / value['gadget'].headers['time']) - 1.0

            if (mass, z) not in nfwplots:
                nfwplots.append((mass, z))
                nfwplot, _, _ = NFW(
                    mass, value['rockstar'].binnedhalos['mean_rvir'], z)

                kws['color'] = helpercolor.next()
                label = "NFW for %1.g SolMass" % mass
                myplot.plot(nfwplot, label=label, **dict(kws))

        myplot.legend()

        myplot.save(name)


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

        index = int(raw_input('Choose desired mass range: '))

        if index in validinputs:
            return index
        else:
            print('Your input was invalid. Please try again.')
