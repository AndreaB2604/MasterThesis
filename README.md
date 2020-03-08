# MasterThesis
This repository contains all the code developed during the master thesis at the Department of Information Engineering (DEI) at the Padova University. During the course we studied, developed, implemented and tested several techniques for the address the problem of the minimisation of the ambulance number in a given region.


## Compilation
To compile the code that solves the model:
* Install IBM ILOG CPLEX (if the version is different from 12.8, 12.9 or 12.10, change the Makefile);
* Compile using `make` in the main folder


## Solve an instance
To run the code to solve an instance use:
```bash
./main -(parameter) <value of the parameter> 
```

For example:
```bash
    ./main -f instances_generator/examples/grid.flow -model flow
```

The available *parameters* as input can be:
```bash
    -file <file>
    -model <the model>
    -random_seed <random seed>		(optional)
    -time_limit <time limit in seconds>		(optional)
```

Use:
```bash
    ./main -help
```

to get more information or
```bash
    ./main -(parameter)
```

to get more information about the parameters.

NOTE: *instances_generator/example/grid.flow* is an example of file that can be used as input in the optimisation software.

The output file is called *solution_flow.txt* and it can be used with the python script to plot the solutions.

## Python plots

### Dependencies
* `numpy`,
* `matplotlib`,
* `networkx`.


### Generate the instances
In *instances_generator*, Python scripts to generate instances are present. In order to generate an instance the following files are needed:
* a black and white image of the region where to generate the population ditribution (see for example *ulss6BW.png*),
* (eventually) a *csv* file containing the top-n most populated cities (see for example *ulss6_pop_distribution.csv*),
* a *.inst* file (see for example *ulss6_inst.inst*) where:
	* `IMG_SRC` is the path to the black and white image,
	* `IMG_PX_PER_KM` are the number of pixels to which a kilometer corresponds in the BW image,
	* `POP_DISTRIBUTION_CSV` is the path to the *csv* file containing the top-n most populated cities. 
	[NOTE: this header can be replaced with `IMG_CENTER`, followed by the coordinates in the BW image of the center of the normal bivariate distribution function, see *ulss9_inst.inst* for example],
	* `SQUARE_EDGE_DIM` is the dimension the the edge of the squares in kilometers,
	* `POPULATION` is the total population of the region,
	* `DEMAND_GENERATION_TYPE` can be for now:
		* `AGENAS_FORMULA` to generate the requests based on the agenas formula,
		* `MAX_REQUESTS_DISTRIBUTION` to generate the requests proportional to the population within each square; the total request must be specified in the `MAX_REQUESTS` parameter (see later),
		* `ALL` to generate the instances with both the two previous parameters in different files.
	* `MAX_REQUESTS` is the maximum request to be distributed in the region according to the randomly generated population,
	* `NUMBER_HOSPITALS` is the number of hospitals,
	* `MAX_DISTANCE` is the radius of coverage of each hospital,
	* `HOSPITALS_COORD_SECTION` is a keyword to notify that in the next `NUMBER_HOSPITALS` rows, the coordinates of each hospital are specified.




### Plot the solutions
The Python script can receive both the *.flow* files, in which case only the demand and service points are plotted, or the *solution_flow.txt* to plot the entire solution

