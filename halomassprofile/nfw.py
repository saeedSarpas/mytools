"""nfw.py"""

from math import pi
import numpy as np

def NFW(mass, r_vir, z, **kwargs):
    """calculating and plotting NFW profile

    Parameters
    ----------
    mass : float
        The mass of the halo
    rvir : float
        Virial radius of the halo
    z : float
        Redshift
    rbins : list, optional
        A list of (radial) bins edges

    Returns
    -------
    plot : dict
        NFW prfile generated points
    c : float
        Concentration
    rho_0 : float
        NFW rho_0 (initial density)
    """

    rbins = kwargs.get('rbins') if 'rbins' in kwargs else np.logspace(
        1, np.log10(3 * r_vir), num=20, base=10)

    # Calculating concentration from Dolag et. al. 2003 for LCDM
    c = 9.59 / (z + 1) * (mass / 10**14)**(-0.102)

    r_s = r_vir / c

    rho_0 = mass / (4 * pi * r_s**3 * (np.log(1 + c) - (c / (c + 1))))

    plot = {'x': [], 'y': []}
    for r in rbins:
        plot['x'].append(r)
        plot['y'].append(
            rho_0 / ((r / r_s) * (1 + (r / r_s))**2))

    return plot, c, rho_0
