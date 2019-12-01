#include "flow.h"

#define LINE_LENGTH 180

double dist_att(int i, int j, instance *inst);
double dist_ceil2D(int i, int j, instance *inst);
double dist_euc2D(int i, int j, instance *inst);
void print_header_flow(instance *inst, FILE *file);
void print_sol_flow(instance *inst, char *plot_file_name);

double dist(int i, int j, instance *inst)
{
	if(!strncmp(inst->dist_type, "EUC_2D", 6))
		return dist_euc2D(i, j, inst);
	else if(!strncmp(inst->dist_type, "CEIL_2D", 7))
		return dist_ceil2D(i, j, inst);
	else if(!strncmp(inst->dist_type, "ATT", 3))
		return dist_att(i, j, inst);
	else {
		print_error(" format error:  only ATT, CEIL_2D and EUC_2D distances implemented so far!");
		return -1;
	}
}

double dist_att(int i, int j, instance *inst)
{
	double dx = inst->x_hosp[i] - inst->x_nodes[j];
	double dy = inst->y_hosp[i] - inst->y_nodes[j];

	double rij = sqrt((dx*dx + dy*dy) / 10.0);
	double tij = (int) (rij + 0.5);

	if(tij < rij)
		return tij + 1;
	else
		return tij;
}

double dist_euc2D(int i, int j, instance *inst)
{
	double dx = inst->x_hosp[i] - inst->x_nodes[j];
	double dy = inst->y_hosp[i] - inst->y_nodes[j];
	double dis = sqrt(dx*dx + dy*dy);
	return dis;
}

double dist_ceil2D(int i, int j, instance *inst)
{
	double dx = inst->x_hosp[i] - inst->x_nodes[j];
	double dy = inst->y_hosp[i] - inst->y_nodes[j];
	int dis = ceil(sqrt(dx*dx + dy*dy)); // ceiling
	return ((double) dis);
}

void free_instance(instance *inst)
{
	free(inst->x_nodes);
	free(inst->y_nodes);
	free(inst->r_nodes);
	free(inst->x_hosp);
	free(inst->y_hosp);
	free(inst->c_hosp);

	free(inst->dist_type);
	free(inst->input_file);
	free(inst->model_type);
	free(inst->best_sol);
}

void init_instance(instance *inst)
{
	inst->x_nodes = NULL;
	inst->y_nodes = NULL;
	inst->r_nodes = NULL;
	inst->x_hosp = NULL;
	inst->y_hosp = NULL;
	inst->c_hosp = NULL;

	inst->dist_type = NULL;
	inst->input_file = NULL;
	inst->model_type = NULL;
	inst->best_sol = NULL;
}

unsigned long microseconds()
{
	struct timeval start;
	unsigned long time;
	gettimeofday(&start, NULL);
	time = start.tv_sec * 1000000 + start.tv_usec;
	return time;
}

void parse_command_line(int argc, char** argv, instance *inst) 
{ 
	int i;
	if(VERBOSE >= 100)
		printf(" running %s with %d parameters\n", argv[0], argc-1); 
		
	// default values
	inst->input_file = (char *) calloc(strlen("NULL\n"), sizeof(char));
	strcpy(inst->input_file, "NULL");

	inst->model_type = (char *) calloc(strlen("NULL\n"), sizeof(char));
	strcpy(inst->model_type, "NULL");

	inst->time_limit = 2147483647;	// random number for now
	inst->random_seed = 201709013;	// default random seed for CPLEX 12.8

	int help = 0;
	if(argc < 1)
		help = 1;
	for(i = 1; i < argc; i++)
	{
		// input file
		int param_exists = (argc != i+1);
		if(!strcmp(argv[i], "-file") || !strcmp(argv[i],"-f"))
		{
			if(param_exists)
			{
				inst->input_file = (char *) realloc(inst->input_file, strlen(argv[++i])+1); 
				strcpy(inst->input_file, argv[i]);
			}
			else
			{
				printf("\nUse -file <input_file>\n");
				help = 1;
				break;
			}
			
		}
		else if(!strcmp(argv[i],"-time_limit"))		// total time limit
		{
			if(param_exists)
			{
				inst->time_limit = atof(argv[++i]);
			}
			else
			{
				printf("\nUse -time_limit <number of seconds of time limit>\n");
				help = 1;
				break;
			}
		}
		else if(!strcmp(argv[i],"-random_seed"))	// total time limit
		{
			if(param_exists)
			{
				inst->random_seed = atol(argv[++i]);
			}
			else
			{
				printf("\nUse -random_seed <integer specifying the random seed>\n");
				printf("NOTE: in heuristics the random seed is useless\n");
				help = 1;
				break;
			}
		}
		else if(!strcmp(argv[i],"-model"))	// model type
		{
			if(param_exists)
			{
				inst->model_type = (char *) realloc(inst->model_type, strlen(argv[++i])+1);
				strcpy(inst->model_type, argv[i]);
			}
			if((strncmp(inst->model_type, "flow", 4)) || !param_exists)  
		    {
		    	printf("\nModel type non supported, choose between:\n");
		    	printf("flow : optimisation with flow model\n");
		    	fflush(NULL); 
				help = 1;
				break;
		    }
		} 	
		else if(!strcmp(argv[i], "-help") || !strcmp(argv[i], "--help")) { help = 1; break; }		// help mutually exclusive
		else { help = 1; break; }
	}

	if(help || (VERBOSE >= 10))
	{
		printf("\n\n");
		printf("available parameters ---------------------------------------------------\n");
		printf("-file %s\n", inst->input_file); 
		printf("-model %s\n", inst->model_type);
		printf("-random_seed %ld\n", inst->random_seed);
		printf("-time_limit %lf\n", inst->time_limit);
		printf("\nenter -help or --help for help\n");
		printf("------------------------------------------------------------------------\n\n");
	}

	if(help) exit(1);
}


void print_error(const char *err)
{
	printf("\n\n ERROR: %s \n\n", err); 
	fflush(NULL); 
	exit(1);
}

void print_header_flow(instance *inst, FILE *file) {
	fprintf(file, "NAME : SOLUTION_FLOW\n");
	fprintf(file, "DIMENSION : %d\n", inst->nnodes);
	fprintf(file, "NUMBER_HOSPITALS : %d\n", inst->nhosp);
	fprintf(file, "EDGE_WEIGHT_TYPE : %s", inst->dist_type);
	fprintf(file, "COMMENT : FOR POINTS (n_point x_coord y_coord request)\n");
	fprintf(file, "COMMENT : FOR HOSPITALS (n_point x_coord y_coord max_capacity)\n");
	fprintf(file, "NODE_COORD_SECTION\n");

	for(int i = 0; i < inst->nnodes; i++)
		fprintf(file, "%d %f %f\n", i+1, inst->x_nodes[i], inst->y_nodes[i]);

	fprintf(file, "\nHOSPITALS_COORD_SECTION\n");
	for(int i = 0; i < inst->nhosp; i++)
		fprintf(file, "%d %f %f\n", i+1, inst->x_hosp[i], inst->y_hosp[i]);
}

void print_plot(instance *inst, char *plot_file_name)
{
	if(!strncmp(inst->model_type, "flow", 4))
		print_sol_flow(inst, plot_file_name);
	else 
		print_error(" format error: plot non supported in print_plot()!");
}

void print_sol_flow(instance *inst, char *plot_file_name)
{
	FILE *file;

	if(plot_file_name == NULL)
		file = stdout;
	else
		file = fopen(plot_file_name, "w");
	
	print_header_flow(inst, file);

	fprintf(file, "\nSOLUTION\n");
	for(int i = 0; i < inst->nhosp; i++)
		fprintf(file, "y_%d = %f\n", i+1, inst->best_sol[i]);
	
	// print the x variables
	for(int i = 0; i < inst->nhosp; i++)
		for(int j = 0; j < inst->nnodes; j++)
			if(fabs(inst->best_sol[xpos_flow(i, j, inst)]) > DOUBLE_TOL)
				fprintf(file, "x_%d_%d = %f\n", i+1, j+1, inst->best_sol[xpos_flow(i, j, inst)]);

	fclose(file);
}

void read_input(instance *inst)
{
	FILE *fin = fopen(inst->input_file, "r");
    if(fin == NULL) print_error("input file not found!");

    if(!strncmp(inst->model_type, "NULL", 4))
    	print_error("model type not specified!");
    inst->nnodes = -1;
    inst->nhosp = -1;
    inst->max_distance = -1;

    char line[LINE_LENGTH];
    
    char *par_name;   
    char *token1;
    char *token2;
    char *token3;
    
    int active_section = 0; // =1 NODE_COORD_SECTION, =2 DEMAND_SECTION, =3 DEPOT_SECTION 
    
    int do_print = (VERBOSE >= 1000);
	
    while(fgets(line, sizeof(line), fin) != NULL) 
	{
		if(VERBOSE >= 2000) {
			printf("%s", line); 
			fflush(NULL);
		}
		if(strlen(line) <= 1) 
			continue; // skip empty lines
		
		par_name = strtok(line, " :");
		
		if(VERBOSE >= 3000) {
			printf("parameter \"%s\"\n", par_name); fflush(NULL); 
		}

		if(!strncmp(par_name, "NAME", 4)) 
		{
			active_section = 0;
			continue;
		}

		if(!strncmp(par_name, "COMMENT", 7)) 
		{
			active_section = 0;   
			token1 = strtok(NULL, "");  
			continue;
		}

		if(!strncmp(par_name, "DIMENSION", 9)) 
		{
			if(inst->nnodes >= 0) 
				print_error(" repeated DIMENSION section in input file");
			token1 = strtok(NULL, " :");
			inst->nnodes = atoi(token1);
			if(do_print) 
				printf(" ... nnodes %d\n", inst->nnodes); 
			inst->x_nodes = (double *) calloc(inst->nnodes, sizeof(double)); 	 
			inst->y_nodes = (double *) calloc(inst->nnodes, sizeof(double));    
			inst->r_nodes = (double *) calloc(inst->nnodes, sizeof(double));    
			active_section = 0;  
			continue;
		}

		if(!strncmp(par_name, "NUMBER_HOSPITALS", 15)) 
		{
			if(inst->nhosp >= 0) 
				print_error(" repeated NUMBER_HOSPITALS section in input file");
			token1 = strtok(NULL, " :");
			inst->nhosp = atoi(token1);
			if(do_print) 
				printf(" ... nhosp %d\n", inst->nhosp); 
			inst->x_hosp = (double *) calloc(inst->nhosp, sizeof(double)); 	 
			inst->y_hosp = (double *) calloc(inst->nhosp, sizeof(double));    
			inst->c_hosp = (double *) calloc(inst->nhosp, sizeof(double));    
			active_section = 0;  
			continue;
		}

		if(!strncmp(par_name, "MAX_DISTANCE", 12)) 
		{
			if(inst->max_distance >= 0)
				print_error(" repeated MAX_DISTANCE section in input file");
			token1 = strtok(NULL, " :");
			inst->max_distance = atoi(token1);
			if(do_print) 
				printf(" ... max distance %f\n", inst->max_distance); 
			active_section = 0;  
			continue;
		}

		if(!strncmp(par_name, "EDGE_WEIGHT_TYPE", 16)) 
		{
			token1 = strtok(NULL, " :");
			if(!strncmp(token1, "EUC_2D", 6))
			{
				inst->dist_type = (char *) calloc(strlen(token1)+1, sizeof(char));
				strcpy(inst->dist_type, token1);
			}
			else if(!strncmp(token1, "CEIL_2D", 7))
			{
				inst->dist_type = (char *) calloc(strlen(token1)+1, sizeof(char));
				strcpy(inst->dist_type, token1);
			}
			else if(!strncmp(token1, "GEO", 3))
			{
				inst->dist_type = (char *) calloc(strlen(token1)+1, sizeof(char));
				strcpy(inst->dist_type, token1);
			}
			else if(!strncmp(token1, "ATT", 3))
			{
				inst->dist_type = (char *) calloc(strlen(token1)+1, sizeof(char));
				strcpy(inst->dist_type, token1);
			}
			else
			{
				print_error(" format error:  only ATT, EUC_2D, CEIL_2D and GEO distances implemented so far!");
			}
			active_section = 0;
			continue;
		}

		if(!strncmp(par_name, "NODE_COORD_SECTION", 18))
		{
			if(inst->nnodes <= 0) 
				print_error(" ... DIMENSION section should appear before NODE_COORD_SECTION section");
			active_section = 1;
			continue;
		}

		if(!strncmp(par_name, "HOSPITALS_COORD_SECTION", 23))
		{
			if(inst->nhosp <= 0) 
				print_error(" ... NUMBER_HOSPITAL section should appear before NODE_COORD_SECTION section");
			active_section = 2;
			continue;
		}

		if(!strncmp(par_name, "EOF", 3)) 
		{
			active_section = 0;
			break;
		}

		if(active_section == 1) // within NODE_COORD_SECTION
		{
			int i = atoi(par_name) - 1;
			if(i < 0 || i >= inst->nnodes)
				print_error(" ... unknown node in NODE_COORD_SECTION");     
			token1 = strtok(NULL, " :,");
			token2 = strtok(NULL, " :,");
			token3 = strtok(NULL, " :,");
			inst->x_nodes[i] = atof(token1);
			inst->y_nodes[i] = atof(token2);
			inst->r_nodes[i] = atof(token3);
			if(do_print)
				printf(" ... node %d at coordinates ( %f , %f ) with request %f\n", i+1, inst->x_nodes[i], inst->y_nodes[i], inst->r_nodes[i]); 
			continue;
		}

		if(active_section == 2) // within HOSPITALS_COORD_SECTION
		{
			int i = atoi(par_name) - 1; 
			if(i < 0 || i >= inst->nnodes)
				print_error(" ... unknown node in HOSPITALS_COORD_SECTION");     
			token1 = strtok(NULL, " :,");
			token2 = strtok(NULL, " :,");
			token3 = strtok(NULL, " :,");
			inst->x_hosp[i] = atof(token1);
			inst->y_hosp[i] = atof(token2);
			inst->c_hosp[i] = atof(token3);
			if(do_print)
				printf(" ... hospital %d at coordinates ( %f , %f ) with capacity %f\n", i+1, inst->x_hosp[i], inst->y_hosp[i], inst->c_hosp[i]); 
			continue;
		}

		printf(" final active section %d\n", active_section);
		print_error(" ... wrong format for the current simplified parser!!!!!!!!!");
	}
	fclose(fin);
}