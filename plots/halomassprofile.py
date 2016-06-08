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
        self.binnedhalos = {}
        self.plotparams = {} # 'x': [], 'y': [], 'xerr': None, 'yerr': []

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

        self.hosts = np.array(
            [h for h in data if h['pid'] == -1], dtype=datatype)
        self.subhalos = np.array(
            [h for h in data if h['pid'] != -1], dtype=datatype)

    def findsubhalos(self, **kwargs):
        """Calculating mass profile of hosts inside a logarithmic mass bin and
        saving the result in self.profiles dictionary"""

        k = kwargs.get

        nmassbins = k('nmassbins') if 'nmassbins' in kwargs else 8
        nsubhalos = k('nsubhalos') if 'nsubhalos' in kwargs else 1

        self.headers['Min_num_of_subhalos'] = nsubhalos

        lratio = 1000 # Mpc/h to kpc/h

        print('Binning host masses using following parameter(s)')
        print('\tNumber_of_mass_bins:\t' + str(nmassbins))
        print('\tnsubhalos:          \t' + str(nsubhalos))
        print('\tlratio:             \t' + str(lratio) + '  # Mpc/h to kpc/h')
        print('')

        binnedhosts = _binninghosts(self.hosts, nmassbins)
        bininfo     = _choosedesiredbin(binnedhosts)

        self.headers['Number_of_used_hosts'] = len(binnedhosts[bininfo['key']])

        for host in binnedhosts[bininfo['key']]['hosts']:
            foundsubhalos = _findsubhalos(host, self.subhalos, lratio=lratio)

            if len(foundsubhalos) > nsubhalos:
                self.binnedhalos[str(host['mass'])] = foundsubhalos
            else:
                continue

    def profilesgen(self, **kwargs):
        """Calculating mass profile of the selected bin"""

        if not self.binnedhalos:
            print('[error] Subhalos are not collected yet.')
            print('        Make sure to run `Rockstar.findsubhalos()` first.')
            return

        k = kwargs.get

        nradiusbins = k('nradiusbins') if 'nradiusbins' in kwargs else 20
        self.headers['Number_of_radius_bins'] = nradiusbins

        print('Profiling host densities using following parameter(s)')
        print('\tNumber_of_radius_bins:\t' + str(nradiusbins))
        print('')

        rmax = 0
        for _, subhalos in self.binnedhalos.iteritems():
            rhalo = np.max(subhalos['dtohost'])
            if rhalo > rmax:
                rmax = rhalo

        rbins = np.logspace(0, np.log10(rmax), num=nradiusbins+1, base=10)

        rhocritperh = rhocrit(
            float(self.headers['h'][0]), perh=True, lunit='kpc', munit='msun')

        profiles = {}
        for hostmass, subhalos in self.binnedhalos.iteritems():
            profiles[hostmass] = _profile(
                subhalos,
                rbins=rbins,
                nrbins=nradiusbins,
                rhocrit=rhocritperh)

        self.plotparams = _stack(profiles)

    def plot(self, **kwargs):
        """Callin _plot function for plotting the halo mass profile"""

        if not self.plotparams:
            print('[error] Profiles data are not available.')
            print('        Make sure to run `Rockstar.profilesgen()` first.')
            return

        k = kwargs.get
        kws = dict(kwargs)

        name = kws['name'] if 'name' in kws else self.fname

        print('Parameters review:')
        for key, value in self.headers.iteritems():
            if key is not 'column_tags':
                print('\t {:30s}'.format(str(key)) + '\t' + str(value))
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
        key = str(10**((np.log10(mmin * mmax)) / 2))
        result[key] = {}
        result[key]['massrange'] = (mmin, mmax)
        result[key]['hosts'] = [h for h in hosts if mmin < h['mass'] <= mmax]

    return result

def _choosedesiredbin(binnedhosts):
    """Getting desired bin to profile from user"""
    keys = binnedhosts.keys()

    while True:
        print('Index\tAverage host mass\tMass range\t\t\t\t# of found hosts')

        validinputs = []
        for idx, key in enumerate(keys):
            nfoundhosts = len(binnedhosts[key]['hosts'])
            strindex = str(idx) if nfoundhosts > 1 else ' '

            validinputs.append(idx)

            print(strindex + '\t' +
                  key + '   \t' +
                  str(binnedhosts[key]['massrange']) +'  \t' +
                  str(nfoundhosts))

        index = int(raw_input('Choose one index to stack: '))

        if index in validinputs:
            return {'min': binnedhosts[keys[index]]['massrange'][0],
                    'max': binnedhosts[keys[index]]['massrange'][1],
                    'key': keys[index]}
        else:
            print('Your input was invalid. Please try again.')

def _findsubhalos(host, subhalos, **kwargs):
    """Find subhalos for a giving host"""

    result = {'mass': [], 'dtohost': []}

    lratio = kwargs.get('lratio') if 'lratio' in kwargs else 1

    satellites = [h for h in subhalos if h['pid'] == host['id']]

    result['mass'].append(host['mass'])
    result['dtohost'].append(0.0)

    for satellite in satellites:
        result['mass'].append(
            satellite['mass'])
        result['dtohost'].append(
            _dtohost(host, satellite, lratio=lratio))

    return result


def _dtohost(host, subhalo, **kwargs):
    """Calculating the distance of the subhalo to its host"""

    lratio = kwargs.get('lratio') if 'lratio' in kwargs else 1

    dx2 = (subhalo['x'] - host['x'])**2
    dy2 = (subhalo['y'] - host['y'])**2
    dz2 = (subhalo['z'] - host['z'])**2

    return sqrt(dx2 + dy2+ dz2) * lratio

def _profile(subhalos, **kwargs):
    """Calculating halo mass profile for a given set of halos"""

    k = kwargs.get

    result = {'density': [], 'radius': []}

    if 'rbins' in kwargs and type(k('rbins')).__module__ == np.__name__:
        rbins = k('rbins')
    else:
        rbins = np.logspace(
            0, np.max(subhalos['dtohost']), num=k('nrbins'), base=10)

    rhocritical = k('rhocrit') if 'rhocrit' in kwargs else 1

    for r in rbins:
        total_mass = 0.0
        for mass, dtohost in zip(subhalos['mass'], subhalos['dtohost']):
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

    print('Stacked ' + str(nhalos) + ' subhalos successfully')

    return result
