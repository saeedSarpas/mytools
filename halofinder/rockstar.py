"""rockstar.py
The aim of this module is to load a rockstar ascii file into an rockstar
object variable.
"""

import numpy as np

class Rockstar(object):
    """Loading Rockstar ascii file

    Parameters
    ----------
    path : str
        Path to the rockstar ascii file

    Methods
    -------
    load(only=[], exclude=[])
        Loading Rockstar file to an object variable (Rockstar.data)

    Examples
    --------
    >>> from mytools.halofinders.rockstar import Rockstar
    >>> rockstar = Rockstar('/path/to/rockstar/ascii/file')
    """

    def __init__(self, path):
        """Initializing Rockstar object by extracting header information
        """

        self.path = path
        self.data = []

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

    def load(self, only=[], exclude=[]):
        """Loading Rockstar ascii file to Rockstar.data

        Parameters
        ----------
        only : list of str, optional
            A list of column tags to load
        exclude : list of str, optional
            A list of column tags to exclude them from loading

        Examples
        --------
        >>> rockstar.load(only=['id', 'PID'])
        """

        with open(self.path) as _file:
            for i, line in enumerate(_file):
                if i == 20:
                    testline = line.strip('#').strip('\n').split(' ')
                elif i > 20:
                    break
        _file.close()

        if len(testline) != len(self.headers['column_tags']):
            headers = self.headers['column_tags'][:len(testline) - 1]
            headers.append('PID')
            self.headers['column_tags'] = headers

        zip_tag_elem = zip(self.headers['column_tags'], testline)

        datatype, usecols = [], []

        for i, (tag, elem) in enumerate(zip_tag_elem):
            if len(only) > 0 and tag not in only:
                continue
            if tag in exclude:
                continue

            datatype.append((tag, np.dtype(type(_2number(elem)).__name__)))
            usecols.append(i)

        self.data = np.genfromtxt(self.path,
                                  skiprows=19,
                                  usecols=usecols,
                                  dtype=datatype)

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
