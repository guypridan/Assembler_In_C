#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "a_project.h"

/* creates a new object file to represent the memory image 
    format according to definition in assignment */
void make_ob(int memory[],char* fname)
{
    char* fpath;
    FILE* file;
    int i = 4;
    printf("making %s.ob\n",fname);
    add_end(fpath,fname,".ob")
    file = fopen(fpath,"w");
    free(fpath);
    fprintf(file,"\t%d\t%d\n",memory[1],memory[2]);
    while (i <= memory[1]+memory[2]+3)
    {
        fprintf(file,"%04i\t",i+96); /* +96 because memory starts at 100 acording to assignment */
        fprint_bitbase(file,memory[i++]);
    }

}

void print_bitbase(int word)
{
    int a,b,c,d,e;
    e = word&0xf;
    d = (word&0xf0)>>4;
    c = (word&0xf00)>>8;
    b = (word&0xf000)>>12;
    a = (word&0xf0000)>>16;
    printf("A%x-B%x-C%x-D%x-E%x\n",a,b,c,d,e);
}

void fprint_bitbase(FILE* f,int word)
{
    int a,b,c,d,e;
    e = word&0xf;
    d = (word&0xf0)>>4;
    c = (word&0xf00)>>8;
    b = (word&0xf000)>>12;
    a = (word&0xf0000)>>16;
    fprintf(f,"A%x-B%x-C%x-D%x-E%x\n",a,b,c,d,e);
}

/* creates a new .ent file to represent the entries defined in the code
    format according to definition in assignment */
void make_ent(label_node* label_list,char* fname)
{
    char* fpath;
    FILE* file;
    label_node* p = label_list;
    if (label_list == NULL) return;
    printf("making %s.ent\n",fname);
    while (1)       /* checks for existing entries in file */
    {
        if (p->ent_flag == 1)   break;
        if (p->next == NULL)    return;
        p = p->next;
    }
    p = label_list;
    add_end(fpath,fname,".ent")
    file = fopen(fpath,"w");
    free(fpath);
    while (p != NULL)
    {
        if (p->ent_flag)
        {
            fprintf(file,"%s,%i,%i\n",p->label,p->base,p->offset);
        }
        p = p->next;
    }
}

/* creates a new .ext file to represent the external labels required for this code
    format according to definition in assignment */
void make_ext(label_node* label_list,char* fname,label_node* ext_used)
{
    char* fpath;
    FILE* file;
    int base, offset;
    label_node *lp = label_list,*ext_p;
    if (label_list == NULL) return;
    printf("making %s.ext\n",fname);
    while (1)       /* checks for existing entries in file */
    {
        if (lp->ext_flag == 1)   break;
        if (lp->next == NULL)    return;
        lp = lp->next;
    }
    lp = label_list;
    add_end(fpath,fname,".ext")
    file = fopen(fpath,"w");
    free(fpath);
    while (lp != NULL)
    {
        ext_p = ext_used;
        if (lp->ext_flag)
        {
            while (ext_p != NULL)
            {
                if (!strcmp(ext_p->label,lp->label))
                {
                    base = ext_p->base+ext_p->offset;
                    offset = base+1;
                    fprintf(file,"%s\tBASE\t%i\n%s\tOFFSET\t%i\n\n",lp->label,base,lp->label,offset);
                }
                ext_p = ext_p->next;
            }
        }
        lp = lp->next;
    }
}