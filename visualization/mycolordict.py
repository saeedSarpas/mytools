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


def primaryshadows(colorscheme):
    """Shadow generator

    Returning shadows one by one

    Parameters
    ----------
    colorscheme : str
    """

    cdict = get(colorscheme)
    for color in cdict['primary_shadows']:
        yield color


CDICT = {
    'UNTITLED': {
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
    },
    'ICE': {
        'dict': {
            'red': [
                (0.00, 0.00000, 0.94531),
                (0.05, 0.73047, 0.73047),
                (0.21, 0.62890, 0.62890),
                (1.00, 0.09765, 0.09765)
            ],
            'green': [
                (0.00, 0.00000, 0.94531),
                (0.05, 0.72656, 0.72656),
                (0.21, 0.83203, 0.83203),
                (1.00, 0.58203, 0.58203)
            ],
            'blue': [
                (0.00, 0.00000, 0.94531),
                (0.05, 0.74219, 0.74219),
                (0.21, 0.88281, 0.88281),
                (1.00, 0.67578, 0.67578)
            ]
        },
        'primary_colors': ['#ff4843', '#328bdc', '#fbac47', '#5fae5b'],
        'primary_shadows': ['#d97b78', '#84a8ca', '#d9b876', '#88af85'],
        'helper_colors': ['#e68570', '#688eb5', '#e1ca61', '#91ab59'],
        'background': '#f0f0f0',
        'gridcolor': '#ffffff',
        'axiscolor': '#666666'
    },
    'RAINBOW': {
        'dict': {
            'red': [
                (0.00, 0.00000, 0.28125),
                (0.33, 0.46484, 0.46484),
                (0.66, 0.67967, 0.67967),
                (1.00, 0.90625, 0.90625)
            ],
            'green': [
                (0.00, 0.00000, 0.26562),
                (0.33, 0.54297, 0.54297),
                (0.66, 0.73828, 0.73828),
                (1.00, 0.83984, 0.83984)
            ],
            'blue': [
                (0.00, 0.00000, 0.29687),
                (0.33, 0.58594, 0.58594),
                (0.66, 0.75000, 0.75000),
                (1.00, 0.73828, 0.73828)
            ]
        },
        'primary_colors': ['#ff4843', '#328bdc', '#5fae5b', '#fbac47'],
        'primary_shadows': ['#d97b78', '#84a8ca', '#88af85', '#d9b876'],
        'helper_colors': ['#e68570', '#688eb5', '#91ab59', '#e1ca61'],
        'background': '#ffffff',
        'gridcolor': '#ffffff',
        'axiscolor': '#000000'
    }
}
