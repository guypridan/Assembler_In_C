#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "a_project.h"

/* 
    phase 2 of loading code to memory
    phase 2 lifts entry flags
    phase 2 fills words that depend on a label from a diffrent line
*/
int phase_2(int memory[],char* filename,label_node *label_list,label_node** externals)
{
    char *fpath,*param_a,*param_b,line[MAXLINE],label[MAXLABELLEN],command[MAXCOMLEN];
    FILE* file;
    int cn,lc = 0,err_flag = 0;

    def_commands();
    add_end(fpath,filename,".am");
    file = fopen(fpath,"r");
    free(fpath);
    if (file == NULL)   return 0;

    while(fgets(line,MAXLINE,file)) /* itirates through lines - stops at EOF */
    {
        lc++;
        if (is_empty(line) || is_comment_line(line)) continue;
        is_labeled(line,label);
        if (!get_command(line,command,commands,&cn))
        {
            printf("ERROR: invalid command at line %i\n",lc);
            err_flag = 1;
            continue;
        }
        if (cn == 19 || cn == 17 || cn == 16)   continue;
        if (cn == 18) /* .entry */ 
        {

            param_a = malloc(sizeof(char)*MAXLINE);
            if (param_a == NULL)
            {
                printf("ERROR: failed while allocating memory\n");
                err_flag = 1;
                continue;
            }
            get_param(line,param_a);
            if (!is_entry_label(label_list,param_a) || !is_empty(line))
            {
                err(filename,lc,INVLD_LBL,&err_flag);
            }
            free(param_a);
            continue;
        }
        if (cn == 14 || cn == 15) continue; /* no operands - no labels to fill */
        if (cn >= 5 && cn <= 13) /* one operand */
        {
            param_a = malloc(sizeof(char)*MAXLINE);
            get_param(line,param_a);
            param_b = NULL;
        }
        if (cn >= 0 && cn <= 4) /* two operands */
        {
            param_a = malloc(sizeof(char)*MAXLINE);
            param_b = malloc(sizeof(char)*MAXLINE);
            get_param(line,param_a);
            clean_comma(line);
            get_param(line,param_b);
        }
        if (!fill_label_words(memory,label_list,param_a,param_b,externals))
        {
            err(filename,lc,INVLD_LBL,&err_flag);
            continue;
        }
        if (param_a != NULL)    free(param_a);
        if (param_b != NULL)    free(param_b);

    }
    if (err_flag)   return 0;
    return 1;
}

/* fills empty words with the base and offset for the appropriate label */
int fill_label_words(int memory[],label_node* label_list,char* param_a,char* param_b,label_node** externals)
{
    if (!(is_registry(param_a)) && param_a[0] != '#') /* for first operand */
    {
        if (is_ad_type2(param_a))
        {
            param_a[strlen(param_a)-5] = '\0';
        }
        if (!insert_label_loc(memory,param_a,label_list,externals))   return 0;
    }
    if (param_b != NULL && !(is_registry(param_b)) && param_b[0] != '#') /* for second operand if one exists */
    {
        if (is_ad_type2(param_b))
        {
            param_b[strlen(param_b)-5] = '\0';
        }
        if (!insert_label_loc(memory,param_b,label_list,externals))   return 0;
    }
    return 1;
}

/* fills next empty word with the base and offset for the apropriate label */
int insert_label_loc(int memory[],char* label,label_node* label_list,label_node** externals)
{
    static int i = 4; /* start of words in memory */
    label_node* p = label_list;
    if (label_list == NULL)     return 0;
    while (strcmp(p->label,label))
    {
        p = p->next;
        if (p == NULL)  return 0; /* couldn't find operand label in label list */
    }
    while (memory[i] != 0)  i++; /* go to next label word to fill */
    if (p->ext_flag)
    {
        make_label_node(i+96,label,externals,0,0,0);
        memory[i] = memory[i+1] = E;
    }
    else
    {
        memory[i++] = R|p->base;
        memory[i] = R|p->offset;
    }
    return 1;
}