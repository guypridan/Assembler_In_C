#include <stdio.h>
#include "a_project.h"

/* this func prints errors to terminal */
void err(char* fname,int lc,int err_type,int* err_flag)
{
    *err_flag = 1;
    printf("\t-----ERROR-----\nIn %s\t",fname);
    switch(err_type)
    {
        case INVLD_STR:
            printf("Invalid string declaration");
            break;
        case INVLD_CMD:
            printf("Invalid command");
            break;
        case INVLD_OP:
            printf("Invalid operand");
            break;
        case INVLD_COMMA:
            printf("Invalid comma use");
            break;
        case INVLD_LBL:
            printf("Invalid label name");
            break;
        case INVLD_INT:
            printf("Invalid int");
            break;
        case EXTR_TXT:
            printf("Extraneus text after last operand");
            break;
        case EOL:
            printf("Expected an operand but reached end of line");
            break;
        case LNG_LN:
            printf("Line is too long");
            break;
        case WB_FAIL:
            printf("Failed while creating word blocks");
            break;
        case MEM_FAIL:
            printf("Failed while allocating memory");
            break;
    }
    printf(" at line %i\n",lc);
}