#include <stdio.h>
#include <stdlib.h>
#include "a_project.h"

/* this function proccesses a code writing in the simplified assembly defined in the assingment 
    this function takes code file names without the file type extension as input 
    the input is given as a command line argument 
    outputs .ob file with the appropriate representation of the memory image created while running said code 
    if external or internal labels are used in the code, outputs .ext and .ent files according to the format described in the assingment */
int main(int argc, char* argv[])
{
    label_node *label_list,*externals;
    int i,*memory; 
    /* memory represents a dinamically allocated array in which each int in the array represents a word, where the last 16 bits are ignored
       memory[0] keeps the current size of the array
       memory[1] keeps the instruction count
       memory[2] keeps the data count */
    printf("spreading macros\n");
    make_am(argc,argv); /* spreading macros */
    for (i = 1; i < argc; i++)
    {
        label_list = externals = NULL;
        /* translating code to bits */
        memory = phase_1(argv[i],&label_list);
        if (memory == NULL)  
        {
            free_label_list(label_list);
            free_label_list(externals);
            continue;
        }
        if (!phase_2(memory,argv[i],label_list,&externals))
        {
            free_label_list(label_list);
            free_label_list(externals);
            continue;
        }
        /* creating files */
        make_ob(memory,argv[i]);
        make_ext(label_list,argv[i],externals);
        make_ent(label_list,argv[i]);

        /* frees allocated memory */
        free_label_list(label_list);
        free_label_list(externals);
        label_list = NULL;
    }
    return 1;
}
