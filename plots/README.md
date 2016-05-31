# Plot

## Halo Mass function
Mass distribution of dark matter halos.

### Rockstar
[Rockstar](https://bitbucket.org/gfcstanford/rockstar) is an implementation of
adaptive hierarchical refinement of friends-of-friends algorithm for identifying
dark matter halos. By default, the output of Rockstar do not include information
about hosts and subhalos. To find that, one should run `find_parents` on each
halo catalog.

#### Usage
A typical usage could be:

```python
>>> from mytools.plots.halomassfunction import Rockstar
>>> r = Rockstar('filename')
>>> r.histogram()
>>> r.plot()
```

##### `Rockstar(fname, **kwargs)`
Initializing the rockstar object:

| Argument      | Definition                                        | Default |
| :------------ | :------------------------------------------------ | :-----: |
| skiprows      | Number of rows to skip from the rockstar output   | 19      |
| num\_p\_col   | The column number of num\_p (number of particles) | 1       |
| mass\_col     | The column number of mass                         | 3       |
| pid\_col      | The column number of pid (parent_id)              | -1      |

##### `Rockstar.histogram(**kwargs)`
Generating the histogram of hosts:

| Argument      | Definition                                        | Default |
| :------------ | :------------------------------------------------ | :-----: |
| nbins         | Number of histogram bins                          | 20      |

##### `Rockstar.plot(**kwargs)`
Plotting the histogram (halo mass function):

| Argument      | Definition                                        | Default |
| :------------ | :------------------------------------------------ | :-----: |
| save          | Saving the plot on disk                           | False   |
| xscale        | The scale of x axis                               | 'log'   |
| yscale        | The scale of y axis                               | 'log'   |
| label         | Label of the plot                                 | ''      |
| color         | Color of the line                                 | cs*     |
| ecolor        | Color of the error bars                           | cs*     |
| linestyle     | Line style of the plot                            | 'solid' |
| xmin          | Left edge of the x axis                           |         |
| xmax          | Right edge of the x axis                          |         |
| ymin          | Bottom edge of the y axis                         |         |
| ymax          | Top edge of the y axis                            |         |
