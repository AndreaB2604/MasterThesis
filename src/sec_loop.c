#include "tsp.h"

int TSPopt_sec_loop(instance *inst)
{
	Cbc_Model *mod = Cbc_newModel();
	Cbc_Model *solve_mod = NULL;
	Cbc_setProblemName(mod, "TSP_WITH_SUBTOUR");
	Cbc_setLogLevel(mod, LOG_LEVEL);
	build_cbc_model(inst, mod);

	if(VERBOSE >= 100)
		Cbc_writeLp(mod, "sec_loop");

	int status;
	int done = 0;
	int num_cols = Cbc_getNumCols(mod);
	inst->best_sol = (double *) calloc(num_cols, sizeof(double));
	unsigned long start = microseconds();
	while(!done)
	{
		solve_mod = Cbc_clone(mod);
		//Cbc_setParameter(solve_mod, "threads", "4");

		// solve the optimisation problem
		if((status = Cbc_solve(solve_mod)))
		{
			printf("Status = %d", status);
			print_error("No optimal solution found in TSPopt_sec_loop()");
		}
		
		// get the optimal solution of the variables
		const double *xsol = Cbc_getColSolution(solve_mod);
	
		for(int i = 0; i < num_cols; ++i)
			inst->best_sol[i] = xsol[i];

		// check if have to add (new) SECs
		int res_conn_comp = sec_loop(solve_mod, mod, inst);
		if(!res_conn_comp && (Cbc_isProvenOptimal(solve_mod) || Cbc_isProvenInfeasible(solve_mod)))
		{
			done = 1;
		}
	}
	unsigned long end = microseconds();
	double elapsed = (end - start) / 1000000.0;

	double obj_val = Cbc_getObjValue(solve_mod);

	if(VERBOSE >= 200)
		print_best_sol(inst);

	printf("Execution time of sec_loop = %.3f s\n", elapsed);
	printf("\nSolution value  = %lf\n", obj_val);


	if(mod != NULL)
		Cbc_deleteModel(mod);
	if(solve_mod != NULL)
		Cbc_deleteModel(solve_mod);

	return 0;
}

// check if we have to add SECs and in case add it
int sec_loop(Cbc_Model *old_mod, Cbc_Model *new_mod, instance *inst)
{
	int num_comp;
	int comp[inst->nnodes];
	int conn_comp[inst->nnodes];

	int cur_numcols = Cbc_getNumCols(old_mod);

	connected_components(inst, inst->best_sol, cur_numcols, comp, conn_comp, &num_comp);

	if(VERBOSE > 1000)
	{
		// print the component for each node
		for(int i = 0; i < inst->nnodes; i++)
		{
			printf("comp[%d] = %d\n", (i+1), (comp[i]+1));
		}
	}

	if(VERBOSE > 1000)
	{
		for(int i = 0; i < num_comp; i++)
		{
			printf("conn_comp[%d] = %d\n", i, (conn_comp[i]+1));
		}
		printf("Number of connected components = %d\n", num_comp);
	}

	if(num_comp == 1)
	{
		return 0;
	}

	// I have to add the SECs
	char *name = (char *) calloc(100, sizeof(char));
	for(int k = 0; k < num_comp; k++)
	{
		int nz = 0;						// number of non-zero variables in the constraint
		double rhs = -1.0;
		char sense = 'L';
		double *coefs = (double *) calloc(cur_numcols, sizeof(double));	// cofficients of the variables in the constraint
		int *cols = (int *) calloc(cur_numcols, sizeof(int)); 			// position of the variables to set (in terms of columns)
		sprintf(name, "SEC_number_%d", k+1);							// constraint name

		for(int i = 0; i < inst->nnodes; i++)
		{
			if(comp[i] == conn_comp[k])
			{
				rhs += 1.0;
				for(int j = i+1; j < inst->nnodes; j++)
				{
					if(comp[j] == conn_comp[k])
					{
						cols[nz] = xpos(i, j, inst);
						coefs[nz] = 1.0;
						nz++;
					}
				}
			}
		}

		Cbc_addRow(new_mod, name, nz, cols, coefs, sense, rhs);

		free(coefs);
		free(cols);
	}

	free(name);

	return 1;
}