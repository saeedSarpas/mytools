"""myhmf.py

Main module for generating halo mass function
"""

import numpy as np


class MyHMF(object):
    """Halo mass function class"""

    def __init__(self, halomasses, boxlength):
        """Constructor for HMF class

        Parameters
        ----------
        halomasses : Array of float
            Masses of halos
        boxlength : float
            Length of the simulation box

        Attributes
        ----------
        self.halos : Array of float
            Halo masses
        self.m : Array of float
            Logarithmic centers of mass bins
        self.dndm : Array of float
            Halo mass function
        self.dndlnm : Array of float
            Halo mass function

        Methods
        -------
        hmf()
            Generating the halo mass function
        """

        self.halos = halomasses
        self.boxlength = boxlength
        self.m = []
        self.dndmdv = []
        self.dndlnmdv = []

    def hmf(self, nbins):
        """Calculating a logarithmic histogram of masses

        Parameters
        ----------
        nbins : int
            Number of bins
        key : str
        """

        bin_edges = np.logspace(
            np.log10(np.min(self.halos)),
            np.log10(np.max(self.halos)),
            num=nbins+1,
            base=10)

        n, hist = [], []

        for bmin, bmax in zip(bin_edges[:-1], bin_edges[1:]):
            binned_hosts = [h for h in self.halos if h >= bmin and h < bmax]
            n.append(len(binned_hosts))
            if len(binned_hosts) != 0:
                hist.append(np.mean(binned_hosts))
            else:
                hist.append(0)

        self.m = [
            10**((np.log10(minm * maxm)) / 2)
            for (minm, maxm) in zip(bin_edges[:-1], bin_edges[1:])]

        self.dndmdv = [
            nn / ((maxm - minm) * (self.boxlength ** 3))
            for (nn, minm, maxm) in zip(n, bin_edges[:-1], bin_edges[1:])]

        self.dndlnmdv = [
            nn / ((np.log(maxm) - np.log(minm)) * (self.boxlength ** 3))
            for (nn, minm, maxm) in zip(n, bin_edges[:-1], bin_edges[1:])]
