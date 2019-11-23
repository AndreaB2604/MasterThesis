#include "flow.h"

/*   MAIN   */
int main(int argc, char **argv)
{
	instance inst;

	init_instance(&inst);
	
	if(argc < 2)
	{
		printf("Usage : %s -help for help\n", argv[0]);
		exit(1);
	}

	parse_command_line(argc, argv, &inst);

	read_input(&inst);
	
	// If we will want to save the file to plot in the same directory of the file read
	// char *file_to_plot = (char *) malloc(strlen(inst.input_file)+strlen("_to_plot"));
	// char *str = strcat(strcpy(file_to_plot, inst.input_file), "_to_plot");

	/***** TSP WITH SUBTOUR *****/
	if(!strncmp(inst.model_type, "flow", 4))
	{
		if(TSPopt(&inst))
			print_error(" error within TSPopt()");
	}

	print_plot(&inst, "plot_file.txt");

	free_instance(&inst);

	return 0;
}
