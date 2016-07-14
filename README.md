# Mytools Package
This package is a collection of tools which I am using for different purposes,
especially generating different kind of plots. Tools (modules) of
this package have been written in a way to be suitable for my own needs.

## Typical usage
Considering to have a Gadget snapshot. By using
[Rockstar](https://bitbucket.org/gfcstanford/rockstar) and `find_parents` we can
find all the halos (and specify their parents if they are not hosts) in the
snapshot. As will be shown below, using reportgenerator, halomassfunction and
halomassprofile modules, we can generate a short report.

```python
>>> from mytools.report.latex import LaTeX

>>> from mytools.plots.halomassfunction import Rockstar as Massfunction

>>> from mytools.plots.halomassprofile import Rockstar as Massprofile

>>> report = LaTeX(
...     '<report-filename>',
...     '<report-title>',
...     authors=['John Doe', 'Jack Smith'],
...     emails=['john@doe.info', 'jack@smith.info'])

>>> mf = Massfunction('<rockstar-filepath>')
... Extracting data using following parameters:
...     skiprows:  	19
...     num_p_col: 	1	num_p
...     mass_col:  	3	mbound_vir
...     pid_col:   	-1	PID

>>> mf.histogram();
... Generating histogram using following parameters
...     nbins:     	20
...
...     Total_particles_processed	['16777216']
...     Ol                       	['0.691100']
...     Box_size                 	['100.000000', 'Mpc/h']
...     Force_resolution_assumed 	['0.013', 'Mpc/h']
...     Number_of_found_hosts    	[45241, 'out of', 49198]
...     Filename                 	<rockstar-filepath>
...     FOF_linking_length       	['0.280000']
...     Om                       	['0.308900']
...     Rockstar_Version         	['0.99.9-RC3+']
...     h                        	['0.677400']
...     Number_of_bins           	20
...     FOF_Refinement_Threshold 	['0.700000']
...     Particle_mass            	['5.10966e+09', 'Msun/h']
...     Unbound_Threshold        	['0.500000']

>>> mf.plot(save=True)
... Plotting using following parameters:
...     xscale         log
...     save           True
...     xlabel         $M\ [h^{-1}M_{\odot}]$
...     yscale         log
...     ylabel         $dn / d\log(M) dV\ [h^3Mpc^{-3}]$

>>> report.section('<halo-mass-function-section-title>')

>>> report.figure(
...     mf.path['plot'],
...     specifier='h',
...     attrs='width=0.6\\textwidth',
...     caption='<caption>')
... 'fig:g5AMwnpOpP'

>>> report.subsection('<subsection-title>')

>>> report.dict2table(mf.headers, specifier='h', excludekeys=['column_tags'])
... 'tab:2AJ6IMA4ro'

>>> report.newpage()

>>> mp = Massprofile('<rockstar-filepath>')
... Extracting Rockstar data
...     pid_col 	-1  	PID
...     r_col   	4   	rvir
...     mass_col	3   	mbound_vir
...     y_col   	9   	y
...     x_col   	8   	x
...     z_col   	10  	z
...     id_col  	0   	id

>>> mp.selecthosts()
... Binning host masses using following parameter(s)
...    Number_of_mass_bins:	12
...
... Index	Average host mass	# of found hosts
... 0	4.23816884254e+13   	97
... 1	528580611797.0   	    6099
... 2	15843736435.2   	    423
... 3	219936760659.0       	12705
... 4	91513342732.5       	17982
... 5	3.05307898089e+12   	1234
... 6	1.27035363406e+12   	2752
... 7	2.44796421242e+14   	19
... 8	7.33755626275e+12   	534
... 9	1.01857182627e+14   	31
... 10	1.76345689863e+13   	241
... 11	38077726856.5   	    2856
... Choose one index: 7 ⏎

>>> mp.loadgadgetsnap('<gadget-snapshot-path>')
... Loading and indexing gadget snapshot (hang tight, it may take some time)

>>> mp.generateprofiles()
... Generating halo mass profiles [12 out of 19]

>>> mp.plot(nfw=True, save=True)
... Parameters review:
...     Min_halo_mass_in_bin	    1.59000008065e+14
...     Total_particles_processed	['16777216']
...     Ol             	            ['0.691100']
...     scale_factor   	            1.0
...     Box_size       	            ['100.000000', 'Mpc/h']
...     Force_resolution_assumed	['0.013', 'Mpc/h']
...     Max_halo_mass_in_bin	    3.79499988386e+14
...     Filename       	            <gadget-snapshot-path>
...     FOF_linking_length	        ['0.280000']
...     Om             	            ['0.308900']
...     Rockstar_Version	        ['0.99.9-RC3+']
...     h              	            ['0.677400']
...     FOF_Refinement_Threshold	['0.700000']
...     Particle_mass  	            ['5.10966e+09', 'Msun/h']
...     Unbound_Threshold	        ['0.500000']
...     Number_of_hosts	            19
...
... Plotting using following parameters:
...     nfw            True
...     save           True
...     xlabel         $\log_{10}(r / kpc)$
...     yscale         log
...     xscale         log
...     ylabel         $\log_{10}(\rho / \rho_{crit})$

>>> report.section('<halo-mass-profile-section-title>')

>>> report.figure(
...     mp.path['plot'],
...     specifier='h',
...     attrs='width=0.55\\textwidth',
...     caption='<caption>')
... 'fig:fH5TKm9qbU'

>>> report.subsection('<subsection-title>')

>>> report.dict2table(mp.headers, specifier='h', excludekeys=['column_tags'])
... 'tab:Rga8KbVzIP'

>>> report.finish()
```

The output of above script is two different plots and also `report.tex` file.
Running `pdflatex` on the `.tex` file, we can generate a pdf version of the
report.
