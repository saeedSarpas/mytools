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

## Reportgenerator module
Generating a LaTex report

### `Report(fname, title, **kwargs)`
Initializing the Report object.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| fname         | String                                            |           |
|               | The name of the tex file                          |           |
| title         | String                                            |           |
|               | Title of the report                               |           |
| authors       | List of strings (optional)                        |           |
|               | Authors name. Beside this you should set emails.  |           |
| emails        | List of strings (optional)                        |           |
|               | Authors emails. Necessary for creating the title. |           |
| abstract      | String (optional)                                 |           |
|               | Abstract of the report.                           |           |

### `Report.section(title, **kwargs)`
Creating a new section.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| title         | String                                            |           |
|               | Title of the section.                             |           |
| text          | String (optional)                                 |           |
|               | Descriptiotn of the section.                      |           |

### `Report.subsection(title, **kwargs)`
Creating a new subsection.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| title         | String                                            |           |
|               | Title of the subsection.                          |           |
| text          | String (optional)                                 |           |
|               | Descriptiotn of the subsection.                   |           |

### `Report.subsubsection(title, **kwargs)`
Creating a new subsubsection.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| title         | String                                            |           |
|               | Title of the subsubsection.                       |           |
| text          | String (optional)                                 |           |
|               | Descriptiotn of the subsubsection.                |           |

### `Report.addparagraph(text)`
Adding a new paragraph of text to the report.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| text          | String                                            |           |
|               | Paragraph text.                                   |           |

### `Report.addfigure(path, **kwargs)`
Addding a new figure to the report.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| path          | String                                            |           |
|               | Ralative path to the picture                      |           |
| specifier     | String (optional)                                 |           |
|               | Figure specifier, e.g. h or h!                    | 'h'       |
| attrs         | String (optional)                                 |           |
|               | Includegraphics attributes, e.g. 'width=7cm'      |           |
| caption       | String (optional)                                 |           |
|               | Figure caption.                                   |           |

This function returns the figure label.

### `Report.addtableofadict(dictionary, **kwargs)`
Addding a new figure to the report.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| dictionary    | dict                                              |           |
|               | Set of keywords values for filling the table      |           |
| specifier     | String (optional)                                 |           |
|               | Table specifier, e.g. h or h!                     | 'h!'      |
| caption       | String (optional)                                 |           |
|               | Table caption.                                    |           |
| excludekeys   | Array of strings                                  |           |
|               | Keys to be filtered                               |           |

This function returns the table label.

### `Report.finish(**kwargs)`
Finalizing the report document.

| Argument      | Description                                       | Default   |
| :------------ | :------------------------------------------------ | :-------: |
| refrences     | String (optional)                                 |           |
|               | Path to the refrencese bib file.                  |           |
