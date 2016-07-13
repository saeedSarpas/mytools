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
        self.dtype = np.dtype([('id', np.int),
                               ('x', np.float64),
                               ('y', np.float64),
                               ('z', np.float64)])

    def load(self):
        """Load gadget snapshot into a numpy array

        Returns
        -------
        numpy.ndarray

        Examples
        --------
        >>> gadget.load()
        """

        self.data = self._readdata()

        return self.data

    def index(self, col='x'):
        """Sorting the data based on a given column

        Parameters
        ----------
        x : str
            the name of the column to be sorted

        Returns
        -------
        numpy.ndarray

        Examples
        --------
        >>> gadget.index(col='z')
        """

        return np.sort(self.data, order=col)

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

        tmp = []
        for i in range(self.headers['nparticles']):
            tmp.append(
                (i, positions[i*3], positions[i*3 + 1], positions[i*3 + 2]))

        return np.array(tmp, dtype=self.dtype)
