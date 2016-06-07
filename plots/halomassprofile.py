"""halofassprofile.py
Plotting the halo mass profile"""

from math                             import sqrt

import numpy                          as np

from ..sharedtools                    import headerextractor
from ..sharedtools.plotting           import errorbars
from ..sharedtools.physicalparameters import rhocrit

class Rockstar(object):
    """Handling necessary actions for generating halo mass profile for
    rockstar output files"""

    def __init__(self, fname, **kwargs):
        """Initializing and extracting data for plotting halo mass profile.
        Note that the normal output of Rockstar does not contain the parent
        id of the halos, for generating outputs with parent ids, run:
        `./util/find_parent <normal rockstar output path>`
        from the root directory of the Rockstar project"""

        self.fname = fname
        self.profile = {}
        self.plotparams = {'x': [], 'y': [], 'xerr': None, 'yerr': None}
        self.datatype = np.dtype([
            ('id',   np.int32),
            ('mass', np.float64),
            ('x',    np.float64),
            ('y',    np.float64),
            ('z',    np.float64),
            ('pid',  np.int32)])

        k = kwargs.get

        # Extracting header information
        self.headers = headerextractor.run(fname, 'rockstar')

        skiprows  = k('skiprows')  if 'skiprows'  in kwargs else 19
        id_col    = k('id_col')    if 'id_col'    in kwargs else 0
        mass_col  = k('mass_col')  if 'mass_col'  in kwargs else 3
        x_col     = k('x_col')     if 'x_col'     in kwargs else 8
        y_col     = k('y_col')     if 'y_col'     in kwargs else 9
        z_col     = k('z_col')     if 'z_col'     in kwargs else 10
        pos_uncer = k('pos_uncer') if 'pos_uncer' in kwargs else 19
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
        print('\tpos_uncer: \t' + str(pos_uncer)   +
              '\t' + self.headers['column_tags'][pos_uncer])
        print('\tpid_col:   \t' + str(pid_col)   +
              '\t' + self.headers['column_tags'][pid_col])
        print('')

        # Extracting data from rockstar output
        self.halos = np.genfromtxt(
            fname,
            unpack='true',
            skiprows=skiprows,
            usecols=(id_col, mass_col, x_col, y_col, z_col, pid_col),
            dtype=self.datatype)

    def stackhalos(self, **kwargs):
        """Calling _profile function for generating mass halo profile"""

        k = kwargs.get

        nmassbins = k('nmassbins') if 'nmassbins' in kwargs else 8
        dratio = 1000 # Mpc/h to kpc/h

        self.headers['Number_of_mass_bins'] = nmassbins

        print('Stacking subhalos using following parameters')
        print('\tnmassbins:\t' + str(nmassbins))
        print('\tdratio:   \t' + str(dratio) + ' (Mpc/h to kpc/h)')
        print('')

        massbins = np.logspace(
            np.log10(np.min(self.halos['mass'])),
            np.log10(np.max(self.halos['mass'])),
            num=nmassbins+1,
            base=10)

        subhalos = [h for h in self.halos if h['pid'] != -1]
        binnedhosts = {}

        for mmin, mmax in zip(massbins[:-1], massbins[1:]):
            meanmass = 10**((np.log10(mmin * mmax)) / 2)
            key = str(meanmass)

            binnedhosts[key] = [
                h for h in self.halos
                if mmin < h['mass'] <= mmax and h['pid'] == -1]

            self.profile[key] = {}
            self.profile[key]['massrange'] = (mmin, mmax)

            self.profile[key]['stackedhalos'] = _stack(
                binnedhosts[key], subhalos, dratio=dratio)

            self.profile[key]['stackedhalos']['mass'].append(float(key))
            self.profile[key]['stackedhalos']['dtohost'].append(0)

    def profilecalc(self, **kwargs):
        """Caclculating the halo mass profile"""

        if not self.profile:
            print('[error] Profile data is not available for plotting.')
            print('        Make sure to run `Rockstar.stackhalos()` first.')
            return

        k = kwargs.get

        nbins = k('nbins') if 'nbins' in kwargs else 20

        print('Generating halo mass profile using following parameters.')
        print('\tnbins:\t' + str(nbins))
        print('')

        key = self._getkeyfrominput()

        rbins =  np.logspace(
            0,
            np.log10(np.max(self.profile[key]['stackedhalos']['dtohost'])),
            num=nbins+1,
            base=10)

        stackedhalos = zip(
            self.profile[key]['stackedhalos']['mass'],
            self.profile[key]['stackedhalos']['dtohost'])

        crit_density = rhocrit(self.headers['h'][0], perh=True,
                               lunit='kpc', munit='msun')

        for r in rbins:
            density = sum([m for m, d in stackedhalos if d <= r]) / r**3
            self.plotparams['x'].append(r)
            self.plotparams['y'].append(density / crit_density)

        print('Stacked ' +
              str(self.headers['Number_of_stacked_subhalos']) +
              ' subhalos successfully using following parameters.')

        for key, value in self.headers.iteritems():
            if key is not 'column_tags':
                print('\t {:30s}'.format(str(key)) + '\t' + str(value))

    def _getkeyfrominput(self):
        """Getting the self.profile desired key (mass) from user for
        calculating the halo mass function for that"""
        keys = self.profile.keys()

        while True:
            print('Index\tKey\t\t\t(Min mass, Max mass)\t\t\t\t# of subhalos')
            for idx, key in enumerate(keys):
                nsubhalos = len(self.profile[key]['stackedhalos']['mass'])
                strindex = str(idx) if nsubhalos > 1 else ' '
                print(strindex + '\t' + key + '   \t' +
                      str(self.profile[key]['massrange']) + '   \t' +
                      str(nsubhalos))

            index = int(raw_input('Choose one index to plot: '))
            if index < len(keys):
                nhalos = len(self.profile[keys[index]]['stackedhalos']['mass'])
                self.headers['Number_of_stacked_subhalos'] = nhalos - 1
                self.headers['Hosthalo_mass'] = [keys[index]]
                mrange = self.profile[keys[index]]['massrange']
                self.headers['Hosthalo_mass_uncertainty'] = ['%e' % mrange[0], '%e' % mrange[1]]
                return keys[index]

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


def _stack(hosts, subhalos, **kwargs):
    """Generating logarithmic binned, mass halo profile"""

    result = {'mass': [], 'dtohost': []}

    k = kwargs.get

    dratio = k('dratio') if 'dratio' in kwargs else 1

    for host in hosts:
        satellites = [h for h in subhalos if h['pid'] == host['id']]

        if len(satellites) < 1: break

        for g in satellites:
            dx2 = (g['x'] - host['x'])**2
            dy2 = (g['y'] - host['y'])**2
            dz2 = (g['z'] - host['z'])**2
            result['mass'].append(g['mass'])
            result['dtohost'].append(sqrt(dx2 + dy2+ dz2) * dratio)

    return result
