#include "tsp.h"

int i = 0;

int TSPopt_sec_callback(instance *inst)
{
	Cbc_Model *mod = Cbc_newModel();
	Cbc_setProblemName(mod, "TSP_SEC_CALLBACK");
	Cbc_setLogLevel(mod, LOG_LEVEL);
	build_cbc_model(inst, mod);

	if(VERBOSE >= 100)
		Cbc_writeLp(mod, "sec_callback");

	Cbc_setParameter(mod, "preprocess", "off");
	Cbc_setParameter(mod, "cuts", "off");
	Cbc_setParameter(mod, "gomory", "ifmove");
	//Cbc_setParameter(mod, "scaling", "off");
	//Cbc_setParameter(mod, "threads", "4");

	int status;
	int num_cols = Cbc_getNumCols(mod);
	
	Cbc_addCutCallback(mod,  sec_callback, "sec_callback", inst, 1, 1);
	
	// solve the optimisation problem	
	unsigned long start = microseconds();
	if((status = Cbc_solve(mod)))
	{
		printf("Status = %d", status);
		print_error("No optimal solution found in TSPopt_sec_callback()");
	}
	unsigned long end = microseconds();
	double elapsed = (end - start) / 1000000.0;

	// get the optimal solution of the variables
	const double *xsol = Cbc_getColSolution(mod);
	inst->best_sol = (double *) calloc(num_cols, sizeof(double));
	for(int i = 0; i < num_cols; ++i)
		inst->best_sol[i] = xsol[i];

	double obj_val = Cbc_getObjValue(mod);

	if(VERBOSE >= 100)
		Cbc_writeLp(mod, "sec_callback");

	if(VERBOSE >= 100)
		print_best_sol(inst);

	printf("Execution time of sec_callback = %.3f s\n", elapsed);
	printf("\nSolution value  = %lf\n", obj_val);

	if(mod != NULL)
		Cbc_deleteModel(mod);

	return 0;
}


void sec_callback(void *mod, void *mod_cut, void *appData)
{
	printf("Ci passo %d\n", ++i);

	instance *inst = (instance *) appData;

	// get solution xstar
	int ncols = inst->nnodes * (inst->nnodes - 1) / 2;
	const double *node_sol = Osi_getColSolution(mod);
	double *xstar = (double *) calloc(ncols, sizeof(double));

	for(int i = 0; i < ncols; i++)
		xstar[i] = node_sol[i];

	//apply cut separator and possibly add violated cuts
	myseparation(inst, xstar, mod_cut);

	free(xstar);
}

int myseparation(instance *inst, double *xstar, void *mod_cut)
{
	int num_comp;
	int comp[inst->nnodes];
	int conn_comp[inst->nnodes];

	int cur_numcols = inst->nnodes * (inst->nnodes - 1) / 2; 

	connected_components(inst, xstar, cur_numcols, comp, conn_comp, &num_comp);

	if(VERBOSE >= 3000)
	{
		// print the component for each node
		for(int i = 0; i < inst->nnodes; i++)
		{
			printf("comp[%d] = %d\n", (i+1), (comp[i]+1));
		}
	}

	if(VERBOSE >= 1000)
	{
		for(int i = 0; i < num_comp; i++)
		{
			printf("conn_comp[%d] = %d\n", i, (conn_comp[i]+1));
		}
		printf("Number of connected components = %d\n", num_comp);
	}

	if(num_comp == 1)
		return 0;

	// I have to add the SECs
	for(int k = 0; k < num_comp; k++)
	{
		int num_x_var = (inst->nnodes-1) * inst->nnodes / 2; 	// number of x variables
		int nz = 0;					// number of non-zero variables in the constraint
		double rhs = -1.0;
		char sense = 'L';
		double coef[num_x_var];		// coefficients of the non-zero variables
		int idx[num_x_var]; 		// position of the variables to set (in terms of columns)

		for(int i = 0; i < num_x_var; i++)
		{
			coef[i] = 0;
		}

		for(int i = 0; i < inst->nnodes; i++)
		{
			if(comp[i] == conn_comp[k])
			{
				rhs += 1.0;
				for(int j = i+1; j < inst->nnodes; j++)
				{
					if(comp[j] == conn_comp[k])
					{
						idx[nz] = xpos(i, j, inst);
						coef[nz] = 1.0;
						nz++;
					}
				}
			}
		}

		OsiCuts_addRowCut(mod_cut, nz, idx, coef, sense, rhs);
	}

	return num_comp;
}