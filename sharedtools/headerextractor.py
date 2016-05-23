"""headerextractor.py
Extract headers informations of different output files"""

def run(fname, ftype):
    """Main function for extracting header info"""

    headers, tmp = {}, {}

    inputs = {
        'rockstar': {
            'sep': ':',
            'rows': 19,
            'header_keys': [
                'Box_size',
                'Total_particles_processed',
                'Particle_mass'
            ]

        }
    }

    with open(fname) as rockstar_file:
        lines = [next(rockstar_file) for _ in range(inputs[ftype]['rows'])]
        for line in lines:
            key, val = line.partition(inputs[ftype]['sep'])[::2]
            key = key.strip().strip('#').replace(' ', '_')
            val = val.strip().strip('\n').split()
            tmp[key] = val

    headers['Filename'] = fname

    for tag in inputs[ftype]['header_keys']:
        headers[tag] = tmp[tag] if tag in tmp else "NOT AVAILABLE"

    return headers
