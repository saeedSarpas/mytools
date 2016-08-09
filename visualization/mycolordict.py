"""This module contains different color dictionaries for plotting"""

def get(colorscheme):
    """Return colorscheme

    Parameter
    ---------
    colorscheme : str
        Color scheme name

    Returns
    -------
    dict
        A color scheme dictionary
    """

    if colorscheme in CDICT.keys():
        return CDICT[str(colorscheme)]
    else:
        return CDICT[CDICT.keys()[0]]


def helpercolors(colorscheme):
    """Helper colors generator

    Getting helper colors one by one

    Parameters
    ----------
    colorscheme : str
    """

    cdict = get(colorscheme)
    for color in cdict['helper_colors']:
        yield color

def primarycolors(colorscheme):
    """Primary colors generator

    Returning primary colors one by one

    Parameters
    ----------
    colorscheme : str
    """

    cdict = get(colorscheme)
    for color in cdict['primary_colors']:
        yield color


CDICT = {
    'AUTUMN_COLORSCHEME': {
        'dict': {
            'red': [
                (0.0, 0.0, 0.3568627450980392),
                (0.25, 0.4470588235294118, 0.4470588235294118),
                (0.50, 0.996078431372549, 0.996078431372549),
                (0.75, 0.996078431372549, 0.996078431372549),
                (1, 0.996078431372549, 0.996078431372549)
            ],
            'green': [
                (0.0, 0.0, 0.48627450980392156),
                (0.25, 0.7215686274509804, 0.7215686274509804),
                (0.50, 0.8274509803921568, 0.8274509803921568),
                (0.75, 0.5647058823529412, 0.5647058823529412),
                (1, 0.3686274509803922, 0.3686274509803922)
            ],
            'blue': [
                (0.0, 0.0, 0.4901960784313725),
                (0.25, 0.788235294117647, 0.788235294117647),
                (0.50, 0.6470588235294118, 0.6470588235294118),
                (0.75, 0.0039215686274510, 0.0039215686274510),
                (1, 0.1921568627450980, 0.1921568627450980)
            ]
        },
        'helper_colors': [
            '#2e3740', '#798c8a', '#4c5359', '#1d2025', '#d0d9d8'],
        'primary_colors': [
            '#581845', '#900C3F', '#C70039', '#FF5733', '#FFC30F'],
        'ecolor': '#febb00',
        'linecolor': '#000000',
        'facecolor': '#fef4ea',
        'background': '#f2f2f2',
        'gridcolor': '#ffffff',
        'axiscolor': '#444444'
    },
    'SANDSTONE': {
        'helper_colors': [
            '#7584AD', '#314570', '#AED1D6', '#E09873', '#A37F6F'],
        'primary_colors': [
            '#1e0a0b', '#4a2245', '#8a2245', '#cb3c3e', '#f9703c', '#ff861d']
    }
}
