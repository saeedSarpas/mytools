"""myhmp.py

Main module for generating halo mass profile
"""
import sys

from math import sqrt, pi
from scipy.interpolate import InterpolatedUnivariateSpline as interpolate
import numpy as np


class MyHMP(object):
    """MyHMP (Halo mass profile) class"""

    def __init__(self, halos, sorted_particles, p_mass):
        """Constructor for MyHMP

        Parameters
        ----------
        halos : numpy.ndarray
            Numpy array of halos data containing x, y, z, rvir
        sorted_particles : numpy.ndarray
            Array of particles sorted  based on their x position
        particles_mass : float
            Particles' mass
        cosmo : str, optional
            Name of cosmology to use (from astropy package)

        Attributes
        ----------
        self.halos : numpy.ndarray
            Array of halos
        self.particles : numpy.ndarray
            Sorted array of particles
        self.pmass : float
            Mass of particles
        self.profiles : 2-d array
            Array of halos profiles
        self.rho_rhocrit : array of float
            Array of rho / rho_crit
        self.rr200 : array of float
            Array of r / r200
        self.r : array of float
            Array of radii
        """

        self.halos = halos
        self.particles = sorted_particles
        self.pmass = p_mass
        self.profiles = []
        self.rho_rhocrit = []
        self.rr200 = []
        self.r = []

    def hmp(self, rbins, rhocritperh):
        """Generating halo mass profiles of each halo

        Parameters
        ----------
        rbins : array of float
            Radius bin edges
        rhocritperh : float
            Critical density
        """

        self.profiles = []
        self.r = rbins

        for i, halo in enumerate(self.halos):
            _write('\rGenerating halo mass profile [%d out of %d]',
                   (i + 1, len(self.halos)))

            sorted_distances = _sorteddistances(halo, self.particles)

            self.profiles.append(_densitycalc(
                sorted_distances, rbins, self.pmass, rhocritperh))
        _write('[done]\n')

        # Stacking profiles
        self.rho_rhocrit = []

        for i in range(len(self.r)):
            self.rho_rhocrit.append(
                sum([d[i] for d in self.profiles]) / len(self.profiles))

        # Finding r200
        reduced_profile = [r - 200.0 for r in self.rho_rhocrit]
        profile_func = interpolate(self.r, reduced_profile)

        r200 = profile_func.roots()[0]

        self.rr200 = [r / r200 for r in self.r]


def _sorteddistances(halo, sorted_particles):
    """Calculating the distance of particles to the center of the halo

    Parameters
    ----------
    halo : numpy.array
    sorted_particles : numpy.ndarray

    Returns
    -------
    array of float
    """

    result = []

    x, r = halo['x'], halo['rvir']

    minid = np.searchsorted(sorted_particles['x'], x - r)
    maxid = np.searchsorted(sorted_particles['x'], x + r)

    for i in range(minid, maxid):
        y, z = halo['y'], halo['z']

        if(y - r < sorted_particles['y'][i] < y + r and
           z - r < sorted_particles['z'][i] < z + r):

            dx2 = (sorted_particles['x'][i] - x)**2
            dy2 = (sorted_particles['y'][i] - y)**2
            dz2 = (sorted_particles['z'][i] - z)**2

            result.append(sqrt(dx2 + dy2 + dz2))

    return np.sort(result)


def _densitycalc(sorted_distances, rbins, pmass, rhocritperh):
    """Calculating halo mass profile

    Parameters
    ----------
    sorted_distances : array of float
    rbins : array of float
    pmass : float
        Particles' mass
    rhocrit : float
        Critical density

    Returns
    -------
    array of float
    """

    result = []

    for r in rbins:
        nparticles = np.searchsorted(sorted_distances, r, side='right') + 1
        result.append(nparticles * pmass / (4/3 * pi * r**3) / rhocritperh)

    return result


def _write(text, args=()):
    """Printing into stdout using sys.stdout.write"""
    sys.stdout.write(text % args)
    sys.stdout.flush()
