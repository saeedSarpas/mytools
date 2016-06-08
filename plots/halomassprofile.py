"""halofassprofile.py
Plotting the halo mass profile"""

from math                             import sqrt, pi

import numpy                          as np

from ..sharedtools                    import headerextractor
from ..sharedtools.plotting           import errorbars
from ..sharedtools.physicalparameters import rhocrit

class Rockstar(object):
    """Handling necessary actions for generating halo mass profile for
    Rockstar output files"""

    def __init__(self, fname, **kwargs):
        """Initializing and extracting data for plotting halo mass profile.
        Note that the normal output of Rockstar does not contain the parent
        id of the halos, for generating outputs with parent ids, run:
        `./util/find_parent <normal rockstar output path>`
        from the root directory of the Rockstar project"""

        k = kwargs.get

        self.fname = fname
        self.profiles = {}
        self.plotparams = {'x': [], 'y': [], 'xerr': None, 'yerr': []}

        # Extracting header information
        self.headers = headerextractor.run(fname, 'rockstar')

        skiprows  = k('skiprows')  if 'skiprows'  in kwargs else 19
        id_col    = k('id_col')    if 'id_col'    in kwargs else 0
        mass_col  = k('mass_col')  if 'mass_col'  in kwargs else 3
        x_col     = k('x_col')     if 'x_col'     in kwargs else 8
        y_col     = k('y_col')     if 'y_col'     in kwargs else 9
        z_col     = k('z_col')     if 'z_col'     in kwargs else 10
        pid_col   = k('pid_col')   if 'pid_col'   in kwargs else -1

        print('Extracting data using following parameters:')
        print('\tskiprows:  \t' + str(skiprows))
        print('\tid_col:    \t' + str(id_col)   +
              '\t' + self.headers['column_tags'][id_col])
        print('\tmass_col:  \t' + str(mass_col)   +
              '\t' + self.headers['column_tags'][mass_col])
        print('\tx:         \t' + str(x_col)   +
              '\t' + self.headers['column_tags'][x_col])
        print('\ty:         \t' + str(y_col)   +
              '\t' + self.headers['column_tags'][y_col])
        print('\tz:         \t' + str(z_col)   +
              '\t' + self.headers['column_tags'][z_col])
        print('\tpid_col:   \t' + str(pid_col)   +
              '\t' + self.headers['column_tags'][pid_col])
        print('')

        datatype = [('id',   np.int32),
                    ('mass', np.float64),
                    ('x',    np.float64),
                    ('y',    np.float64),
                    ('z',    np.float64),
                    ('pid',  np.int32)]

        # Extracting data from rockstar output
        data = np.genfromtxt(
            fname,
            unpack='true',
            skiprows=skiprows,
            usecols=(id_col, mass_col, x_col, y_col, z_col, pid_col),
            dtype=datatype)

        self.halos = {}
        self.halos['hosts'] = np.array(
            [h for h in data if h['pid'] == -1], dtype=datatype)
        self.halos['subhalos'] = np.array(
            [h for h in data if h['pid'] != -1], dtype=datatype)

    def profilesgen(self, **kwargs):
        """Calculating mass profile of hosts inside a logarithmic mass bin and
        saving the result in self.profiles dictionary"""

        k = kwargs.get

        nbins = {}
        nbins['radius'] = k('nradiusbins') if 'nradiusbins' in kwargs else 20
        nbins['mass']   = k('nmassbins')   if 'nmassbins'   in kwargs else 8

        lratio = 1000 # Mpc/h to kpc/h

        self.headers['Number_of_mass_bins']   = nbins['mass']
        self.headers['Number_of_radius_bins'] = nbins['radius']

        print('Profiling subhalos using following parameters')
        print('\tnmassbins:  \t' + str(nbins['mass']))
        print('\tnradiusbins:\t' + str(nbins['radius']))
        print('\tlratio:     \t' + str(lratio) + ' # Mpc/h to kpc/h')
        print('')

        mbins = np.logspace(
            np.log10(np.min(self.halos['hosts']['mass'])),
            np.log10(np.max(self.halos['hosts']['mass'])),
            num=nbins['mass']+1,
            base=10)

        rhocritperh = rhocrit(self.headers['h'][0], perh=True,
                              lunit='kpc', munit='msun')

        for mmin, mmax in zip(mbins[:-1], mbins[1:]):
            subhalos = {}
            hosts = [
                h for h in self.halos['hosts'] if mmin < h['mass'] <= mmax]

            massbinkey = str(10**((np.log10(mmin * mmax)) / 2))
            self.profiles[massbinkey] = {}

            haloradii = []

            for host in hosts:
                satellites = [
                    h for h in self.halos['subhalos'] if h['pid'] == host['id']]

                if len(satellites) < 1: continue

                # subhalos[str(host['mass'])] = _ wl

                subhalos[str(host['mass'])] = {}
                subhalos[str(host['mass'])]['mass'] = []
                subhalos[str(host['mass'])]['dtohost'] = []

                subhalos[str(host['mass'])]['mass'].append(host['mass'])
                subhalos[str(host['mass'])]['dtohost'].append(0.0)

                for satellite in satellites:
                    subhalos[str(host['mass'])]['mass'].append(
                        satellite['mass'])
                    subhalos[str(host['mass'])]['dtohost'].append(
                        _dtohost(host, satellite, lratio=lratio))

                haloradius = np.max(subhalos[str(host['mass'])]['dtohost'])
                haloradii.append(haloradius)

            if len(haloradii) < 1: continue

            rbins = np.logspace(
                0, np.log10(np.max(haloradii)), num=nbins['radius']+1, base=10)

            for hostmasskey, halos in subhalos.iteritems():
                self.profiles[massbinkey][hostmasskey] = _profile(
                    halos,
                    rbins=rbins,
                    nrbins=nbins['radius'],
                    rhocrit=rhocritperh)

        for key, value in self.headers.iteritems():
            if key is not 'column_tags':
                print('\t {:30s}'.format(str(key)) + '\t' + str(value))

    def stack(self):
        """Stacking all mass profiles of a given set of hosts together"""

        if not self.profiles:
            print('[error] Profiles data are not available for plotting.')
            print('        Make sure to run `Rockstar.profilesgen()` first.')
            return

        key = self._getkeyfrominput()

        self.plotparams = _stack(self.profiles[key])

    def _getkeyfrominput(self):
        """Getting the self.profiles desired key from user for stacking the
        halo mass function of it"""

        keys = self.profiles.keys()

        while True:
            print('Index\tAverage host mass\t# of found hosts (with more than one subhalo)')

            valid_inputs = []
            for idx, key in enumerate(keys):
                nfoundhosts = len(self.profiles[key])
                strindex = str(idx) if nfoundhosts > 1 else ' '
                valid_inputs.append(idx)
                print(strindex + '\t' + key + '   \t' + str(nfoundhosts))

            index = int(raw_input('Choose one index to stack: '))

            if index in valid_inputs:
                nusedhosts = len(self.profiles[keys[index]])
                self.headers['Number_of_used_hosts'] = nusedhosts

                return keys[index]
            else:
                print('Your input was invalid. Please try again.')

    def plot(self, **kwargs):
        """Callin _plot function for plotting the halo mass profile"""

        kws = dict(kwargs)

        name  = kws['name']  if 'name'  in kws else self.fname
        save  = kws['save']  if 'save'  in kws else False

        if not save:
            print('[note] In case you want to save your plot, make sure to set')
            print('       `save=True` when you are calling plot function.')
            print('')

        if 'xscale' not in kws: kws['xscale'] = 'log'
        if 'yscale' not in kws: kws['yscale'] = 'log'
        if 'xlabel' not in kws: kws['xlabel'] = '$\\log_{10}(r / Kpc)$'
        if 'ylabel' not in kws: kws['ylabel'] = '$\\log_{10}(\\rho / \\rho_{crit})$'

        errorbars(self.plotparams, name, save, **dict(kws))


def _dtohost(host, subhalo, **kwargs):
    """Calculating the distance of the subhalo to its host"""

    lratio = kwargs.get('lratio') if 'lratio' in kwargs else 1

    dx2 = (subhalo['x'] - host['x'])**2
    dy2 = (subhalo['y'] - host['y'])**2
    dz2 = (subhalo['z'] - host['z'])**2

    return sqrt(dx2 + dy2+ dz2) * lratio

def _profile(halos, **kwargs):
    """Calculating halo mass profile for a given set of halos"""

    k = kwargs.get

    result = {'density': [], 'radius': []}

    if 'rbins' in kwargs and type(k('rbins')).__module__ == np.__name__:
        rbins = k('rbins')
    else:
        rbins = np.logspace(
            0, np.max(halos['dtohost']), num=k('nrbins'), base=10)

    rhocritical = k('rhocrit') if 'rhocrit' in kwargs else 1

    for r in rbins:
        total_mass = 0.0
        for mass, dtohost in zip(halos['mass'], halos['dtohost']):
            if r <= dtohost:
                total_mass += mass

        result['radius'].append(r)
        density = total_mass / (4/3 * pi * r**3)
        result['density'].append(density / rhocritical)

    return result

def _stack(halos):
    """Stacking all profiles together"""

    result = {'x': [], 'y': [], 'xerr': None, 'yerr': []}

    maxlength = 0
    for _, profile in halos.iteritems():
        if len(profile['density']) > maxlength:
            maxlength = len(profile['density'])

    nhalos = 0
    for i in range(maxlength):
        densities = []

        for hostmass, profile in halos.iteritems():
            if i < len(profile['density']):
                key = hostmass
                index = i
                densities.append(profile['density'][i])

        nhalos += len(densities)

        mean_density = sum(densities) / len(densities)

        result['x'].append(halos[key]['radius'][index])
        result['y'].append(mean_density)
        result['yerr'].append(
            sqrt(sum([(d - mean_density)**2 for d in densities])) / len(densities))

        print('Stacking and profiling ' + str(nhalos) + ' subhalos successfully')

    return result
