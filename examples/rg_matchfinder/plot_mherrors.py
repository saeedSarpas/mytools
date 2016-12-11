"""plot-mherrors.py
Generating errors of different features of the matching halos
"""

from __future__ import print_function

import gc
import numpy as np

from math import sqrt

from ...examples.rg_matchfinder.mh import MatchingHalo
from ...halofinder.rockstar import Rockstar, NOTAVLBL
from ...visualization.myplot import MyPlot
from ...visualization.mycolordict import primarycolors


class MatchingHalosErrors(object):
    """GenerateErrors class"""

    def __init__(self, matchinghalosfile, halos1file, halos2file):
        """Initializing GenerateErrors object

        Parameters
        ----------
        matchinghalosfile : str
            path to the matchinghalos ascii file
        halos1file, halos2file : str
            path to the ascii primary and secondary halo files

        Examples
        --------
        >>> from mytools.examples.rockstargadgetmatchfinder.generateerrors \
        import GenerateErrors as G
        >>> from mytools.halofinders.rockstar import Rockstar as R
        >>> g = G('/path/to/matchinghalos, /path/to/pri_halo, /path/to/sec_halo)
        >>> g.generateerrors()
        >>> g.save('/path/to/output/file')
        """

        self.matchinghalos = MatchingHalo(matchinghalosfile)
        self.matchinghalos.load()

        self.halos1 = Rockstar(halos1file)
        self.halos1.load()
        self.halos1.sortbyid()

        self.halos2 = Rockstar(halos2file)
        self.halos2.load()
        self.halos2.sortbyid()

        self.datatype = []
        self.errors = []
        self.tags = []

    def generateerrors(self, only='none'):
        r"""Generating errors of halos features
        by definition, for a given halo feature, x, the error is:
        \err_x = (x_halo1 - x_halo2) / x_halo2

        Parameters
        ----------
        only : array of str, optional
            Array of tags to be calculated
        """
        self.datatype = self.halos1.datatype
        self.datatype.remove(self.halos1.datatype[0])

        num_matches = self.matchinghalos.header['num_of_found_matches']
        self.errors = np.zeros(num_matches, dtype=self.datatype)

        if only == 'none':
            self.tags = [self.datatype[i][0] for i in range(len(self.datatype))]
        else:
            self.tags = only

        mhd = self.matchinghalos.data
        err = self.errors
        h1 = self.halos1.halossortedbyid
        h2 = self.halos2.halossortedbyid

        for i in range(num_matches):
            for tag in self.tags:
                id1 = mhd[i]['id1']
                id2 = mhd[i]['id2']
                if h2[id2][tag] == 0:
                    self.tags.remove(tag)
                    print("[Error] Removed " + tag + "from tags")
                    print("        halos2[" + str(id2) + "][" + tag + "] = 0")
                    continue

                if h1[id1]['id'] != NOTAVLBL and h2[id2]['id'] != NOTAVLBL:
                    err[i][tag] = (h1[id1][tag] - h2[id2][tag]) / h2[id2][tag]
                else:
                    self.tags.remove(tag)
                    print("[Error] Removed " + tag + "from tags")
                    print("        halos1[" + str(id1) +"] or halos2[" +
                          str(id2) + "] is not available!")
                    continue

    def plot(self, directory):
        """Save the error into a file

        Parameters
        ----------
        directory : str
            path to the directory that plots should be saved inside it
        """

        plots = {}

        primarycolor = primarycolors('RAINBOW')
        _ = primarycolor.next()
        color = primarycolor.next()

        for tag in self.tags:
            plots[tag] = MyPlot()

            # Plotting the first axes, a scatter plot
            ax1 = plots[tag].new2daxes(pos='211')

            h1 = self.halos1.halossortedbyid
            h2 = self.halos2.halossortedbyid

            x99 = np.percentile(h2[tag], 99)
            x50 = np.percentile(h2[tag], 50)
            xscale = 'log' if x99/x50 >= 10 else 'linear'

            kws = {
                'xmin': np.min((np.min(h1[tag]), np.min(h2[tag]))),
                'xmax': np.max((np.max(h1[tag]), np.max(h2[tag]))),
                'ymin': -1 * np.percentile(-1 * self.errors[tag], 99.5) * 1.5,
                'ymax': np.percentile(self.errors[tag], 99.5) * 1.5,
                'xscale': xscale,
                'ylabel': '$\\Delta ' + str(tag) + '$',
                'color': color,
                'silent': True,
                'alpha': 0.25
            }

            xs = [h2[i][tag] for i in self.matchinghalos.data['id2']]
            plots[tag].scatter(xs, self.errors[tag], ax=ax1, **dict(kws))

            xmin = np.min(self.halos2.halos[tag])
            xmax = np.max(self.halos2.halos[tag])

            if xscale == 'log' and xmin > 0 and xmax > 0:
                binedges = np.logspace(np.log10(xmin), np.log10(xmax), num=20,
                                       base=10)
            else:
                binedges = np.linspace(xmin, xmax, num=20)

            ax2 = plots[tag].new2daxes(pos='212', sharex=ax1)
            plotparams = self._errors(tag, binedges, xscale)

            y = np.array(plotparams['y'])
            yerr = np.array(plotparams['yerr'])

            kws = {
                'ymin': np.min(y - yerr),
                'ymax': np.max(y + yerr),
                'color': color,
                'xlabel': '$' + str(tag) + '$',
                'shaded': True,
                'silent': True
            }

            plots[tag].errorbar(plotparams, ax=ax2, **dict(kws))

            plots[tag].plt.gcf().subplots_adjust(hspace=0.1)
            plots[tag].save(directory + tag + '.png')

            # Cleaning up...
            plots[tag].plt.clf()
            plots[tag].plt.close()
            del plots[tag]
            gc.collect()

    def _errors(self, tag, binedges, xscale):
        """Plot standard errors of a given quantity"""

        plotparams = {'x': [], 'y': [], 'xerr': None, 'yerr': []}

        h2 = self.halos2.halos
        h2sorted = [h2[i][tag] for i in self.matchinghalos.data['id2']]

        for bmin, bmax in zip(binedges[:-1], binedges[1:]):
            if xscale == 'log':
                plotparams['x'].append(10**(np.log10(bmin * bmax) / 2))
            else:
                plotparams['x'].append((bmax + bmin) / 2)

            values = [self.errors[i][tag] for i in range(len(h2sorted))
                      if bmax > h2sorted[i] >= bmin]

            if len(values) != 0:
                mean = sum(values) / len(values)
                plotparams['y'].append(mean)
                plotparams['yerr'].append(
                    sqrt(sum([(v - mean)**2 for v in values]) / len(values)))
            else:
                plotparams['y'].append(0)
                plotparams['yerr'].append(0)

        return plotparams
