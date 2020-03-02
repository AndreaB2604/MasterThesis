#include "flow.h"

void build_model(instance *inst, CPXENVptr env, CPXLPptr lp) {

	char continuous = 'C';
	char integer = 'I';

	char *cname = (char *) calloc(100, sizeof(char));

	// for informative purposes
	double max_dist = inst->max_distance;

	// add the y variables to the objective function
	for(int i = 0; i < inst->nhosp; i++) {
		
		sprintf(cname, "y_%d", (i+1));
		double obj = 10.0;
		double lb = 1.0;  
		double ub = inst->c_hosp[i];
		
		if(VERBOSE > 1000) {
			printf("The variable %s number %d has value %lf\n", cname, i, obj);
		}
		if(CPXnewcols(env, lp, 1, &obj, &lb, &ub, &integer, &cname)) {
			print_error(" wrong CPXnewcols on y variables");
		}
		if(CPXgetnumcols(env,lp)-1 != i) {
			print_error(" wrong position for y variables");
		}
	}

	// add the x variables to the objective function
	for(int i = 0; i < inst->nhosp; i++) {
		for(int j = 0; j < inst->nnodes; j++) {
			// Adding variables to model + their respective cost in the
			// objective function
			sprintf(cname, "x_%d_%d", i+1,j+1);
			double obj = dist(i, j, inst);
			double lb = 0.0;  
			double ub = CPX_INFBOUND;

			if(VERBOSE >= 1000) {
				printf("The variable %s number %d has value %lf\n", cname, xpos_flow(i, j, inst)+1, obj);
			}
			if(CPXnewcols(env, lp, 1, &obj, &lb, &ub, &continuous, &cname)) {
				print_error(" wrong CPXnewcols on x var.s");
			}
			if(CPXgetnumcols(env,lp)-1 != xpos_flow(i, j, inst)) {
				print_error(" wrong position for x var.s");
			}
		}
	}
	
	// add the request satisfiability constraint for each node
	for(int j = 0; j < inst->nnodes; j++) {
		
		int lastrow = CPXgetnumrows(env, lp);
		double rhs = inst->r_nodes[j];
		char sense = 'G';
		sprintf(cname, "request(%d)", j+1);   
		if(CPXnewrows(env, lp, 1, &rhs, &sense, NULL, &cname))
			print_error(" wrong CPXnewrows [x1]");

		for(int i = 0; i < inst->nhosp; i++) {
			if(CPXchgcoef(env, lp, lastrow, xpos_flow(i, j, inst), 1.0)) 
				print_error(" wrong CPXchgcoef request contraint");
		}
	}

	// add the constraint of fraction of requests served by each hospital
	for(int i = 0; i < inst->nhosp; i++) {
		int lastrow = CPXgetnumrows(env, lp);
		double k = 1.0;
		double rhs = 0.0;
		char sense = 'G';
		sprintf(cname, "requests_served(%d)", i+1);
		if(CPXnewrows(env, lp, 1, &rhs, &sense, NULL, &cname))
			print_error(" wrong CPXnewrows [x1]");
		
		// change the coefficient for the single y_i
		if(CPXchgcoef(env, lp, lastrow, i, k)) 
			print_error(" wrong CPXchgcoef fraction of requests contraint");
		
		// change the coefficient for the x_i_j
		for(int j = 0; j < inst->nnodes; j++) {
			if(CPXchgcoef(env, lp, lastrow, xpos_flow(i, j, inst), -1.0)) 
				print_error(" wrong CPXchgcoef fraction of requests contraint");
		}
	}

	// add the constraint of infeasibility of all the x pairs
	for(int j = 0; j < inst->nnodes; j++) {
		double min_dist = DBL_MAX;
		int min_dist_idx = -1;
		int counter = 0;
		
		int is_within = inst->r_nodes[j] > DOUBLE_TOL;

		// check how many hospitals are in the radius of max_distance
		for(int i = 0; i < inst->nhosp && is_within; i++) {
			double curr_dist = dist(i, j, inst);
			if(curr_dist > inst->max_distance) {
				if(curr_dist < min_dist) {
					min_dist = curr_dist;
					min_dist_idx = i;
				}
				counter++;
			}
		}

		for(int i = 0; i < inst->nhosp; i++) {
			int lastrow = CPXgetnumrows(env, lp);
			double rhs = 0.0;
			char sense = 'E';
			double curr_dist = dist(i, j, inst);
			// for the hospital outside the radius add the infeasibility for all except the nearest hospital
			if(counter == inst->nhosp && is_within) {
				if(i != min_dist_idx) {
					sprintf(cname, "non_reachability(x_%d_%d)", i+1, j+1);
					if(CPXnewrows(env, lp, 1, &rhs, &sense, NULL, &cname))
						print_error(" wrong CPXnewrows [x1]");

					// change the coefficient for the single x_i
					if(CPXchgcoef(env, lp, lastrow, xpos_flow(i, j, inst), 1.0)) 
						print_error(" wrong CPXchgcoef infeasibility contraint");
				}
				else if(curr_dist > max_dist) {
					// for informative purposes find the max_distance needed to solve the problem
					max_dist = curr_dist;
				}
			}
			else if(curr_dist > inst->max_distance) {
				sprintf(cname, "non_reachability(x_%d_%d)", i+1, j+1);
				if(CPXnewrows(env, lp, 1, &rhs, &sense, NULL, &cname))
					print_error(" wrong CPXnewrows [x1]");

				// change the coefficient for the single x_i
				if(CPXchgcoef(env, lp, lastrow, xpos_flow(i, j, inst), 1.0)) 
					print_error(" wrong CPXchgcoef infeasibility contraint");
			}
		}
	}

	printf("NOTE: the minimum distance to cover all the points is %f\n\n", ceil(max_dist));

	free(cname);
}

int flow_opt(instance *inst)
{
	// open cplex model
	int error;
	int cur_numcols;
	double obj_val;

	CPXENVptr env = CPXopenCPLEX(&error);
	CPXsetintparam(env, CPXPARAM_Read_DataCheck, 1);			// used to check if there are errors while reading data
	CPXsetintparam(env, CPXPARAM_RandomSeed, inst->random_seed);
	CPXLPptr lp = CPXcreateprob(env, &error, "AMBULANCE FLOW"); 
	
	if(VERBOSE >= 50)
	{
		CPXsetlogfilename(env, "exec_log.txt", "w");			// it saves the log of the computation in exec_compact_log.txt
	}
	
	// build model
	build_model(inst, env, lp);

	// save model
	if(VERBOSE >= 100)
	{
		CPXwriteprob(env, lp, "flow.lp", NULL); 
	}

	// solve the optimisation problem
	if(CPXmipopt(env, lp))
		print_error("Optimisation failed in TSPopt()");
 
	cur_numcols = CPXgetnumcols(env, lp);
	
	// get the optimal solution of the variables
	inst->best_sol = (double *) calloc(cur_numcols, sizeof(double));
	if(CPXgetx(env, lp, inst->best_sol, 0, cur_numcols-1))
	{
		print_error("Failure to read the optimal solution in CPXgetx()");
	}

	if(VERBOSE >= 1000)
		print_plot(inst, NULL);

	// get the best solution and print it
	if(CPXgetobjval(env, lp, &obj_val))
		print_error("Failure to read the value of the optimal solution in CPXgetobjval()");

	// print the y variables and their sum
	double sum = 0;
	for(int i = 0; i < inst->nhosp; i++) {
		sum += inst->best_sol[i];
		printf("y_%d = %f\n", i+1, inst->best_sol[i]);
	}
	printf("Ambulances required = %lf\n", sum);
	printf("\nSolution value  = %lf\n", obj_val);

	// Free up the problem as allocated by CPXcreateprob, if necessary
	if(lp != NULL)
	{
		CPXfreeprob(env, &lp);
	}

	// Free up the CPLEX environment, if necessary
	if(env != NULL) 
	{
		CPXcloseCPLEX(&env);
	}

	return 0;
}


int xpos_flow(int i, int j, instance *inst) {
	return (inst->nhosp + inst->nnodes*i + j);
}
