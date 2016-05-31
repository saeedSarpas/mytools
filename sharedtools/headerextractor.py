"""headerextractor.py
Extract headers informations of different output files"""

def run(fname, ftype):
    """Main function for extracting header info"""

    headers, tmp = {}, {}

    inputs = {
        'rockstar': {
            'headers': {
                'sep': ':',
                'rows': 19,
                'keys': [
                    'Box_size',
                    'Total_particles_processed',
                    'Particle_mass'
                ]
            },
            'column_tags': {
                'sep': ' ',
                'line': 1
            }
        }
    }

    with open(fname) as _file:
        _input = inputs[ftype]
        lines = [next(_file) for _ in range(_input['headers']['rows'])]
        for line in lines:
            key, val = line.partition(_input['headers']['sep'])[::2]
            key = key.strip().strip('#').replace(' ', '_')
            val = val.strip().strip('\n').split()
            tmp[key] = val

        if 'column_tags' in _input:
            _file.seek(_input['column_tags']['line'], 0)
            line = _file.readline().strip('\n')
            headers['column_tags'] = line.split(_input['column_tags']['sep'])


    headers['Filename'] = fname

    for key in _input['headers']['keys']:
        headers[key] = tmp[key] if key in tmp else "NOT AVAILABLE"

    return headers
