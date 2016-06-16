"""halomassprofile.py
Plotting the halo mass profile"""

from __future__                       import print_function
from math                             import sqrt, pi

import sys
import numpy                          as np
import matplotlib.pyplot              as plt

from ..sharedtools                    import headerextractor
from ..sharedtools.plotting           import plotline, errorbars, save
from ..sharedtools.physicalparameters import rhocrit
from ..sharedtools.gadgetreader       import Gadget
from ..sharedtools.reportgenerator    import Report

class Rockstar(object):
    """Handling necessary actions for generating halo mass profile for
    Rockstar output files"""

    def __init__(self, rockstar_fname, gadget_fname, **kwargs):
        """Initializing and extracting data for plotting halo mass profile.
        Note that the normal output of Rockstar does not contain the parent
        id of the halos, for generating outputs with parent ids, run:
        `./util/find_parent <normal rockstar output path>`
        from the root directory of the Rockstar project"""
        k = kwargs.get

        self.names = {
            'rfname': rockstar_fname,
            'gfname': gadget_fname,
            'plot': '',
            'bin': ''}

        self.sortedparticles = []
        self.plotparams = {} # 'x': [], 'y': [], 'xerr': None, 'yerr': []
        self.hosts = {}
        self.rbins = []

        # Extracting header information
        self.headers = headerextractor.run(rockstar_fname, 'rockstar')
        # [TODO]: Extracting positions unit from Rockstar header
        self.lratio = 1000 # Mpc to kpc

        column_num = {}
        column_num['id']   = k('id_col')   if 'id_col'   in kwargs else 0
        column_num['mass'] = k('mass_col') if 'mass_col' in kwargs else 3
        column_num['r']    = k('r_col')    if 'r_col'    in kwargs else 4
        column_num['x']    = k('x_col')    if 'x_col'    in kwargs else 8
        column_num['y']    = k('y_col')    if 'y_col'    in kwargs else 9
        column_num['z']    = k('z_col')    if 'z_col'    in kwargs else 10
        column_num['pid']  = k('pid_col')  if 'pid_col'  in kwargs else -1

        print('Extracting Rockstar data')
        for key, value in column_num.iteritems():
            print('\t{:8}\t{:4}\t{:15}'.format(
                str(key) + '_col', str(value), self.headers['column_tags'][value]))

        datatype = [('id',   np.int32),
                    ('mass', np.float64),
                    ('r',    np.float64),
                    ('x',    np.float64),
                    ('y',    np.float64),
                    ('z',    np.float64),
                    ('pid',  np.int32)]

        # Extracting data from rockstar output
        data = np.genfromtxt(
            rockstar_fname,
            unpack='true',
            skiprows=19,
            usecols=(
                column_num['id'],
                column_num['mass'],
                column_num['r'],
                column_num['x'],
                column_num['y'],
                column_num['z'],
                column_num['pid']),
            dtype=datatype)

        # Filter subhalos
        self.rockstar = np.array(
            [h for h in data if h['pid'] == -1], dtype=datatype)

    def selecthosts(self, **kwargs):
        """Calculating mass profile of hosts inside a logarithmic mass bin and
        saving the result in self.profiles dictionary"""

        k = kwargs.get

        nmassbins = k('nmassbins') if 'nmassbins' in kwargs else 12

        print('Binning host masses using following parameter(s)')
        print('\tNumber_of_mass_bins:\t' + str(nmassbins))
        print('')

        self.hosts = _binninghosts(self.rockstar, nmassbins)
        self.names['bin'] = _selectbin(self.hosts)

        hostmasses = [m['mass'] for m in self.hosts[self.names['bin']]]
        self.headers['Min_halo_mass_in_bin'] = np.min(hostmasses)
        self.headers['Max_halo_mass_in_bin'] = np.max(hostmasses)
        self.headers['Number_of_hosts'] = len(self.hosts[self.names['bin']])

    def loadgadgetsnap(self):
        """Loading sorted Gadget snapshot"""

        _write("Loading and indexing gadget snapshot (hang tight, it may take some time)")
        self.sortedparticles, self.headers['scale_factor'] = _loadgadgetsnap(
            self.names['gfname'])
        _write(" [done]\n")

    def generateprofiles(self, **kwargs):
        """Calculating mass profile of the selected bin"""

        total_particles = int(self.headers['Total_particles_processed'][0])
        if len(self.sortedparticles) != total_particles:
            print('[error] Sorted particle positions are not available.')
            print('        Make sure to run `Rockstar.loadgadgetsnap()` first.')
            return

        if not self.hosts or not self.names['bin']:
            print('[error] Hosts are not collected yet.')
            print('        Make sure to run `Rockstar.selecthosts()` first.')
            return

        k = kwargs.get
        hosts = self.hosts[self.names['bin']]

        nrbins = k('nrbins') if 'nrbins' in kwargs else 50

        ## rmin: half of force softening length
        ## rmax: twice the size of the biggest R_vir
        softening = float(self.headers['Force_resolution_assumed'][0])
        rmin = softening * self.lratio / 2
        rmax = 2 * np.max([h['r'] for h in hosts])

        self.rbins = np.logspace(
            np.log10(rmin), np.log10(rmax), num=nrbins, base=10)

        # Mass of particles.
        mparticlesperh = float(self.headers['Particle_mass'][0])

        # rho_crit. unit: M_sun / kpc^3 / h
        rhocritperh = rhocrit(
            float(self.headers['h'][0]), munit='msun', lunit='kpc', perh=True)

        tmp = {}
        for idx, host in enumerate(hosts):
            _write('\rGenerating halo mass profiles [%d out of %d]',
                   (idx + 1, self.headers['Number_of_hosts']))

            key = str(host['mass'])
            tmp[key] = {}
            tmp[key]['dtohost'] = _sorteddtohost(
                host, self.sortedparticles, self.lratio)
            tmp[key]['profile'] = _densitycalc(
                tmp[key]['dtohost'],
                self.rbins,
                mparticlesperh,
                rhocritperh)
        _write(' [done]\n')

        self.plotparams = _stackprofiles(tmp, self.rbins)

    def plot(self, **kwargs):
        """Callin _plot function for plotting the halo mass profile"""

        if not self.plotparams:
            print('[error] Profiles data are not available.')
            print('        Make sure to run `Rockstar.profilesgen()` first.')
            return

        kws = dict(kwargs)

        print('Parameters review:')
        for key, value in self.headers.iteritems():
            if key is not 'column_tags':
                print('\t{:15}\t{:15}'.format(str(key), str(value)))
        print('')

        if 'save' not in kws or ('save' in kws and kws['save'] is not True):
            print('[NOTE] In case you want to save your plot, make sure to set')
            print('       `save=True` when you are calling plot function.')
            print('')

        if 'xscale' not in kws: kws['xscale'] = 'log'
        if 'yscale' not in kws: kws['yscale'] = 'log'
        if 'xlabel' not in kws: kws['xlabel'] = '$\\log_{10}(r / kpc)$'
        if 'ylabel' not in kws: kws['ylabel'] = '$\\log_{10}(\\rho / \\rho_{crit})$'

        errorbarsplt = errorbars(plt, self.plotparams, **dict(kws))

        if 'nfw' in kwargs and kwargs.get('nfw') is True:
            for i in range(len(self.rbins)):
                if self.plotparams['y'][i] < 200:
                    r_vir = (self.plotparams['x'][i-1] + self.plotparams['x'][i]) / 2
            _, c, rho_0 = _plotnfw(errorbarsplt,
                                   float(self.names['bin']),
                                   self.rbins,
                                   r_vir,
                                   float(self.headers['scale_factor']),
                                   **dict(kws))
            self.headers['NFW_c'] = c
            self.headers['NFW_rho_0'] = rho_0

        if 'save' in kws and kws['save'] is True:
            self.names['plot'] = kws['name'] if 'name' in kws else self.names['rfname']
            save(errorbarsplt, self.names['plot'], '.png')

        if 'show' in kws and kws['show'] is True:
            errorbarsplt.show()

    def report(self):
        """Generating a short report of the result"""

        if not self.names['plot']:
            print('[error] The plot has not saved on disk yet.')
            print('        Make sure to run `Rockstar.plot(save=True)` first.')
            return

        report = Report(
            self.names['plot'],
            'Halo mass function',
            authors=['Saeed Sarpas'],
            emails=['saeed@astro.uni-bonn.de'])

        report.section(self.names['rfname'])
        report.addfigure(self.names['plot'] + '.png',
                         specifier='h',
                         attrs='width=0.6\\textwidth',
                         caption='')
        report.addtableofadict(self.headers, specifier='h',
                               excludekeys=['column_tags'])
        report.finish()


def _loadgadgetsnap(fname):
    """Loading and indexing Gadget snapshot"""
    gadget = Gadget(fname)
    gadget.load()
    sortedx = gadget.index('x')
    return sortedx, gadget.headers['time']


def _binninghosts(hosts, nmassbins):
    """Binning hosts"""
    result = {}

    mbins = np.logspace(
        np.log10(np.min(hosts['mass'])),
        np.log10(np.max(hosts['mass'])),
        num=nmassbins+1,
        base=10)

    for mmin, mmax in zip(mbins[:-1], mbins[1:]):
        key = 10**((np.log10(mmin * mmax)) / 2)
        result[str(key)] = [h for h in hosts if mmin < h['mass'] <= mmax]

    return result

def _selectbin(binnedhosts):
    """Getting desired bin to profile from user"""
    keys = binnedhosts.keys()

    while True:
        print('Index\tAverage host mass\t# of found hosts')
        validinputs = []
        for idx, key in enumerate(keys):
            nfoundhosts = len(binnedhosts[key])
            strindex = str(idx) if nfoundhosts > 1 else ' '
            validinputs.append(idx)
            print(strindex + '\t' + key + '   \t' + str(nfoundhosts))

        index = int(raw_input('Choose one index: '))
        if index in validinputs:
            return keys[index]
        else:
            print('Your input was invalid. Please try again.')

def _sorteddtohost(host, sorted_particles, lratio):
    """Calculating the particles distance to their hosts"""
    result = []
    x, r = host['x'] * lratio, host['r']
    minid = np.searchsorted(sorted_particles['x'], x - r)
    maxid = np.searchsorted(sorted_particles['x'], x + r)
    for i in range(minid, maxid):
        y, z = host['y'] * lratio, host['z'] * lratio
        if(y - r < sorted_particles['y'][i] < y + r
           and z - r < sorted_particles['z'][i] < z + r):
            dx2 = (sorted_particles['x'][i] - x)**2
            dy2 = (sorted_particles['y'][i] - y)**2
            dz2 = (sorted_particles['z'][i] - z)**2
            result.append(sqrt(dx2 + dy2 + dz2))
    return np.sort(result)

def _densitycalc(sorteddtohost, rbins, mparticles, rho_crit):
    """Calculating halo mass profile"""
    result = []
    for r in rbins:
        # Index of the last particle with dtohost smaller or equal to r
        nparticles = np.searchsorted(sorteddtohost, r, side='right')
        # density unit: M_sun / kpc^3 / h
        density_perh = nparticles * mparticles / (4/3 * pi * r**3)
        result.append(density_perh / rho_crit)
    return result

def _stackprofiles(hosts, rbins):
    """Stacking all profiles together"""

    result = {'x': [], 'y': [], 'xerr': None, 'yerr': []}

    for i in range(len(rbins)):
        rhos = []
        for _, host in hosts.iteritems():
            rhos.append(host['profile'][i])
        rho_mean = sum(rhos) / len(rbins)
        result['y'].append(rho_mean)
        rho_error = sum([sqrt((r  - rho_mean)**2) for r in rhos]) / len(rbins)
        if rho_error < rho_mean:
            result['yerr'].append(rho_error)
        else:
            result['yerr'].append(.99999 * rho_mean)

    result['x'] = rbins
    return result

def _write(text, args=()):
    """Printing into stdout using sys.stdout.write"""
    sys.stdout.write(text % args)
    sys.stdout.flush()

def _plotnfw(pyplotobj, mass, rbins, r_vir, a, **kwargs):
    """Generating NFW profile"""

    # Calculating concentration from Dolag et. al. 2003 for LCDM
    c = a * 9.59 * (mass / 10**14)**(-0.102)

    r_s = r_vir / c

    rho_0 = mass / (4 * pi * r_s**3 * (np.log(1 + c) - (c / (c + 1))))

    plot = {'x': [], 'y': []}
    for r in rbins:
        plot['x'].append(r)
        plot['y'].append(
            rho_0 / ((r / r_s) * (1 + (r / r_s))**2))

    nfwplt = plotline(pyplotobj, plot, **kwargs)

    return nfwplt, c, rho_0
