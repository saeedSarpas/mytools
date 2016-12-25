"""rockstar.py
The aim of this module is to load a rockstar ascii file into an rockstar
object variable.
"""

from __future__ import print_function

import numpy as np


NOTAVLBL = -1


class Rockstar(object):
    """Loading Rockstar ascii file

    Parameters
    ----------
    path : str
        Path to the rockstar ascii file

    Methods
    -------
    load()
        Loading Rockstar file to an object variable (Rockstar.data)
    binning()
        Binning halos based on their masses
    setheader()
        Add new attribute to headers

    Examples
    --------
    >>> from mytools.halofinders.rockstar import Rockstar
    >>> rockstar = Rockstar('/path/to/rockstar/ascii/file')
    """

    def __init__(self, path):
        """Constructor for Rockstar class"""

        self.path = path
        self.dtype, self.halos, self.halossortedbyid = [], [], []
        self.binnedhalos = {}
        self.usecols = []

        self.header = self._loadheader()

    def load(self, only=None, exclude=None, onlyhosts=False):
        """Loading Rockstar ascii file to Rockstar.data

        Parameters
        ----------
        only : list of str, optional
            A list of column tags to load
        exclude : list of str, optional
            A list of column tags to exclude them from loading
        onlyhosts : bool, optional
            Keep only host halos

        Examples
        --------
        >>> rockstar.load(only=['id', 'PID'], onlyhosts=True)
        """

        self._loaddtype(only, exclude)

        self.halos = np.genfromtxt(self.path,
                                   skip_header=19,
                                   usecols=self.usecols,
                                   dtype=self.dtype)

        if onlyhosts:
            if 'PID' not in self.header['included_columns']:
                raise KeyError("Can't find PID")

            self.halos = np.array(
                [d for d in self.halos if d['PID'] == -1],
                dtype=self.dtype)


    def sortbyid(self):
        """Sorted halos by id, left unavailable halos zeros"""

        highestid = np.max(self.halos['id']) + 1

        self.halossortedbyid = np.zeros(highestid, dtype=self.dtype)
        self.halossortedbyid['id'] = [NOTAVLBL] * highestid

        for halo in self.halos:
            self.halossortedbyid[halo['id']] = halo

    def binning(self, mbins, prop='mbound_vir'):
        """Binning halos based on a given mass bins

        Parameters
        ----------
        mbins : array of float
            Mass bins edges

        Examples
        --------
        >>> import numpy as np
        >>> mbins = np.logspace(10, 15, num=21, base=10)
        >>> rockstar.binning(mbins)
        """

        for idx, (minm, maxm) in enumerate(zip(mbins[:-1], mbins[1:])):
            self.binnedhalos[idx] = np.array(
                [h for h in self.halos if minm < h[prop] <= maxm],
                dtype=self.dtype)

    def vs(self, prop1, prop2, nbins=21, **kwargs):
        """Plotting the relations between Rockstar output properties

        Parameters
        ----------
        prop1, prop2 : string
            Halos properties to plot
        nbins : integer
            Number of bins
        xscale, yscale : string, optional
        xmin, xmax, ymin, ymax : numbers, optional
        """

        sortedhalos = np.sort(self.halos, order=prop1)

        if 'xscale' in kwargs:
            xscale = kwargs.get('xscale')
        else:
            x99 = np.percentile(sortedhalos[prop1], 99)
            x50 = np.percentile(sortedhalos[prop1], 50)
            xscale = 'log' if x50 == 0 or x99/x50 >= 10 else 'linear'

        if xscale == 'log':
            bins = np.logspace(np.log10(sortedhalos[prop1][0]),
                               np.log10(sortedhalos[prop1][-1]),
                               num=nbins, base=10)
        else:
            bins = np.linspace(sortedhalos[prop1][0], sortedhalos[prop1][-1],
                               num=nbins)

        xs, ys = [], []
        for bmin, bmax in zip(bins[:-1], bins[1:]):
            values = [sortedhalos[prop2][i]
                     for i in range(len(sortedhalos[prop1]))
                     if bmin < sortedhalos[prop1][i] <= bmax]
            if len(values) == 0:
                continue
            else:
                ys.append(sum(values) / len(values))
                if xscale == 'log':
                    xs.append(10**(np.log10(bmin * bmax) / 2))
                else:
                    xs.append((bmin + bmax) / 2)

        return xs, ys


    def setheader(self, key, value):
        """Add new attribute to headers

        Parameters
        ----------
        key : str
        value :
        """

        self.header[key] = value

    def _loadheader(self):
        """Loading the header of the rockstar ascii halo file"""

        header = {'units': {}}

        with open(self.path) as _file:
            for i, line in enumerate(_file):
                line = line.strip('#').strip('\n')
                if i == 0: # First line: column names
                    header['column_tags'] = line.split(' ')
                elif 0 < i <= 2:
                    statements = line.split(';')
                    for statement in statements:
                        _extractkeyvalue(statement, '=', header)
                elif 2 < i <= 8:
                    statements = line.split(';')
                    for statement in statements:
                        _extractkeyvalue(statement, ':', header)
                elif 8 < i <= 15: # Units
                    if ' in ' in line:
                        delim = ' in '
                    elif ' is ' in line:
                        delim = ' is '
                    elif ' are ' in line:
                        delim = ' are '
                    else:
                        continue
                    parts = line.partition(':')[2].strip().partition(delim)
                    key = parts[0].strip().replace(' ', '_')
                    val = parts[2].strip()
                    header['units'][key] = val
                elif i > 15:
                    break

        return header

    def _loaddtype(self, only=None, exclude=None):
        """Loading the datatypes of the rockstar ascii halo file"""

        with open(self.path) as _file:
            for i, line in enumerate(_file):
                if i == 20:
                    inputs = line.strip('#').strip('\n').split(' ')
                elif i > 20:
                    break

        # Due to a reported bug in Rockstar, after generating parents of halos,
        # the header won't be updated properly. Following we try to solve this
        # problem.
        # NOTE: it's not the best way to handle this bug!
        if len(inputs) != len(self.header['column_tags']):
            tmp_headers = self.header['column_tags'][:len(inputs) - 1]
            tmp_headers.append('PID')
            self.header['column_tags'] = tmp_headers

        self.dtype, self.usecols, tags = [], [], []
        zip_tag_elem = zip(self.header['column_tags'], inputs)

        for i, (tag, elem) in enumerate(zip_tag_elem):
            if only is not None and len(only) > 0 and tag not in only:
                continue
            if exclude is not None and len(exclude) > 0 and tag in exclude:
                continue

            self.dtype.append((tag, np.dtype(type(_2number(elem)).__name__)))

            tags.append(tag)
            self.usecols.append(i)

        self.header['included_columns'] = tags

        if len(self.dtype) < 1:
            raise LookupError('datatype array is empty')

def _extractkeyvalue(statement, delimiter, dictionary):
    """Extracting data from a statement using a delimiter and inserting them
    into a given dictionary (headers)

    Parameters
    ----------
    statement : str
    delimiter : str
        A string for using as a delimiter for paritioning the statement
    dictionary : dict
        A dictionary for adding keys, values into it
    """
    key, val = statement.strip().partition(delimiter)[::2]

    key = key.strip().replace(' ', '_')
    val = val.strip().strip('\n').split()

    dictionary[key] = val


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
        return float(string)
