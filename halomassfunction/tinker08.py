"""tinker08.py
Tinker halo mass function, Tinker et al., 2008"""

from astropy.cosmology import Planck15
from hmf import MassFunction
from hmf.filters import TopHat
import hmf.transfer_models as tm
import hmf.fitting_functions as ff

def Tinker08():
    """Tinker halo mass function generator

    Returns
    ----------
    dict
        A dictionary contains of 'x' and 'y' keys, suitable to use with
        ..visualization.plot module
    """

    hmf = MassFunction(
        hmf_model=ff.Tinker08,
        delta_h=200.0,
        filter_model=TopHat,
        transfer_model=tm.EH,
        cosmo_model=Planck15)

    return {'x': hmf.m, 'y': hmf.dndlnm}
