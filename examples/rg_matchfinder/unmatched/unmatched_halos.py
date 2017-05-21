"""unmatched-halos.pu
Plotting unmatched halos
"""


import numpy as np
from mytools.visualization.myplot import MyPlot
from mytools.visualization.mycolordict import CDICT


MYPLOT = MyPlot(figsize=(15, 15))


class UnmatchedHalos(object):
    """Plotiing unmatched halos"""

    def __init__(self, headerpath):
        """Initializing UnmatchedHalos object by load the header file"""
        self.header = {}

        with open(headerpath) as _headerfile:
            for line in _headerfile:
                kv = line.split(':')
                self.header[kv[0]] = _2number(kv[1].strip('\n').strip())

        self.halo = []
        self.particles, self.halos = [], []
        self.targetIds = []

        self.pDtype = [
            ('id', np.int),
            ('x', np.float),
            ('y', np.float),
            ('z', np.float)]

        self.hDtype = [
            ('id', np.int),
            ('x', np.float),
            ('y', np.float),
            ('z', np.float),
            ('r', np.float)]


    def load(self):
        """Loading halos and particles"""

        self.halo = np.genfromtxt(
            self.header['halo_output_addr'],
            skip_header=1,
            max_rows=self.header['halo_num_p'],
            dtype=self.pDtype)

        for tag in ['x', 'y', 'z']:
            self.halo[tag] /= 1000 # kpc -> Mpc

        res = ['256', '512', '1024']
        for i in range(3):
            self.halos.append(np.genfromtxt(
                self.header['halos%s_output_addr' % res[i]],
                skip_header=1,
                max_rows=self.header['halos%s_found' % res[i]],
                dtype=self.hDtype))

            # self.halos[-1]['r'] /= 1000 # kpc -> Mpc

            self.particles.append(np.genfromtxt(
                self.header['particles%s_output_addr' % res[i]],
                skip_header=1,
                max_rows=self.header['particles%s_found' % res[i]],
                dtype=self.pDtype))

            for tag in ['x', 'y', 'z']:
                self.particles[-1][tag] /= 1000 # kpc -> Mpc

            self.halos[-1]['r'] /= 1000 # kpc -> Mpc

        # Finding target particles ids in higher resolutions
        positions = [2 * _1_to_3(hid, 256) for hid in self.halo['id']]
        self.targetIds.append([_3_to_1(p, 512) for p in positions])

        positions = [4 * _1_to_3(hid, 256) for hid in self.halo['id']]
        self.targetIds.append([_3_to_1(p, 1024) for p in positions])


    def plot(self):
        """Plotting the halo and it's environment in 3 different resolutions"""
        self._plot_column(tags=['x', 'y'], colnum=1)
        self._plot_column(tags=['x', 'z'], colnum=2)
        self._plot_column(tags=['y', 'z'], colnum=3)


    def _plot_column(self, tags=['x', 'y'], colnum=1):
        """Plotting each column of figure"""
        kws = {
            'xscale': 'linear',
            'yscale': 'linear',
            'silent': True
        }

        kws['xlabel'] = tags[0] + r" [$h^{-1}$ Mpc]"
        kws['ylabel'] = tags[1] + r" [$h^{-1}$ Mpc]"
        kws['xmin'] = self.header[tags[0] + 'min'] / 1000
        kws['xmax'] = self.header[tags[0] + 'max'] / 1000
        kws['ymin'] = self.header[tags[1] + 'min'] / 1000
        kws['ymax'] = self.header[tags[1] + 'max'] / 1000

        for i in range(3):
            pos = '33%d' % (colnum + i*3)
            ax = MYPLOT.new2daxes(pos=pos)
            dotsize = 2**(3 * (2 - i))

            # Plotting particles
            kws['color'] = "#888888"
            MYPLOT.scatter(
                self.particles[i][tags[0]],
                self.particles[i][tags[1]],
                ax=ax, s=dotsize, **dict(kws))

            if i is 0:
                # Plotting particles of the halo
                kws['color'] = "#000000"
                MYPLOT.scatter(
                    self.halo[tags[0]],
                    self.halo[tags[1]],
                    ax=ax, s=dotsize, **dict(kws))
            else:
                # Plotting similar particles
                particles = [p for p in self.particles[i] if p['id'] in self.targetIds[i-1]]
                particles = np.array(particles, dtype=self.pDtype)

                kws['color'] = CDICT['RAINBOW']['primary_colors'][0]
                MYPLOT.scatter(
                    particles[tags[0]],
                    particles[tags[1]],
                    ax=ax, s=32, **dict(kws))

            # Plotting halos
            circles = []
            for h in self.halos[i]:
                MYPLOT.plt.gca().add_artist(
                    MYPLOT.plt.Circle(
                        (h[tags[0]], h[tags[1]]), h['r'],
                        color=CDICT['RAINBOW']['primary_colors'][1],
                        fill=False))


    def save(self, path):
            """Saving the generated plot"""
            MYPLOT.plt.tight_layout()
            MYPLOT.save(path, dpi=120, transparent=False)
            MYPLOT.plt.clf()


def _1_to_3(index, res):
    """Converting a row-major 3D id to its components"""
    index %= res**3
    x = int(index / res**2)
    index -= (x * res**2)
    y = int(index / res)
    z = index - (y * res)
    return np.array([x, y, z])


def _3_to_1(pos, res):
    """Converting a 3D coordinate to a row-major id"""
    return (pos[0] % res) * res**2 + (pos[1] % res) * res + (pos[2] % res)


def _2number(string):
    """Converting a string to int or float

    Parameters
    ----------
    string : str
        A string for converting to int or float

    Returns
    -------
    int or float
    """

    try:
        return int(string)
    except ValueError:
        try:
            return float(string)
        except ValueError:
            return string
