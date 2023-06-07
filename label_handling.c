#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "a_project.h"

void insert_label_node(label_node** list,label_node* n) 
{
    n->next = *list;
    *list = n;
    return;
}

void make_label_node(int lc,char* label,label_node** ll,int e_flag,int c_flag,int d_flag)
{
    label_node* node = malloc(sizeof(label_node));
    strcpy(node->label,label);
    node->base = ((lc)/16)*16;
    node->offset = (lc)%16;
    node->ext_flag = e_flag;
    node->ent_flag = 0;
    node->c_flag = c_flag;
    node->d_flag = d_flag;
    insert_label_node(ll,node);
    return;
}

/* this function checks if a command line is labeled
   if a command line is labeled the label is copied to char* label
   the label is also cleared from command line for easier parsing in upcoming functions */
int is_labeled(char* line,char* label)
{
    int i,f,h;
    i = 0;
    while (isspace(line[i])) i++;
    f = i;
    while (!isspace(line[i]))
    {
        if (line[i] == ':')
        {
            if (!isalpha(line[f])) /* label must start with an alphabetic char */
            {
                return -1;
            }
            for (h = f; h < i; h++)
            {
                label[h] = line[h];
                line[h] = ' ';
            }
            label[i] = '\0';
            line[i] = ' ';
            return LABELED;
        }
        if (line[i] == ',') return UNLABELED; 
        if (i - f == 30) return -1; /* according to maximum label length */
        i++;
    }
    return UNLABELED;
}

/* checks if a valid label according to the assingment deffinition*/
int valid_label(char* label,label_node* label_list,char* commands[CC],int is_extern,int is_parameter)
{
    int i;
    label_node* p = label_list;
    for(i = 0; i < CC; i++)
    {
        if(!strcmp(label,commands[i])) return 0; /* label cant be command name in assembely */

    }
    while (p != NULL)
    {
        if(!strcmp(label,p->label)) /* label is already in use by another command line */
        {
            if (is_extern)
            {
                if (p->ext_flag == 0)
                {
                    return 0;
                }
            }
            else    return 0;
        }
        p = p->next;
    }
    if (is_registry(label)) return 0; /* label cant be a rigestry */
    return 1;
}

/* at the end of first phase, updates data words location accordind to the instruction word count */
void update_labels(label_node* list,int ic)
{
    int lc;
    label_node* p = list;
    while (p != NULL)
    {
        if (p->d_flag)
        {
            lc = p->base+p->offset+ic;
            p->base = ((lc)/16)*16;
            p->offset = (lc)%16;
            p = p->next;
        }
        else    p = p->next;
    }
    
}

/* checks if label is a valid operand for .entry command */
int is_entry_label(label_node *list,char *label)
{
    label_node *p = list;
    while (p != NULL)
    {
        if (!strcmp(p->label,label))
        {
            p->ent_flag = 1;
            return 1;
        }
        p = p->next;
    }
    return 0;
}

/* frees allocated label list */
void free_label_list(label_node* ll)
{
    label_node* buffer = ll;
    while (buffer != NULL)
    {
        ll = buffer->next;
        free(buffer);
        buffer = ll;
    }
    return;
}