"""gadget.py
Loading Gadget snapshot"""

import struct
import numpy  as np

class Gadget(object):
    """Tool for loading and manipulating Gadget 2 snapshot.

    .. note:: This module only support N-body simulations

    Parameters
    ----------
    path : str
        Path to the gadget snapshot

    Examples
    --------
    >>> from mytools.simulation.gadget import Gadget
    >>> gadget = Gadget('/path/to/gadget/snapshot')
    """

    def __init__(self, path):
        """Initializing"""
        self._file = open(path, 'rb')
        self.headers = self._readheader()
        self.data = []
        self.dtype = np.dtype([('x', np.float64),
                               ('y', np.float64),
                               ('z', np.float64)])

    def load(self, sorting=False, col='x'):
        """Load gadget snapshot into a numpy array

        Parameters
        ----------
        lratio : float
            Length conversion
        sorting : bool, optional
            Whether sorting the data or not
        col : str, optional
            Specifying the sorting refrence

        Returns
        -------
        numpy.ndarray

        Examples
        --------
        >>> gadget.load()
        """

        data = self._readdata()

        if sorting is True:
            self.data = np.sort(data, order=col)
        else:
            self.data = data


    def _readheader(self):
        """Extracting gadget header

        Returns
        -------
        dict
        """
        headerlen = struct.unpack('i', self._file.read(4))[0] + 4
        fmtstring = "6i8d9i{0:d}x".format(headerlen - 124)
        headers = struct.unpack(fmtstring, self._file.read(headerlen))
        return {
            'particles': {
                'gas': (headers[0], headers[6]),
                'halo': (headers[1], headers[7]),
                'disk': (headers[2], headers[8]),
                'bulge': (headers[3], headers[9]),
                'stars': (headers[4], headers[10]),
                'bndry': (headers[5], headers[11])
            },
            'time': headers[12],
            'nparticles': sum(headers[:6])
        }

    def _readdata(self):
        """Extracting position data

        Returns
        -------
        numpy.ndaaray
        """

        positionslen = struct.unpack('i', self._file.read(4))[0] + 4
        fmtstring = "{0:d}f4x".format(self.headers['nparticles']*3)
        positions = struct.unpack(fmtstring, self._file.read(positionslen))

        tmp = [(0.0, 0.0, 0.0)] * self.headers['nparticles']
        for i in range(self.headers['nparticles']):
            tmp[i] = (positions[i*3], positions[i*3 + 1], positions[i*3 + 2])

        return np.array(tmp, dtype=self.dtype)
