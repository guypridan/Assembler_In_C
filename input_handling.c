#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "a_project.h"

/* gets command from code line */
int get_command(char* line,char* command,char* commands[CC],int *cn)
{
    int f,i = 0;
    while (isspace(line[i]))    i++;
    f = i;
    while (!isspace(line[i]))
    {
        if (i-f == MAXCOMLEN)  return 0; /* longer then the longest possible command, therefore invalid */
        command[i-f] = line[i];
        line[i] = ' '; /* cleaning line for easier parameter getting */
        i++;
    }

    command[i-f] = '\0';
    for (i = 0; i < CC; i++)
    {
        if (!strcmp(commands[i],command))
        {
            *cn = i;
            return 1;
        }
    }
    return 0;
}

/* gets operand from code line */
int get_param(char *line,char *param)
{
    int i,f;
    i = f = 0;
    while (isspace(line[i]))
    {
        i++; /* skipping spaces before parameter */
        if (i == MAXLINE || line[i] == '\n' || line[i] == '\0')   return 0;
    }
    while (!isspace(line[i]) && line[i] != ',') /* copying parameter */
    {
        param[f] = line[i];
        if (line[i] != '\0')    line[i] = ' ';
        f++; i++;
    }
    param[f] = '\0';
    return 1;
}

/* gets string from code line */
int get_str(char *line,char* str)
{
    int i,f;
    i = f = 0;
    while (isspace(line[i]))
    {
        i++; /* skipping spaces before parameter */
        if (i == MAXLINE || line[i] == '\n')   return 0;
    }
    str[f++] = line[i];
    line[i++] = ' ';
    while(line[i] != '"' )
    {
        str[f++] = line[i];
        line[i++] = ' ';
        if (i == strlen(line))  return 0; /* reached end of line */
    }
    str[f++] = line[i];
    line[i] = ' ';
    str[f] = '\0';
    if (!(str[0] == '"' && str[strlen(str)-1] == '"') || !is_empty(line))
        return 0;
    else
        return 1;
}

/* removes a single comma seperating two operands in code */ 
int clean_comma(char *line)
{
    int i;
    i = 0;
    while (isspace(line[i]))
    {
        i++;
    }    
    if (line[i] == ',')
    {
        line[i] = ' ';
    }
    else
    {
        return 0;
    }
    while (isspace(line[i]))    i++;
    if (line[i] == ',')
    {
        return 0;
    }
    else    return 1;
}

/* checks that code line is in max line length bounds */
int valid_line_length(char* line)
{
    int i;
    for (i = 0; i < MAXLINE; i++)
    {
        if (line[i] == '\n')    return 1;
    }
    return 0;

}

/* checks if a given line is empty */
int is_empty(char* line)
{
    int i = 0;
    int n = strlen(line);
    while (isspace(line[i]) && i < n)
    {
        i++;
    }
    if (i == n)  return 1;
    else        return 0;
}

/* check if this line is a comment line */
int is_comment_line(char *line)
{
    int i = 0;
    while (isspace(line[i]))
    {
        i++;
        if (i == MAXLINE)   return 0;
    }
    if (line[i] == ';') return 1;
    else return 0;
}