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
        self.datatype, self.halos, self.halossortedbyid = [], [], []
        self.binnedhalos = {}

        # Extracting header information
        self.headers = {'units': {}}

        with open(self.path) as _file:
            for i, line in enumerate(_file):
                line = line.strip('#').strip('\n')
                if i == 0: # First line: column names
                    self.headers['column_tags'] = line.split(' ')
                elif 0 < i <= 2:
                    statements = line.split(';')
                    for statement in statements:
                        _extractkeyvalue(statement, '=', self.headers)
                elif 2 < i <= 8:
                    statements = line.split(';')
                    for statement in statements:
                        _extractkeyvalue(statement, ':', self.headers)
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
                    self.headers['units'][key] = val
                elif i > 15:
                    break

        _file.close()

    def load(self, only=[], exclude=[], onlyhosts=False):
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

        with open(self.path) as _file:
            for i, line in enumerate(_file):
                if i == 20:
                    testline = line.strip('#').strip('\n').split(' ')
                elif i > 20:
                    break
        _file.close()

        if len(testline) != len(self.headers['column_tags']):
            tmp_headers = self.headers['column_tags'][:len(testline) - 1]
            tmp_headers.append('PID')
            self.headers['column_tags'] = tmp_headers

        zip_tag_elem = zip(self.headers['column_tags'], testline)

        self.datatype, usecols, headers = [], [], []
        for i, (tag, elem) in enumerate(zip_tag_elem):
            if len(only) > 0 and tag not in only:
                continue
            if tag in exclude:
                continue

            self.datatype.append((tag, np.dtype(type(_2number(elem)).__name__)))
            headers.append(tag)
            usecols.append(i)

        self.headers['included_columns'] = headers

        if len(self.datatype) < 1:
            raise LookupError('datatype array is empty')

        self.halos = np.genfromtxt(self.path,
                                   skip_header=19,
                                   usecols=usecols,
                                   dtype=self.datatype)

        if onlyhosts:
            if 'PID' not in self.headers['included_columns']:
                raise KeyError("Can't find PID")

            self.halos = np.array(
                [d for d in self.halos if d['PID'] == -1],
                dtype=self.datatype)

    def sortbyid(self):
        """Sorted halos by id, left unavailable halos zeros"""

        highestid = np.max(self.halos['id']) + 1

        self.halossortedbyid = np.zeros(highestid, dtype=self.datatype)
        self.halossortedbyid['id'] = [-1] * highestid

        for halo in self.halos:
            self.halossortedbyid[halo['id']] = halo

    def binning(self, mbins):
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
                [h for h in self.halos if minm < h['mbound_vir'] <= maxm],
                dtype=self.datatype)

    def setheader(self, key, value):
        """Add new attribute to headers

        Parameters
        ----------
        key : str
        value :
        """

        self.headers[key] = value


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
