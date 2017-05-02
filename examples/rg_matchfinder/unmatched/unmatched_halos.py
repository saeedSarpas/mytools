"""unmatched-halos.pu
Plotting unmatched halos
"""


import numpy as np
from mytools.visualization.myplot import MyPlot


MYPLOT = MyPlot(aspect=0.5)


def load_header(path):
    """Loading the header of an unmatched output file"""
    header = {}
    with open(path) as _file:
        for i, line in enumerate(_file):
            if i > 18: break
            keyvalue = line.split(':')
            header[keyvalue[0]] = _2number(keyvalue[1].strip('\n'))

    # Mpc2kpc
    header['xmin'] /= 1000
    header['xmax'] /= 1000
    header['ymin'] /= 1000
    header['ymax'] /= 1000
    header['zmin'] /= 1000
    header['zmax'] /= 1000

    return header


def load_particles(path, header):
    """Loading particles from an unmatched output file

    Parameters
    ----------
    path : string
        path to an unmatched output file
    header : dict
        loaded header dictionary
    """
    dtype = [
        ('x', np.float),
        ('y', np.float),
        ('z', np.float),
    ]

    halo, pri_box, sec_box = [], [], []

    halo = np.genfromtxt(path, skip_header=19,
                         max_rows=header['halo_n_p'], dtype=dtype)

    pri_box = np.genfromtxt(path, skip_header=19+header['halo_n_p'],
                            max_rows=header['pri_region_n_p'], dtype=dtype)

    sec_box = np.genfromtxt(path, skip_header=19+header['halo_n_p']+header['pri_region_n_p'],
                            max_rows=header['sec_region_n_p'], dtype=dtype)

    for axis in ['x', 'y', 'z']:
        halo[axis] /= 1000
        pri_box[axis] /= 1000
        sec_box[axis] /= 1000

    return halo, pri_box, sec_box

def plot_unmatched(halo, pri_box, sec_box, header):
    """Plotting an unmatched halo"""
    kws = {
        'xscale': 'linear',
        'yscale': 'linear',
        'silent': True
    }

    kws['xlabel'] = 'x'
    kws['ylabel'] = 'y'
    kws['xmin'] = header['xmin']
    kws['xmax'] = header['xmax']
    kws['ymin'] = header['ymin']
    kws['ymax'] = header['ymax']

    kws['color'] = "#aaaaaa"
    MYPLOT.scatter(pri_box['x'], pri_box['y'], pos='231', s=1, **dict(kws))
    MYPLOT.scatter(sec_box['x'], sec_box['y'], pos='234', s=1, **dict(kws))
    kws['color'] = "#000000"
    MYPLOT.scatter(halo['x'], halo['y'], pos='231', s=4, **dict(kws))

    kws['xlabel'] = 'x'
    kws['ylabel'] = 'z'
    kws['xmin'] = header['xmin']
    kws['xmax'] = header['xmax']
    kws['ymin'] = header['zmin']
    kws['ymax'] = header['zmax']

    kws['color'] = "#aaaaaa"
    MYPLOT.scatter(pri_box['x'], pri_box['z'], pos='232', s=1, **dict(kws))
    MYPLOT.scatter(sec_box['x'], sec_box['z'], pos='235', s=1, **dict(kws))
    kws['color'] = "#000000"
    MYPLOT.scatter(halo['x'], halo['z'], pos='232', s=4, **dict(kws))

    kws['xlabel'] = 'y'
    kws['ylabel'] = 'z'
    kws['xmin'] = header['ymin']
    kws['xmax'] = header['ymax']
    kws['ymin'] = header['zmin']
    kws['ymax'] = header['zmax']

    kws['color'] = "#aaaaaa"
    MYPLOT.scatter(pri_box['y'], pri_box['z'], pos='233', s=1, **dict(kws))
    MYPLOT.scatter(sec_box['y'], sec_box['z'], pos='236', s=1, **dict(kws))
    kws['color'] = "#000000"
    MYPLOT.scatter(halo['y'], halo['z'], pos='233', s=4, **dict(kws))

def save(path):
    """Saving the generated plot"""
    MYPLOT.plt.tight_layout()
    MYPLOT.save(path, dpi=120, transparent=False)
    MYPLOT.plt.clf()


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
