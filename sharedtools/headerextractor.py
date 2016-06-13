"""headerextractor.py
Extract headers informations of different output files"""

def run(fname, ftype):
    """Main function for extracting header info"""

    headers, tmp = {}, {}

    inputs = {
        'rockstar': {
            'headers': {
                'delimiters': ['=', ':'],
                'inline_delimiter': ';',
                'rows': 19,
                'keys': [
                    'Box_size',
                    'Total_particles_processed',
                    'Particle_mass',
                    'FOF_linking_length',
                    'Unbound_Threshold',
                    'FOF_Refinement_Threshold',
                    'Force_resolution_assumed',
                    'Rockstar_Version',
                    'Units',
                    'Om',
                    'Ol',
                    'h'
                ]
            },
            'column_tags': {
                'delimiter': ' ',
                'line': 1
            }
        }
    }

    with open(fname) as _file:
        _input = inputs[ftype]
        for delimiter in _input['headers']['delimiters']:
            _file.seek(0)
            lines = [next(_file) for _ in range(_input['headers']['rows'])]
            for line in lines:
                statements = line.split(_input['headers']['inline_delimiter'])
                for statement in statements:
                    if delimiter in statement:
                        key, val = statement.strip('#').strip().partition(delimiter)[::2]
                        key = key.strip().replace(' ', '_')
                        val = val.strip().strip('\n').split()
                        tmp[key] = val

        if 'column_tags' in _input:
            _file.seek(_input['column_tags']['line'], 0)
            line = _file.readline().strip('\n')
            headers['column_tags'] = line.split(_input['column_tags']['delimiter'])

    headers['Filename'] = fname

    for key in _input['headers']['keys']:
        headers[key] = tmp[key] if key in tmp else "NOT AVAILABLE"

    return headers
