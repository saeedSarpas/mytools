"""plot-mherrors.py
Generating errors of different features of the matching halos
"""

from __future__ import print_function

import gc
import numpy as np
from scipy import stats

from math import sqrt, acos

from ...examples.rg_matchfinder.mh import MatchingHalo
from ...halofinder.rockstar import Rockstar, NOTAVLBL
from ...visualization.myplot import MyPlot
from ...visualization.mycolordict import get


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

        self.matches = MatchingHalo(matchinghalosfile)
        self.matches.load()

        self.halos = {'low': None, 'high': None}

        self.halos['low'] = Rockstar(halos1file)
        self.halos['low'].load()
        self.halos['low'].sortbyid()

        self.halos['high'] = Rockstar(halos2file)
        self.halos['high'].load()
        self.halos['high'].sortbyid()

        self.tags = [field[0] for field in self.halos['low'].dtype]


    def plotall(self, path='./'):
        """Plotting all available properties"""

        self.plot(path=path)
        self.plotvector(path=path)
        self.plotvs(path=path)


    def plot(self, path='./', only=None):
        """Plotting regular properties"""

        properties = PLOTS.keys() if only is None else only

        for prop in properties:
            try:
                e = self._generrors(prop)
            except KeyError:
                print(prop + " is not available!")
                continue

            if 'ymin' in PLOTS[prop]:
                ymin = PLOTS[prop]['ymin']
            else:
                ymin = -1 * np.percentile(
                    -1 * np.array(e[prop + '_err']), 99.5) * 1.5

            if 'ymax' in PLOTS[prop]:
                ymax = PLOTS[prop]['ymax']
            else:
                ymax = np.percentile(e[prop + '_err'], 99.5) * 1.5

            yscale = 'linear' if 'yscale' not in PLOTS[prop] \
                     else PLOTS[prop]['yscale']

            self._plot(e['num_p_low'],
                       e[prop + '_err'],
                       ymin, ymax,
                       r"$num\_p_{(low)}$",
                       PLOTS[prop]['ylabel'],
                       path + prop + '.png',
                       yscale=yscale)


    def plotvector(self, path='./', only=None):
        """Plotting available moduli such as positions, velocities, etc."""

        properties = MODULI.keys() if only is None else only

        for prop in properties:
            try:
                e = self._genvectorerr(MODULI[prop]['input'])
            except KeyError:
                print("Not able to plot " + prop)
                continue

            # Plotting modulus distribution
            if 'ymin' in MODULI[prop]:
                ymin = MODULI[prop]['ymin']
            else:
                ymin = -1 * np.percentile(
                    -1 * np.array(e['modulus_err']), 99.5) * 1.5

            yscale = 'linear' if 'yscale' not in MODULI[prop] \
                     else MODULI[prop]['yscale']

            if 'ymax' in MODULI[prop]:
                ymax = MODULI[prop]['ymax']
            else:
                ymax = np.percentile(e['modulus_err'], 99.5) * 1.5

            self._plot(e['num_p_low'],
                       e['modulus_err'],
                       ymin, ymax,
                       r"$num\_p_{(low)}$",
                       MODULI[prop]['ylabel'],
                       path + prop + '.png',
                       yscale=yscale)

            # Plotting angle distribution
            if 'ymin_angle' in MODULI[prop]:
                ymin_angle = MODULI[prop]['ymin_angle']
            else:
                ymin_angle = -1 * np.percentile(
                    -1 * np.array(e['angle_err']), 99.5) * 1.5

            yscale_angle = 'linear' if 'yscale_angle' not in MODULI[prop] \
                           else MODULI[prop]['yscale_angle']

            if 'ymax_angle' in MODULI[prop]:
                ymax_angle = MODULI[prop]['ymax_angle']
            else:
                ymax_angle = np.percentile(e['angle_err'], 99.5) * 1.5

            self._plot(e['num_p_low'],
                       e['angle_err'],
                       ymin_angle, ymax_angle,
                       r"$num\_p_{(low)}$",
                       MODULI[prop]['ylabel_angle'],
                       path + prop + '_angledist.png',
                       yscale=yscale_angle)


    def plotvs(self, path='./'):
        """Plotting mvir vs. num_p for the high resolution run"""


        myplot = MyPlot()

        kws = {
            'xlabel': '',
            'ylabel': '',
            'xscale': 'log',
            'yscale': 'log',
            'silent': True
            }

        # Plotting M_vir vs. num_p of the high resolution run
        xs1, ys1 = self.halos['high'].vs('mvir', 'num_p', nbins=201, path=path)

        kws['xlabel'] = r'$M_{vir, high} [M_{\odot} h^{-1}]$'
        kws['ylabel'] = r'$num\_p_{(high)}$'
        myplot.plot({'x': xs1, 'y': ys1}, **dict(kws))

        myplot.save(path + 'mvir_vs_num_p_high.png')
        myplot.plt.clf()

        # Plotting M_vir vs. num_p of the low resolution run
        xs2, ys2 = self.halos['low'].vs('mvir', 'num_p', nbins=201, path=path)

        kws['xlabel'] = r'$M_{vir, low} [M_{\odot} h^{-1}]$'
        kws['ylabel'] = r'$num\_p_{(low)}$'
        myplot.plot({'x': xs2, 'y': ys2}, **dict(kws))

        myplot.save(path + 'mvir_vs_num_p_low.png')
        myplot.plt.clf()

        # Plotting num_p_high vs. num_p_low
        num_p_low, num_p_high = self.nump_low_vs_nump_high(nbins=201)

        kws['xlabel'] = r'$num\_p_{(low)}$'
        kws['ylabel'] = r'$num\_p_{(high)}$'

        myplot.plot({'x': num_p_low, 'y': num_p_high}, **dict(kws))

        myplot.save(path + 'num_p_low_vs_num_p_high.png')
        myplot.plt.clf()

        del myplot


    def nump_low_vs_nump_high(self, nbins=None):
        """Calculating a soften version of num_p for low and high resolution
        runs
        """
        vs = []

        low = self.halos['low'].halossortedbyid
        high = self.halos['high'].halossortedbyid

        for id1, id2 in zip(self.matches.data['id1'], self.matches.data['id2']):
            # Checking the availability of the data
            if id1 is NOTAVLBL or id2 is NOTAVLBL: continue
            vs.append((low[id1]['num_p'], high[id2]['num_p']))

        vs = np.array(vs, dtype=[('num_p_low', np.float64),
                                ('num_p_high', np.float64)])
        vs = np.sort(vs, order='num_p_low')

        if nbins is None or not isinstance(nbins, int):
            return  vs['num_p_low'], vs['num_p_high']
        else:
            xs, ys = [], []
            bins = np.logspace(np.log10(vs['num_p_low'][0]),
                               np.log10(vs['num_p_low'][-1]),
                               num=21, base=10)

            for bmin, bmax in zip(bins[:-1], bins[1:]):
                i_min = np.searchsorted(vs['num_p_low'], bmin, side='right')
                i_max = np.searchsorted(vs['num_p_low'], bmax, side='right')

                values = vs['num_p_high'][i_min:i_max]

                if len(values) == 0: continue

                xs.append(10**(np.log10(bmin * bmax) / 2))
                ys.append(sum(values) / len(values))

            return xs, ys



    def _generrors(self, prop):
        """Generating the errors for a given halo feature
        By definition, for a given halo feature, x, the error is:
        err_x = (x_low - x_high) / x_high

        Parameters
        ----------
        prop : string
            A specific property of halos provided by rockstar, e.g. mvir, rvir
        """

        if prop not in self.tags: raise KeyError

        output = {prop + '_err': [], 'num_p_low': []}

        low = self.halos['low'].halossortedbyid
        high = self.halos['high'].halossortedbyid

        for id1, id2 in zip(self.matches.data['id1'], self.matches.data['id2']):
            # Checking the availability of the data
            if id1 is NOTAVLBL or id2 is NOTAVLBL: continue
            if low[id1][prop] == 0 or high[id2][prop] == 0: continue

            # Filling the output dictionary
            output['num_p_low'].append(low[id1]['num_p'])
            output[prop + '_err'].append(
                (low[id1][prop] - high[id2][prop]) / float(high[id2][prop]))

        return output


    def _genvectorerr(self, props):
        """Generating position modulus and its errors

        Parameters
        ----------
        props : list of strings
            A list of properties. For example for generating position modulus
            error the elems list should be: ['x', 'y', 'z']
            The length of the list should be 3
        """

        if(props[0] and props[1] and props[2]) not in self.tags: raise KeyError

        output = {'modulus_err': [], 'angle_err': [], 'num_p_low': []}

        low = self.halos['low'].halossortedbyid
        high = self.halos['high'].halossortedbyid

        for id1, id2 in zip(self.matches.data['id1'], self.matches.data['id2']):
            # Checking the availability of the data
            if id1 is NOTAVLBL or id2 is NOTAVLBL: continue

            v1dotv2 = low[id1][props[0]] * high[id2][props[0]] + \
                      low[id1][props[1]] * high[id2][props[1]] + \
                      low[id1][props[2]] * high[id2][props[2]]

            v1 = sqrt(low[id1][props[0]]**2 +
                      low[id1][props[1]]**2 +
                      low[id1][props[2]]**2)

            v2 = sqrt(high[id2][props[0]]**2 +
                      high[id2][props[1]]**2 +
                      high[id2][props[2]]**2)

            if v1 == 0 or v2 ==0: continue

            # Filling the output dictionary
            output['num_p_low'].append(low[id1]['num_p'])
            output['modulus_err'].append(sqrt(
                (low[id1][props[0]] - high[id2][props[0]])**2 +
                (low[id1][props[1]] - high[id2][props[1]])**2 +
                (low[id1][props[2]] - high[id2][props[2]])**2))

            output['angle_err'].append(acos(v1dotv2 / (v1 * v2)))

        return output


    def _plot(self, num_p, err, ymin, ymax, xlabel,
              ylabel, path, yscale='linear'):
        """Plotting a scatter plot"""
        myplot = MyPlot()
        axes = myplot.new2daxes()

        xmin = np.min(num_p)
        xmax = np.max(num_p)

        color = get('RAINBOW')['primary_colors'][1]
        shadow = get('RAINBOW')['primary_shadows'][0]

        scatterkws = {
            'xmax': xmax,
            'xmin': xmin,
            'ymax': ymax,
            'ymin': ymin,
            'xscale': 'log',
            'yscale': yscale,
            'xlabel': xlabel,
            'ylabel': ylabel,
            'color': color,
            'silent': True,
            'alpha': 0.25
        }

        myplot.scatter(num_p, err, ax=axes, s=4, **dict(scatterkws))

        # Plotting shaded percentiles
        binedges = np.logspace(np.log10(xmin), np.log10(xmax), num=27, base=10)

        x, y, ylow, yhigh = [], [], [], []

        for bmin, bmax in zip(binedges[:-1], binedges[1:]):
            errs = [err[i] for i in range(len(err)) if bmax >= num_p[i] > bmin]

            if len(errs) != 0:
                x.append(10**(np.log10(bmin * bmax) / 2))
                y.append(np.percentile(errs, 50))
                ylow.append(np.percentile(errs, 15.9))
                yhigh.append(np.percentile(errs, 84.1))

        # Plotting a line at y = 0
        linekws = {
            'color': get('RAINBOW')['axiscolor'],
            'silent': True
        }

        myplot.plot({'x': [xmin, xmax], 'y': [0, 0]}, ax=axes, **dict(linekws))

        # Displacing the first x to the smallest num_p
        x[0] = xmin


        fill_betweenkws = {
            'xmax': xmax,
            'xmin': xmin,
            'ymax': ymax,
            'ymin': ymin,
            'xscale': 'log',
            'yscale': yscale,
            'silent': True
        }
        myplot.fill_between(x, ylow, yhigh, shadow, ax=axes,
                          alpha=0.5, **dict(fill_betweenkws))

        # Plotting line corresponding to 50% percentile
        plotkws = {
            'xmax': xmax,
            'xmin': xmin,
            'ymax': ymax,
            'ymin': ymin,
            'xscale': 'log',
            'yscale': yscale,
            'color': '#000000',
            'silent': True
        }
        myplot.plot({'x': x, 'y': y}, ax=axes, **dict(plotkws))

        # Second x axis
        num_p_low, num_p_high = self.nump_low_vs_nump_high(nbins=21)
        slope = stats.linregress(num_p_high, num_p_low).slope

        min_pow = int(np.log10(np.min(num_p_low) / slope))
        max_pow = int(np.log10(np.max(num_p_low) / slope))

        xticks = [10**i for i in range(min_pow, max_pow + 1)]

        ticksloc = [x * slope for x in xticks]
        tickslabels = ["$10^{" + str(i) + "}$"
                       for i in range(min_pow, max_pow + 1)]

        myplot.secondaxis(ticksloc, tickslabels, r"$num\_p_{(high)}$",
                          axes=axes, scale='log', showminorticks=False)

        myplot.save(path)

        # Cleaning up...
        myplot.plt.clf()
        gc.collect()


PLOTS = {
    'mvir': {
        'ylabel': r"$\Delta M_{vir} / M_{vir, high}$",
        'ymax': 1,
        'ymin': -1
    },
    'mbound_vir': {
        'ylabel': r"$\Delta M_{bound\_vir} / M_{bound\_vir, high}$",
        'ymax': 1,
        'ymin': -1
    },
    'rvir': {
        'ylabel': r"$\Delta R_{vir} / R_{vir, high}$",
        'ymax': 0.4,
        'ymin': -0.4
    },
    'vmax': {
        'ylabel': r"$\Delta V_{max} / V_{max, high}$",
        'ymax': 0.5,
        'ymin': -0.5
    },
    'rvmax': {
        'ylabel': r"$\Delta R(V_{max}) / R(V_{max})_{high}}$",
        'ymax': 4,
        'ymin': -1
    },
    'vrms': {
        'ylabel': r"$\Delta V_{RMS} / V_{RMS, high}$",
        'ymax': 0.4,
        'ymin': -0.6
    },
    'm200b': {
        'ylabel': r"$\Delta M_{200b} / M_{200b, high}$",
        'ymax': 1,
        'ymin': -1
    },
    'm200c': {
        'ylabel': r"$\Delta M_{200c} / M_{200c, high}$",
        'ymax': 1,
        'ymin': -1
    },
    'm500c': {
        'ylabel': r"$\Delta M_{500c} / M_{500c, high}$",
        'ymax': 1,
        'ymin': -1
    },
    'm2500c': {
        'ylabel': r"$\Delta M_{2500c} / M_{2500c, high}$",
        'ymax': 0.75,
        'ymin': -1
    },
    'Xoff': {
        'ylabel': r"$\Delta Xoff / Xoff_{high}}$",
        'ymax': 10,
        'ymin': -1
    },
    'Voff': {
        'ylabel': r"$\Delta Voff / Voff_{high}}$",
        'ymax': 5,
        'ymin': -1
    },
    'b_to_a': {
        'ylabel': r"$\Delta \left(\frac{b}{a}\right) / \left(\frac{b}{a}\right)_{high}}$",
        'ymax': 0.4,
        'ymin': -1

    },
    'c_to_a': {
        'ylabel': r"$\Delta \left(\frac{c}{a}\right) / \left(\frac{c}{a}\right)_{high}}$",
        'ymax': 0.4,
        'ymin': -1
    },
    'b_to_a(500c)': {
        'ylabel': r"$\Delta \left(\frac{b}{a}\right)_{500c} / \left(\frac{b}{a}\right)_{500c, high}$",
        'ymax': 0.4,
        'ymin': -1
    },
    'c_to_a(500c)': {
        'ylabel': r"$\Delta \left(\frac{c}{a}\right)_{500c} / \left(\frac{c}{a}\right)_{500c, high}$",
        'ymax': 0.4,
        'ymin': -1
    },
    'Rs': {
        'ylabel': r"$\Delta R_s / R_{s, high}}$",
        'ymax': 3,
        'ymin': -1
    },
    'Halfmass_Radius': {
        'ylabel': r"$\Delta R_{(M/2)} / R_{(M/2), high}$",
        'ymax': 2,
        'ymin': -1
    }
}


MODULI = {
    'pos': {
        'input': ['x', 'y', 'z'],
        'ylabel': r"$|\vec{X}_{high} - \vec{X}_{low}| [Mpc\/h^{-1}]$",
        'ylabel_angle': r"$\angle \vec{X}_{high} \/ \vec{X}_{low} \/ [Rad]$",
        'ymax': 0.6,
        'ymin': 0,
        'ymax_angle': 3.141592,
        'ymin_angle': 0
    },
    'vel': {
        'input': ['vx', 'vy', 'vz'],
        'ylabel': r"$|\vec{V}_{high} - \vec{V}_{low}| [km\/s^{-1}]$",
        'ylabel_angle': r"$\angle \vec{V}_{high} \/ \vec{V}_{low} \/ [Rad]$",
        'ymax': 150,
        'ymin': 0,
        'ymax_angle': 3.141592,
        'ymin_angle': 0
    },
    'ang_mom': {
        'input': ['Jx', 'Jy', 'Jz'],
        'ylabel': r"$|\vec{J}_{high} - \vec{J}_{low}| [(M_{\odot} \/h^{-1}) (Mpc\/h^{-1}) (km\/s^{-1})]$",
        'ylabel_angle': r"$\angle \vec{J}_{high} \/ \vec{J}_{low} \/ [Rad]$",
        'ymax': 1e14,
        'ymin': 0,
        'ymax_angle': 3.141592,
        'ymin_angle': 0,
        'yscale': 'log'
    }
}
