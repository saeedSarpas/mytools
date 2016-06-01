# Plotting modules

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
... Extracting data using following parameters:
...     skiprows:  	19
...     num_p_col: 	1	num_p
...     mass_col:  	3	mbound_vir
...     pid_col:   	-1	PID

>>> r.histogram()
Generating histogram using using following parameters
...     nbins:     	20

... ('Filename', 'filename')
... ('Total_particles_processed', ['16777216'])
... ('Number_of_found_hosts', [45241, 'out of', 49198])
... ('Box_size', ['100.000000', 'Mpc/h'])
... ('Number_of_bins', '20')
... ('Particle_mass', ['5.10966e+09', 'Msun/h'])

>>> r.plot()
... In case you want to save your plot, you should pass,
...     `save=True`
... to the `plot` function.

>>>
```

##### `Rockstar(fname, **kwargs)`
Initializing the rockstar object:

| Argument      | Description                                       | Default |
| :------------ | :------------------------------------------------ | :-----: |
| skiprows      | Number of rows to skip from the rockstar output   | 19      |
| num\_p\_col   | The column number of num\_p (number of particles) | 1       |
| mass\_col     | The column number of mass                         | 3       |
| pid\_col      | The column number of pid (parent_id)              | -1      |

##### `Rockstar.histogram(**kwargs)`
Generating the histogram of hosts:

| Argument      | Description                                       | Default |
| :------------ | :------------------------------------------------ | :-----: |
| nbins         | Number of histogram bins                          | 20      |

##### `Rockstar.plot(**kwargs)`
Plotting the histogram (halo mass function):

| Argument      | Description                                       | Default |
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

*cs: colorscheme from `mytools.sharedtools.colorscheme` module.
