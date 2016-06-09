# Plots

## Halo mass function
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
...
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
| fname         | String                                            |         |
|               | The filename of the Rockstar output               |         |
| skiprows      | Number                                            | 19      |
|               | Number of rows to skip from the rockstar output   |         |
| num\_p\_col   | Number                                            | 1       |
|               | The column number of num\_p (number of particles) |         |
| mass\_col     | Number                                            | 3       |
|               | The column number of mass                         |         |
| pid\_col      | Number                                            | -1      |
|               | The column number of pid (parent\_id)             |         |

##### `Rockstar.histogram(**kwargs)`
Generating the histogram of hosts:

| Argument      | Description                                       | Default |
| :------------ | :------------------------------------------------ | :-----: |
| nbins         | Number                                            | 20      |
|               | Number of histogram bins                          |         |

##### `Rockstar.plot(**kwargs)`
Plotting the histogram (halo mass function):

| Argument      | Description               | Default                         |
| :------------ | :-------------------------| :-----------------------------: |
| save          | Boolean                   | False                           |
|               | Saving the plot on disk   |                                 |
| xscale        | String                    | 'log'                           |
|               | The scale of x axis       |                                 |
| yscale        | String                    | 'log'                           |
|               | The scale of y axis       |                                 |
| label         | String                    | ''                              |
|               | Label of the plot         |                                 |
| color         | String                    | cs*                             |
|               | Color of the line         |                                 |
| ecolor        | String                    | cs*                             |
|               | Color of the error bars   |                                 |
| linestyle     | String                    | 'solid'                         |
|               | Line style of the plot    |                                 |
| xmin          | Number                    |                                 |
|               | Left edge of the x axis   |                                 |
| xmax          | Number                    |                                 |
|               | Right edge of the x axis  |                                 |
| ymin          | Number                    |                                 |
|               | Bottom edge of the y axis |                                 |
| ymax          | Number                    |                                 |
|               | Top edge of the y axis    |                                 |
| xlabel        | String                    | '$M\ [h^{-1}M\_{\odot}]$ '      |
|               | X axis label              |                                 |
| ylabel        | String                    | '$dn / d\log(M) dV\ [h^3Mpc^{-3}]$' |
|               | Y axis label              |                                 |

*cs: colorscheme from `mytools.sharedtools.colorscheme` module.

## Halo mass profile
Mass profile of dark matter halos.

### Rockstar
Generating the dark matter halo mass profile using the output file of
[Rockstar](https://bitbucket.org/gfcstanford/rockstar). By default, the output
of Rockstar do not include information about hosts and subhalos. To find that,
one should run `find_parents` on each halo catalog.

#### Usage
A typical usage could be:

```python
>>> from mytools.plots.halomassprofile import Rockstar
>>> r = Rockstar('filename')
... Extracting data using following parameters:
...     skiprows:  	19
...     id_col:    	0	id
...     mass_col:  	3	mbound_vir
...     x:         	8	x
...     y:         	9	y
...     z:         	10	z
...     pid_col:   	-1	PID

>>> r.findsubhalos()
... Binning host masses using following parameter(s)
...     Number_of_mass_bins:	8
...     nsubhalos:          	1
...     lratio:             	1000  # Mpc/h to kpc/h
...
... Index	Average host mass	Mass range				# of found hosts
... 0	273842704638.0   	(141870180649.98578, 528580611796.68945)  	16347
... 1	3.80138092012e+12   	(1969388224412.5452, 7337556262745.7852)  	1553
... 2	73498938685.4   	(38077726856.54464, 141870180649.98578)  	20046
... 3	1.02028448607e+12   	(528580611796.68945, 1969388224412.5452)  	5209
... 4	5.27693331064e+13   	(27338302951933.164, 101857182627176.14)  	117
... 5	1.41632035937e+13   	(7337556262745.7852, 27338302951933.164)  	456
... 6	1.96608238953e+14   	(101857182627176.14, 379499988385792.56)  	30
... 7	19726996178.4   	(10220000256.000002, 38077726856.54464)  	1215
... Choose one index to stack: 1 ⏎

>>> r.profilesgen()
... Profiling host densities using following parameter(s)
...     Number_of_radius_bins:	20
...
... Stacked 14511 subhalos successfully

>>> r.plot()
... Parameters review:
...     Total_particles_processed     	['16777216']
...     Ol                            	['0.691100']
...     h                             	['0.677400']
...     Number_of_used_hosts          	2
...     Om                            	['0.308900']
...     Box_size                      	['100.000000', 'Mpc/h']
...     Number_of_radius_bins         	20
...     Filename                      	hosts
...     Particle_mass                 	['5.10966e+09', 'Msun/h']
...     Min_num_of_subhalos           	1
...
... [note] In case you want to save your plot, make sure to set
...        `save=True` when you are calling plot function.
...
... Plotting using following parameters:
...     xscale         log
...     yscale         log
...     xlabel         $\log_{10}(r / Kpc)$
...     ylabel         $\log_{10}(\rho / \rho_{crit})$
```

##### `Rockstar(fname, **kwargs)`
Initializing the rockstar object:

| Argument      | Description                                        | Default |
| :------------ | :------------------------------------------------- | :-----: |
| fname         | String                                             |         |
|               | The filename of the Rockstar output                |         |
| skiprows      | Number                                             | 19      |
|               | Number of rows to skip from the rockstar output    |         |
| id\_col       | Number                                             | 0       |
|               | The column number of particle id                   |         |
| mass\_col     | Number                                             | 3       |
|               | The column number of particle mass                 |         |
| x\_col        | Number                                             | 8       |
|               | The column number of particle x position           |         |
| y\_col        | Number                                             | 9       |
|               | The column number of particle y position           |         |
| z\_col        | Number                                             | 10      |
|               | The column number of particle z position           |         |
| pid\_col      | Number                                             | -1      |
|               | The column number of pid (parent\_id)              |         |

##### `Rockstar.findsubhalos(**kwargs)`
Finding all subhalos of a selected host in the same mass bin and stacking them
together.

| Argument      |l Description                                       | Default |
| :------------ | :------------------------------------------------- | :-----: |
| nmassbins     | Number                                             | 8       |
|               | Number of mass bins for stacing                    |         |
| nsubhalos     | Number                                             | 1       |
|               | Minimum number of subhalos in each halo            |         |

##### `Rockstar.profilesgen(**kwargs)`
Profiling the found halos in the selected bin.

| Argument      |l Description                                       | Default |
| :------------ | :------------------------------------------------- | :-----: |
| nradiusbins   | Number                                             | 20      |
|               | Number of radial bins for calculating mass profile |         |

##### `Rockstar.plot(**kwargs)`
Plotting the histogram (halo mass function):

| Argument      | Description               | Default                         |
| :------------ | :-------------------------| :-----------------------------: |
| save          | Boolean                   |                                 |
|               | Saving the plot on disk   |                                 |
| xscale        | String                    | 'log'                           |
|               | The scale of x axis       |                                 |
| yscale        | String                    | 'log'                           |
|               | The scale of y axis       |                                 |
| label         | String                    | ''                              |
|               | Label of the plot         |                                 |
| color         | String                    | cs*                             |
|               | Color of the line         |                                 |
| ecolor        | String                    | cs*                             |
|               | Color of the error bars   |                                 |
| linestyle     | String                    | 'solid'                         |
|               | Line style of the plot    |                                 |
| xmin          | Number                    |                                 |
|               | Left edge of the x axis   |                                 |
| xmax          | Number                    |                                 |
|               | Right edge of the x axis  |                                 |
| ymin          | Number                    |                                 |
|               | Bottom edge of the y axis |                                 |
| ymax          | Number                    |                                 |
|               | Top edge of the y axis    |                                 |
| xlabel        | String                    | '$M\ [h^{-1}M\_{\odot}]$ '      |
|               | X axis label              |                                 |
| ylabel        | String                    | '$dn / d\log(M) dV\ [h^3Mpc^{-3}]$' |
|               | Y axis label              |                                 |

*cs: colorscheme from `mytools.sharedtools.colorscheme` module.
