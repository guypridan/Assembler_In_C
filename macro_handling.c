#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "a_project.h"

/* Pre-assembler phase */
/* this file contains functions required for replacing macros with their declaration */
int make_am(int argc,char* argv[])
{   
    int i;
    for (i = 1; i < argc; i++)
    {
        macro_parsing(argv[i]);
    }
    return 1;
}
/* spreads the macros in file fname */
int macro_parsing(char* fname)
{
    int i,f,macro_flag;
    char line[MAXLINE];
    char macro_name[MAXLINE];
    char macro_lines[MAXLINE*MAX_LINES_IN_MACRO];
    char *amfname,*asfname;

    FILE *as_f, *am_f; /* file pointers for file.as & file.am */

    macro* new_macro;
    macro* macro_list = NULL;

    add_end(asfname,fname,".as");
    add_end(amfname,fname,".am");

    if (!(as_f = fopen(asfname,"r"))) /* checking for file existance */
    {
        printf("could'nt find %s.as\n",fname);
        free(asfname);
        free(amfname);
        return 0;
    }
    am_f = fopen(amfname,"w");

    free(asfname);
    free(amfname);

    if (am_f == NULL)
    {
        printf("ERROR: couldn't open %s.am",fname);
        return 0;
    }
    if (as_f == NULL)
    {
        printf("ERROR: couldn't open %s.as",fname);
        return 0;
    }
    macro_flag = OUT;
    while (fgets(line,MAXLINE,as_f))  /* itirates through lines in file to creat a macro list - stops at EOF */
    {
        i = 0;
        while (isspace(line[i]))    i++; /* skips spaces */
        if (macro_flag == OUT && is_macro_boe(&line[i],"macro")) /* start of a macro declaration */
        {
            macro_flag = IN;
            i=+strlen("macro");
            f = 0; /* an int to assist with string copying */
            while (isspace(line[i]))    i++; /* skips spaces */
            while (!isspace(line[i]))
            {
                macro_name[f] = line[i];
                f++; i++;
            }
            macro_name[f] = '\0';
            macro_lines[0] = '\0';
        }
        else if (macro_flag == IN && is_macro_boe(&line[i],"endm")) /* end of macro declaration */
        {
            macro_flag = OUT;
            new_macro = malloc(sizeof(macro));
            strcpy(new_macro->macro_name,macro_name);
            strcpy(new_macro->macro_lines,macro_lines);
            new_macro->next = NULL;
            macro_list_insert(&macro_list,new_macro);
        }
        else if (macro_flag == IN) /* inside a macro declaration */
        {
            strcat(macro_lines,line);
        }
        else if (is_macro(am_f,macro_list,&line[i])) /* macro name in need of replacing */
        {
            continue;
        }
        else if (macro_flag == OUT) /* not a macro or a macro declaration */
        {
            fprintf(am_f,"%s",line); /* prints line to file with no changes */
        }
    }
    free_macro_list(macro_list);
    fprintf(am_f,"\n");
    fclose(am_f); fclose(as_f);
    return 1;

}

/* checks if begining or end of a macro declarationg */
int is_macro_boe(char* s,char* boe) 
{   
    int i;
    for (i = 0; i < strlen(boe); i++)
    {
        if (s[i] != boe[i])
        {
            return 0;
        }
    }
    return 1;
}

/* checks if line should be replaced by a declared macro
    replaces line with macro if needed */
int is_macro(FILE* am_f,macro* list,char* line)
{
    int i,e;
    macro* m = list;
    while (m != NULL)
    {
        e = strlen(m->macro_name);
        for (i = 0; i < e; i++)
        {
            if (line[i] != m->macro_name[i])
            {
                m = m->next;
                break;
            }
        }
        if (i == e)
            {
                fprintf(am_f,"%s",m->macro_lines);
                return 1;
            }
    }
    return 0;
}

/* inserts a macro to macro list */
void macro_list_insert(macro** list,macro* m)
{
    m->next = *list;
    *list = m;
}

/* frees allocated macro list */
void free_macro_list(macro* ml)
{
    macro* buffer = ml;
    while (buffer != NULL)
    {
        ml = buffer->next;
        free(buffer);
        buffer = ml;
    }
    return;
}