# Shared tools

## Mycolordict module
Containing the color schemes.

### `get(colorscheme)`
Returning the dictionary, containing the spectrum of color scheme and also some
special color for different situation. Currently, there is only one color
scheme in this module.
Following is the description of input parameters.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| colorscheme   | String                                            |           |
|               | The name of the preferred color scheme            |           |

## Headerextractor module
Extracting meaningful information from the header of input files.

### `run(fname, ftype)`
Starting extraction of the information. Following is a list of parameters of
this function.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| fname         | String                                            |           |
|               | Name of the input file                            |           |
| ftype         | String                                            |           |
|               | The type of the input file (currently only        |           |
|               | 'rockstar')                                       |           |

## Plotting module
Tools for generating different plots

### `errorbars(plot, fname, save, **kwargs)`
Creating plots with error bars.
As inputs, this function accept three mandatory parameters and also a set of
keywords. Following we list these parameters.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| plot          | (dict: 'x', 'y', 'xerr', 'yerr')                  |           |
|               | Contains necessary parameters for plotting        |           |
| fname         | String                                            |           |
|               | File name for saving. (save should true)          |           |
| save          | Boolean                                           |           |
|               | Whether you want to save the plot in disk or not  |           |
| scheme        | String                                            | 'AUTU...' |
|               | Color scheme name.                                |           |
| label         | String                                            | ''        |
|               | Plot label                                        |           |
| color         | String                                            | 'AUTU...' |
|               | Color of the main line in the plot                |           |
| ecolor        | String                                            | 'AUTU...' |
|               | Color of the error bars                           |           |
| linestyle     | String                                            | 'solid'   |
|               | Style of the main line                            |           |
| xscale        | String                                            |           |
|               | Scale of the x axis                               |           |
| yscale        | String                                            |           |
|               | Scale of the y axis                               |           |
| xmin          | Number                                            |           |
|               | Minimum of the x axis                             |           |
| xmax          | Number                                            |           |
|               | Maximum of the x axis                             |           |
| ymin          | Number                                            |           |
|               | Minimum of the y axis                             |           |
| ymax          | Number                                            |           |
|               | Maximum of the y axis                             |           |
| xlabel        | String                                            |           |
|               | Label of the x axis                               |           |
| ylabel        | String                                            |           |
|               | Label of the y axis                               |           |
