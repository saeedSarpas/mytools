"""physicalparameters.py
Generating physical constants and parameters in desired unit"""


from math import pi


def _get(const):
    """Returning constants

    Parameters
    ----------
    const : str
        Constant name
    """

    constants = {
        'kg': 1,
        'm': 1,
        's': 1,
        'G': 6.67408e-11, # m^3 kg^-1 s^-2
        'pc': 3.086e16, # km
        'kpc': 3.086e19, # km
        'Mpc': 3.086e22, # km
        'msun': 1.989e30, # kg
        'Mpc/km': 3.086e19
    }

    if str(const) in constants:
        return constants[str(const)]
    else:
        raise LookupError(str(const) + 'does not exist in constants')


def getG(**kwargs):
    """Gravitational constant

    Parameters
    ----------
    lunit : str, optional
        Unit of length
    tunit : str, optional
        Unit of time
    munit : str, optional
        Unit of mass

    Returns
    -------
    float
        Gravitational constant in desired units
    """

    k = kwargs.get

    lunit = k('lunit') if 'lunit' in kwargs else 'm'
    tunit = k('tunit') if 'tunit' in kwargs else 's'
    munit = k('munit') if 'munit' in kwargs else 'kg'

    return _get('G') * (_get(munit)) * _get(tunit)**2 / _get(lunit)**3


def rhocrit(h_cte, **kwargs):
    """Critical density of the Universe

    Parameters
    ----------
    h_cte: float
       Unit-less Hubble constant [km / (Mpc s^2)]
    lunit : str, optional
        Unit of length
    munit : str, optional
        Unit of mass
    perh : bool, optional
        Return the comoving critical density or not

    Returns
    -------
    float
        Critical density in desired units
    """

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
