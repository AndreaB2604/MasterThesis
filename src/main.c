#include "tsp.h"

/*   MAIN   */
int main(int argc, char **argv)
{
	instance inst;

	if(argc < 2)
	{
		printf("Usage : %s -help for help\n", argv[0]);
		exit(1);
	}

	init_instance(&inst);

	parse_command_line(argc, argv, &inst);

	read_input(&inst);
	
	// If we will want to save the file to plot in the same directory of the file read
	// char *file_to_plot = (char *) malloc(strlen(inst.input_file)+strlen("_to_plot"));
	// char *str = strcat(strcpy(file_to_plot, inst.input_file), "_to_plot");

	/***** TSP WITH SUBTOUR *****/
	if(!strncmp(inst.model_type, "subtour", 7))
	{
		if(TSPopt_subtour(&inst))
			print_error(" error within TSPopt_subtour()");
	}

	/***** TSP SEC_LOOP *****/
	if(!strncmp(inst.model_type, "sec_loop", 8))
	{
		if(TSPopt_sec_loop(&inst))
			print_error(" error within TSPopt_sec_loop()");
	}

	/***** TSP SEC_LOOP_WRITE *****/
	if(!strncmp(inst.model_type, "write_sec_loop", 14))
	{
		if(TSPopt_write_sec_loop(&inst))
			print_error(" error within TSPopt_sec_loop_write()");
	}

	/***** TSP SEC_LOOP *****/
	if(!strncmp(inst.model_type, "sec_callback", 12))
	{
		if(TSPopt_sec_callback(&inst))
			print_error(" error within TSPopt_sec_callback()");
	}

	print_plot(&inst, "sol_to_plot.txt");

	free_instance(&inst);

	return 0;
}
