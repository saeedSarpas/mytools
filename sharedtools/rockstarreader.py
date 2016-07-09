"""rockstarreader.py
Loading Rockstar output"""

import headerextractor
import numpy           as np

class Rockstar(object):
    """Tools for loading and manipulating Rockstar output files"""

    def __init__(self, path):
        """Loading a Rockstar output file"""

        # Extracting header information
        self.headers = headerextractor.run(path, 'rockstar')

        with open(path) as _file:
            for i, line in enumerate(_file):
                if i == 20:
                    testline = line
                if i > 20:
                    break

        _file.close();

        datatype = []

        for tag, elem in zip(self.headers['column_tags'], testline.split(' ')):
            datatype.append((tag, np.dtype(type(eval(elem)).__name__)))

        self.data = np.genfromtxt(path, skiprows=19, dtype=datatype)
