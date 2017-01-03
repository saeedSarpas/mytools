"""mherrors.py"""

from __future__ import print_function

import gc
import numpy as np
from scipy import stats

from math import sqrt, acos

from ...examples.rg_matchfinder.mh import MatchingHalo
from ...halofinder.rockstar import Rockstar, NOTAVLBL
from ...visualization.myplot import MyPlot
from ...visualization.mycolordict import get

MINNUMOFBINPOINTS = 100
MAXNUMOFPARTS = 3181904
MINNUMOFPARTS = 160


class MHErrors(object):
    """Generating different errors based on matching halos"""

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
        >>> from mytools.examples.rg_matchfinder.mherrors import MHErrors as MHE
        >>> mhe = MHE('/path/to/matchinghalos',
                      '/path/to/pri_halo',
                      '/path/to/sec_halo')
        >>> mhe.plotall(path='./directory/', confactor=8)
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


    def plotall(self, path='./', lowsymb='l', highsymb='h', confactor=None):
        """Plotting all available properties"""

        self.plot(path=path, lowsymb=lowsymb, highsymb=highsymb,
                  confactor=confactor)
        self.plotconcentration(path=path, lowsymb=lowsymb, highsymb=highsymb,
                               confactor=confactor)
        self.plotvector(path=path, lowsymb=lowsymb, highsymb=highsymb,
                        confactor=confactor)
        self.plotvs(path=path, lowsymb=lowsymb, highsymb=highsymb)


    def plot(self, path='./', only=None, lowsymb='l', highsymb='h',
             confactor=None):
        """Plotting regular properties"""

        PLOTS = _genplots(lowsymb, highsymb)

        properties = PLOTS.keys() if only is None else only

        for prop in properties:
            try:
                e = self._generrors(prop)
            except KeyError:
                print(prop + " is not available!")
                continue

            print('Plotting ' + prop + '...')

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
                       PLOTS[prop]['ylabel'],
                       path + prop + '_' + lowsymb + '.png',
                       confactor=confactor,
                       yscale=yscale,
                       lowsymb=lowsymb,
                       highsymb=highsymb)


    def plotvector(self, path='./', only=None, lowsymb='l', highsymb='h',
                   confactor=None):
        """Plotting available moduli such as positions, velocities, etc."""

        MODULI = _genmoduli(lowsymb, highsymb)

        properties = MODULI.keys() if only is None else only

        for prop in properties:
            try:
                e = self._genvectorerr(MODULI[prop]['input'])
            except KeyError:
                print("Not able to plot " + prop)
                continue

            print('Plotting ' + prop + '...')

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
                       MODULI[prop]['ylabel'],
                       path + prop + '_' + lowsymb + '.png',
                       confactor=confactor,
                       yscale=yscale,
                       lowsymb=lowsymb,
                       highsymb=highsymb)

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
                       MODULI[prop]['ylabel_angle'],
                       path + prop + '_angledist_' + lowsymb + '.png',
                       confactor=confactor,
                       yscale=yscale_angle,
                       lowsymb=lowsymb,
                       highsymb=highsymb)


    def plotconcentration(self, path='./', lowsymb='l', highsymb='h',
                          confactor=None):
        """Plotting the concentration parameter, c"""

        print('Plotting concentration parameter, c...')

        e = self._genconcentration()

        self._plot(e['num_p_low'],
                   e['c_err'],
                   -1, 1,
                   r"$\Delta c / c^{" + highsymb + r"}$",
                   path +  'c_' + lowsymb + '.png',
                   confactor=confactor,
                   yscale='linear',
                   lowsymb=lowsymb,
                   highsymb=highsymb)



    def plotvs(self, path='./', lowsymb='l', highsymb='h'):
        """Plotting mvir vs. num_p for the high resolution run"""

        print('Plotting vs. plots...')

        myplot = MyPlot()

        kws = {
            'xlabel': '',
            'ylabel': '',
            'xscale': 'log',
            'yscale': 'log',
            'silent': True
            }

        # Plotting M_vir vs. num_p of the high resolution run
        xs1, ys1 = self.halos['high'].vs('mvir', 'num_p', nbins=201)

        kws['xlabel'] = r'$M_{vir}^{' + highsymb + r'} [M_{\odot} h^{-1}]$'
        kws['ylabel'] = r'$N_P^{' + highsymb + r'}$'
        myplot.plot({'x': xs1, 'y': ys1}, **dict(kws))

        myplot.save(path + 'mvir_vs_num_p_high.png')
        myplot.plt.clf()

        # Plotting M_vir vs. num_p of the low resolution run
        xs2, ys2 = self.halos['low'].vs('mvir', 'num_p', nbins=201, path=path)

        kws['xlabel'] = r'$M_{vir}^{' + lowsymb + r'} [M_{\odot} h^{-1}]$'
        kws['ylabel'] = r'$N_P^{' + lowsymb + r'}$'
        myplot.plot({'x': xs2, 'y': ys2}, **dict(kws))

        myplot.save(path + 'mvir_vs_num_p_low_' + lowsymb + '.png')
        myplot.plt.clf()

        # Plotting num_p_high vs. num_p_low
        num_p_low, num_p_high = self.nump_low_vs_nump_high(nbins=201)

        kws['xlabel'] = r'$N_P^{' + lowsymb + r'}$'
        kws['ylabel'] = r'$N_P^{' + highsymb + r'}$'

        myplot.plot({'x': num_p_low, 'y': num_p_high}, **dict(kws))

        myplot.save(path + 'num_p_low_vs_num_p_high_' + lowsymb + '.png')
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
            if high[id2][prop] == 0: continue

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

            if v1 == 0 or v2 == 0: continue

            # Filling the output dictionary
            output['num_p_low'].append(low[id1]['num_p'])
            output['modulus_err'].append(sqrt(
                (low[id1][props[0]] - high[id2][props[0]])**2 +
                (low[id1][props[1]] - high[id2][props[1]])**2 +
                (low[id1][props[2]] - high[id2][props[2]])**2))

            output['angle_err'].append(acos(v1dotv2 / (v1 * v2)))

        return output


    def _genconcentration(self):
        """Generating concentration parameter, c"""
        output = {'c_err': [], 'num_p_low': []}

        low = self.halos['low'].halossortedbyid
        high = self.halos['high'].halossortedbyid

        for id1, id2 in zip(self.matches.data['id1'], self.matches.data['id2']):
            # Checking the availability of the data
            if id1 is NOTAVLBL or id2 is NOTAVLBL: continue
            if low[id1]['Rs'] == 0 or high[id2]['Rs'] == 0: continue
            if high[id2]['rvir'] == 0: continue

            c_high = high[id2]['rvir'] / high[id2]['Rs']
            c_low = low[id1]['rvir'] / low[id1]['Rs']

            output['num_p_low'].append(low[id1]['num_p'])
            output['c_err'].append((c_low - c_high) / c_high)

        return output


    def _plot(self, num_p, err, ymin, ymax, ylabel, path, confactor=None,
              yscale='linear', lowsymb='l', highsymb='h'):
        """Plotting a scatter plot

        Parameters
        ----------
        num_p : array of float
        err : array of float
        ymin, ymax : float
        ylabel : str
        path : str
        confactor : float, optional
            Conversion factor, converting num_p in high res to num_p in low res
        yscale : str, optional
        lowsymb, highsymb : str, optional
        """

        myplot = MyPlot()
        axes = myplot.new2daxes()

        xmin = np.min(num_p)
        xmax = np.max(num_p)

        num_p_low, num_p_high = self.nump_low_vs_nump_high()

        if confactor is None:
            print('[Error!] Conversion factor is not available!')
            confactor = stats.linregress(num_p_low, num_p_high).slope
            print('         confactor set to ' + str(confactor))

        xmin_box = MINNUMOFPARTS / confactor
        xmax_box = MAXNUMOFPARTS / confactor

        color = get('RAINBOW')['primary_colors'][1]
        shadow = get('RAINBOW')['primary_shadows'][0]

        scatterkws = {
            'xmax': xmax_box,
            'xmin': xmin_box,
            'ymax': ymax,
            'ymin': ymin,
            'xscale': 'log',
            'yscale': yscale,
            'xlabel': "$N_P^{" + lowsymb + "}$",
            'ylabel': ylabel,
            'color': color,
            'silent': True,
            'alpha': 0.25
        }

        myplot.scatter(num_p, err, ax=axes, s=1, **dict(scatterkws))

        # Plotting zero line
        linekws = {
            'color': get('RAINBOW')['axiscolor'],
            'alphs': 0.5,
            'silent': True
        }

        myplot.plot({'x': [xmin, xmax], 'y': [0, 0]}, ax=axes, **dict(linekws))

        # Plotting shaded percentiles
        binedges = np.logspace(np.log10(xmin), np.log10(xmax), num=27, base=10)

        x, y, ylow, yhigh = [], [], [], []

        bmin = binedges[0]
        for bmax in binedges[1:]:
            errs = [err[i] for i in range(len(err)) if bmax >= num_p[i] > bmin]

            if len(errs) < MINNUMOFBINPOINTS and bmax != binedges[-1]:
                continue

            if len(errs) != 0:
                x.append(10**(np.log10(bmin * bmax) / 2))
                y.append(np.percentile(errs, 50))
                ylow.append(np.percentile(errs, 15.9))
                yhigh.append(np.percentile(errs, 84.1))

            bmin = bmax

        # Stretching the x vector to the boundaries of num_p
        x[0] = xmin
        x[-1] = xmax

        fill_betweenkws = {
            'xmax': xmax_box,
            'xmin': xmin_box,
            'ymax': ymax,
            'ymin': ymin,
            'xscale': 'log',
            'yscale': yscale,
            'silent': True
        }
        myplot.fill_between(x, ylow, yhigh, shadow, ax=axes,
                            alpha=0.6, **dict(fill_betweenkws))

        # Plotting the median
        plotkws = {
            'xmax': xmax_box,
            'xmin': xmin_box,
            'ymax': ymax,
            'ymin': ymin,
            'xscale': 'log',
            'yscale': yscale,
            'color': '#000000',
            'silent': True
        }
        myplot.plot({'x': x, 'y': y}, ax=axes, **dict(plotkws))

        # Second x axis

        min_pow = int(np.log10(MINNUMOFPARTS))
        max_pow = int(np.log10(MAXNUMOFPARTS))

        xticks = [10**i for i in range(min_pow, max_pow + 1)]

        ticksloc = [x / confactor for x in xticks]
        tickslabels = ['$10^{' + str(i) + '}$'
                       for i in range(min_pow, max_pow + 1)]

        myplot.coaxis(ticksloc, tickslabels, "$N_P^{" + highsymb + "}$",
                      axes=axes, scale='log', showminorticks=False)

        myplot.save(path)

        # Cleaning up...
        myplot.plt.clf()
        gc.collect()


def _genplots(lowsymb, highsymb):
    """Generating PLOTS dictionary"""

    return {
        'mvir': {
            'ylabel': r"$\Delta M_{vir} / M_{vir}^{" + highsymb + r"}$",
            'ymax': 1,
            'ymin': -1
        },
        'mbound_vir': {
            'ylabel': r"$\Delta M_{b\_vir} / M_{b\_vir}^{" + highsymb + r"}$",
            'ymax': 1,
            'ymin': -1
        },
        'rvir': {
            'ylabel': r"$\Delta R_{vir} / R_{vir}^{" + highsymb + r"}$",
            'ymax': 0.4,
            'ymin': -0.4
        },
        'vmax': {
            'ylabel': r"$\Delta V_{max} / V_{max}^{" + highsymb + r"}$",
            'ymax': 0.5,
            'ymin': -0.5
        },
        'rvmax': {
            'ylabel': r"$\Delta R_{V(max)} / R_{V(max)}^{" + highsymb + r"}}$",
            'ymax': 4,
            'ymin': -1
        },
        'vrms': {
            'ylabel': r"$\Delta V_{RMS} / V_{RMS}^{" + highsymb + r"}$",
            'ymax': 0.4,
            'ymin': -0.6
        },
        'm200b': {
            'ylabel': r"$\Delta M_{200b} / M_{200b}^{" + highsymb + r"}$",
            'ymax': 1,
            'ymin': -1
        },
        'm200c': {
            'ylabel': r"$\Delta M_{200c} / M_{200c}^{" + highsymb + r"}$",
            'ymax': 1,
            'ymin': -1
        },
        'm500c': {
            'ylabel': r"$\Delta M_{500c} / M_{500c}^{" + highsymb + r"}$",
            'ymax': 1,
            'ymin': -1
        },
        'm2500c': {
            'ylabel': r"$\Delta M_{2500c} / M_{2500c}^{" + highsymb + r"}$",
            'ymax': 0.75,
            'ymin': -1
        },
        'Xoff': {
            'ylabel': r"$\Delta Xoff / Xoff^{" + highsymb + r"}$",
            'ymax': 10,
            'ymin': -1
        },
        'Voff': {
            'ylabel': r"$\Delta Voff / Voff^{" + highsymb + r"}$",
            'ymax': 5,
            'ymin': -1
        },
        'b_to_a': {
            'ylabel': r"$\Delta \left(\frac{b}{a}\right) / \left(\frac{b}{a}\right)^{" + highsymb + r"}$",
            'ymax': 0.4,
            'ymin': -1

        },
        'c_to_a': {
            'ylabel': r"$\Delta \left(\frac{c}{a}\right) / \left(\frac{c}{a}\right)^{" + highsymb + r"}$",
            'ymax': 0.4,
            'ymin': -1
        },
        'b_to_a(500c)': {
            'ylabel': r"$\Delta {\left(\frac{b}{a}\right)}_{500c} / {\left(\frac{b}{a}\right)}_{500c}^{" + highsymb + r"}$",
            'ymax': 0.4,
            'ymin': -1
        },
        'c_to_a(500c)': {
            'ylabel': r"$\Delta {\left(\frac{c}{a}\right)}_{500c} / {\left(\frac{c}{a}\right)}_{500c}^{" + highsymb + r"}$",
            'ymax': 0.4,
            'ymin': -1
        },
        'Rs': {
            'ylabel': r"$\Delta R_s / R_{s}^{" + highsymb + r"}$",
            'ymax': 3,
            'ymin': -1
        },
        'Halfmass_Radius': {
            'ylabel': r"$\Delta R_{(M/2)} / R_{(M/2)}^{" + highsymb + r"}$",
            'ymax': 2,
            'ymin': -1
        }
    }


def _genmoduli(lowsymb, highsymb):
    """Generating MODULI dictionary"""

    return {
        'pos': {
            'input': ['x', 'y', 'z'],
            'ylabel': r"$|\vec{X}^{" + highsymb + r"} - \vec{X}^{" + lowsymb + r"}| [Mpc\/h^{-1}]$",
            'ylabel_angle': r"$\angle \vec{X}^{" + highsymb + r"} \/ \vec{X}^{" + lowsymb + r"} \/ [Rad]$",
            'ymax': 0.6,
            'ymin': 0,
            'ymax_angle': 3.141592,
            'ymin_angle': 0
        },
        'vel': {
            'input': ['vx', 'vy', 'vz'],
            'ylabel': r"$|\vec{V}^{" + highsymb + r"} - \vec{V}^{" + lowsymb + r"}| [km\/s^{-1}]$",
            'ylabel_angle': r"$\angle \vec{V}^{" + highsymb + r"} \/ \vec{V}^{" + lowsymb + r"} \/ [Rad]$",
            'ymax': 150,
            'ymin': 0,
            'ymax_angle': 3.141592,
            'ymin_angle': 0
        },
        'ang_mom': {
            'input': ['Jx', 'Jy', 'Jz'],
            'ylabel': r"$|\vec{J}^{" + highsymb + r"} - \vec{J}^{" + lowsymb + r"}| [(M_{\odot} \/h^{-1}) (Mpc\/h^{-1}) (km\/s^{-1})]$",
            'ylabel_angle': r"$\angle \vec{J}^{" + highsymb + r"} \/ \vec{J}^{" + lowsymb + r"} \/ [Rad]$",
            'ymax': 1e14,
            'ymin': 0,
            'ymax_angle': 3.141592,
            'ymin_angle': 0,
            'yscale': 'log'
        }
    }
