"""physicalparameters.py
Generating physical constants in a desired unit"""

from math import pi

def _get(const):
    """Returning constants"""

    constants = {
        'kg': 1,
        'm': 1,
        's': 1,
        'G': 6.67408e-11,
        'pc': 3.086e16,
        'kpc': 3.086e19,
        'Mpc': 3.086e22,
        'Mpc/km': 3.086e19,
        'msun': 1.989e30
    }

    if str(const) in constants:
        return constants[str(const)]
    else:
        raise LookupError(str(const) + 'does not exist in constants')

def getG(**kwargs):
    """Gravitational constant"""

    k = kwargs.get

    lunit = k('lunit') if 'lunit' in kwargs else 'm'
    tunit = k('tunit') if 'tunit' in kwargs else 's'
    munit = k('munit') if 'munit' in kwargs else 'kg'

    return _get('G') * (_get(munit)) * _get(tunit)**2 / _get(lunit)**3

def rhocrit(h_cte, **kwargs):
    """Critical density of the Universe
    Parameters:
    h: Number
       Unit-less Hubble constant [km / (Mpc s^2)]"""

    k = kwargs.get

    lunit = k('lunit') if 'lunit' in kwargs else 'm'
    munit = k('munit') if 'munit' in kwargs else 'kg'


    H_si = h_cte * 100 / _get('Mpc/km')
    G_si = getG()

    rho = (3 * H_si**2 / (8 * pi * G_si)) * _get(lunit)**3 / _get(munit)

    if 'perh' in kwargs and k('perh') == True:
        return rho / h_cte
    else:
        return rho
