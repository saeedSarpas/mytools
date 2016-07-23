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
        'ecolor': '#febb00',
        'linecolor': '#000000',
        'facecolor': '#fef4ea',
        'background': '#f2f2f2',
        'gridcolor': '#ffffff',
        'axiscolor': '#444444'
    }
}