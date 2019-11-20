#include "tsp.h"

void build_cbc_model(instance *inst, Cbc_Model *mod)
{
	int n = inst->nnodes;
	char isInteger = 1;
	double lb = 0.0;
	double ub = 1.0;

	char *name = (char *) calloc(100, sizeof(char));

	// adding all the variables (the columns)
	for(int i = 0; i < n; ++i)
	{
		for(int j = i+1; j < n; ++j)
		{
			sprintf(name, "x_%d_%d", i+1, j+1);

			double c_ij = dist(i, j, inst);

			Cbc_addCol(mod, name, lb, ub, c_ij, isInteger, 0, NULL, NULL);
		}
	}

	// adding all the constraints (the rows)
	int nz = n-1;	// number of variables with non-zero coefficients in this row
	char sense = 'E'; // constraint sense: L if <=, G if >=, E if =, R if ranged and N if free
	double rhs = 2.0; // right hand size
	double coefs[nz]; // cofficients of the variables in the constraint
	for(int i = 0; i < nz; ++i)
		coefs[i] = 1.0;
	for(int h = 0; h < n; ++h)
	{
		sprintf(name, "deg(%d)", h+1); // constraint name
		
		int cols[n-1];	// position of the variables to set (in terms of columns)
		for(int i = 0; i < h; ++i)
			cols[i] = xpos(i, h, inst);
		for(int i = h+1; i < n; ++i)
			cols[i-1] = xpos(h, i, inst);

		Cbc_addRow(mod, name, nz, cols, coefs, sense, rhs);
	}

	free(name);
}

int TSPopt_subtour(instance *inst)
{
	int status;
	Cbc_Model *mod = Cbc_newModel();
	Cbc_setProblemName(mod, "TSP WITH SUBTOUR");
	Cbc_setLogLevel(mod, LOG_LEVEL);

	build_cbc_model(inst, mod);

	if(VERBOSE >= 100)
		Cbc_writeLp(mod, "subtour");

	unsigned long start = microseconds();
	if((status = Cbc_solve(mod))) 
	{
		printf("Status = %d", status);
		print_error("No optimal solution found in TSPopt_subtour()");
	}
	unsigned long end = microseconds();
	double elapsed = (end - start) / 1000000.0;

	int num_cols = Cbc_getNumCols(mod);

	double obj_val = Cbc_getObjValue(mod);
	const double *xsol = Cbc_getColSolution(mod);
	
	inst->best_sol = (double *) calloc(num_cols, sizeof(double));
	for(int i = 0; i < num_cols; ++i)
		inst->best_sol[i] = xsol[i];

	if(VERBOSE >= 200)
		print_best_sol(inst);

	printf("Execution time of sec_loop = %.3f s\n", elapsed);
	printf("\nSolution value  = %lf\n", obj_val);
	
	return 0;
}