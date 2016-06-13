"""halomassprofile.py
Plotting the halo mass profile"""

from math                             import sqrt, pi

import sys
import numpy                          as np

from ..sharedtools                    import headerextractor
from ..sharedtools.plotting           import errorbars
from ..sharedtools.physicalparameters import rhocrit
from ..sharedtools.gadgetreader       import Gadget

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

        self.rfname = rockstar_fname
        self.gfname = gadget_fname
        self.binnedhosts = []
        self.plotparams = {} # 'x': [], 'y': [], 'xerr': None, 'yerr': []
        self.hosts = {}
        self.rbins = []

        # Extracting header information
        self.headers = headerextractor.run(rockstar_fname, 'rockstar')
        # [TODO]: Extracting positions unit from Rockstar header
        self.lratio = 1000

        column_num = {}
        column_num['id']   = k('id_col')   if 'id_col'   in kwargs else 0
        column_num['mass'] = k('mass_col') if 'mass_col' in kwargs else 3
        column_num['r']    = k('r_col')    if 'r_col'    in kwargs else 4
        column_num['x']    = k('x_col')    if 'x_col'    in kwargs else 8
        column_num['y']    = k('y_col')    if 'y_col'    in kwargs else 9
        column_num['z']    = k('z_col')    if 'z_col'    in kwargs else 10
        column_num['pid']  = k('pid_col')  if 'pid_col'  in kwargs else -1

        skiprows = k('skiprows') if 'skiprows' in kwargs else 19

        print('Extracting follwing data from Rockstar by skipping ' \
              + str(skiprows) + ' rows:')
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
            skiprows=skiprows,
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
        self.data = np.array(
            [h for h in data if h['pid'] == -1], dtype=datatype)

    def selecthosts(self, **kwargs):
        """Calculating mass profile of hosts inside a logarithmic mass bin and
        saving the result in self.profiles dictionary"""

        k = kwargs.get

        nmassbins = k('nmassbins') if 'nmassbins' in kwargs else 8

        print('Binning host masses using following parameter(s)')
        print('\tNumber_of_mass_bins:\t' + str(nmassbins))
        print('')

        binnedhosts = _binninghosts(self.data, nmassbins)

        binkey = _selectbin(binnedhosts)
        self.binnedhosts = binnedhosts[binkey]

        self.headers['Number_of_hosts'] = len(self.binnedhosts)

    def generateprofiles(self, **kwargs):
        """Calculating mass profile of the selected bin"""

        if not self.binnedhosts:
            print('[error] Hosts are not collected yet.')
            print('        Make sure to run `Rockstar.selecthosts()` first.')
            return

        k = kwargs.get

        _write('Loading Gadget snapthot (Hang tight, it may take some time)')
        gadget = Gadget(self.gfname)
        self.headers['Scale_factor'] = gadget.headers['time']
        gadget.load()
        _write(' [done]\n')

        _write('Indexing positions (Hang tight, it may take some time)')
        sortedx = gadget.index('x')
        _write(' [done]\n')

        # Radial binning
        nrbins = k('nrbins') if 'nrbins' in kwargs else 50
        ## rmin: half of force softening length
        ## rmax: twice the size of the biggest R_vir
        rmin = float(self.headers['Force_resolution_assumed'][0]) * self.lratio / 2
        rmax = 2 * np.max([h['r'] for h in self.binnedhosts])
        self.rbins = np.logspace(
            np.log10(rmin), np.log10(rmax), num=nrbins, base = 10)

        # Mass of particles
        mparticlesperh = float(self.headers['Particle_mass'][0])

        # rho_crit. unit: M_sun / kpc^3 / h
        rhocritperh = rhocrit(
            float(self.headers['h'][0]), munit='msun', lunit='kpc', perh=True)

        for idx, host in enumerate(self.binnedhosts):
            _write('\rGenerating halo mass profiles [%d out of %d]',
                   (idx + 1, self.headers['Number_of_hosts']))

            key = str(host['mass'])
            thehost = self.hosts[key] = {}
            thehost['dtohost'] = _sorteddtohost(host, sortedx, self.lratio)
            thehost['profile'] = _densitycalc(
                self.hosts[key]['dtohost'], self.rbins, mparticlesperh, rhocritperh)
        _write(' [done]\n')

        self.plotparams = _stackprofiles(self.hosts, self.rbins)

    def plot(self, **kwargs):
        """Callin _plot function for plotting the halo mass profile"""

        if not self.plotparams:
            print('[error] Profiles data are not available.')
            print('        Make sure to run `Rockstar.profilesgen()` first.')
            return

        k = kwargs.get
        kws = dict(kwargs)

        name = kws['name'] if 'name' in kws else self.rfname

        print('Parameters review:')
        for key, value in self.headers.iteritems():
            if key is not 'column_tags':
                print('\t{:15}\t{:15}'.format(str(key), str(value)))
        print('')

        if 'save' not in kwargs or ('save' in kwargs and k('save') != True):
            print('[note] In case you want to save your plot, make sure to set')
            print('       `save=True` when you are calling plot function.')
            print('')

        if 'xscale' not in kws: kws['xscale'] = 'log'
        if 'yscale' not in kws: kws['yscale'] = 'log'
        if 'xlabel' not in kws: kws['xlabel'] = '$\\log_{10}(r / Kpc)$'
        if 'ylabel' not in kws: kws['ylabel'] = '$\\log_{10}(\\rho / \\rho_{crit})$'

        errorbars(self.plotparams, name, **dict(kws))

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
            result['yerr'].append(rho_mean)

    result['x'] = rbins
    return result

def _write(text, args=()):
    """Printing into stdout using sys.stdout.write"""
    sys.stdout.write(text % args)
    sys.stdout.flush()
