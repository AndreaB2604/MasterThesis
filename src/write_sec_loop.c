#include "tsp.h"

int TSPopt_write_sec_loop(instance *inst)
{
	Cbc_Model *mod = Cbc_newModel();
	Cbc_Model *solve_mod = Cbc_newModel();
	Cbc_setProblemName(mod, "TSP_WITH_SUBTOUR");
	Cbc_setLogLevel(mod, LOG_LEVEL);
	
	build_cbc_model(inst, mod);

	int status;
	int done = 0;
	int num_cols = Cbc_getNumCols(mod);
	inst->best_sol = (double *) calloc(num_cols, sizeof(double));
	unsigned long start = microseconds();
	while(!done)
	{
		Cbc_writeLp(mod, "write_sec_loop");
		Cbc_readLp(solve_mod, "write_sec_loop.lp");

		//Cbc_setParameter(solve_mod, "threads", "4");

		// solve the optimisation problem
		if((status = Cbc_solve(solve_mod)))
		{
			printf("Status = %d", status);
			print_error("No optimal solution found in TSPopt_write_sec_loop()");
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