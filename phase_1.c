#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "a_project.h"


/*
    phase 1 of loading code to memory
    phase 1 handles label getting
    filling the memory with words that depends directly to the line read from file
*/

int* phase_1(char* filename,label_node** label_list)
{
    char* fpath; /* file path buffer */
    char line[MAXLINE]; /* line buffer */
    char command[MAXCOMLEN]; /* command buffer */
    char param_a[MAXLINE],param_b[MAXLINE]; /* operands buffers */
    char label[MAXLABELLEN]; /* label buffer */
    int *memory,*ic,*dc,lc,i,err_flag; /* i = instruction | d = data | l = line | c = count | i = index */
    int cn; /* command number */
    FILE* file; /* code file */
    word_node *fd,*ld,*fi,*li,*new__word_node; /* data and instruction queues, pointers to first and last */
    short int label_flag = UNLABELED; /* flaged if current line is labeled */

    def_commands(); /* string array with names of commands */

    fd = ld = fi = li = NULL; /* pointers to the start and end of instruction and data words
    fd - first data | ld - last data | fi - first instruction | li - last instruction */
    err_flag = 0;   /* lift flag if failed any time while running phase 1 */

    memory = calloc(4,sizeof(int));
    memory[0] = 4;

    add_end(fpath,filename,".am");
    file = fopen(fpath,"r");
    free(fpath);
    if (file == NULL)   return NULL;

    ic = &memory[1];
    dc = &memory[2];
    lc = 0;
    while(fgets(line,MAXLINE,file)) /* itirates through lines - stops at EOF */
    {
        lc++;
        if (!valid_line_length(line))
        {
            err(filename,lc,LNG_LN,&err_flag);
            while (!valid_line_length(line))    /* skipping rest of longer then max line */
            {
                fgets(line,MAXLINE,file);
            }
            continue;
        }
        if (is_empty(line) || is_comment_line(line)) continue; /* skipping empty lines and comment lines */
        label_flag = is_labeled(line,label);
        if (label_flag == -1) /* error while label checking */
        {
            err(filename,lc,INVLD_LBL,&err_flag);
            continue;
        }
        if (!get_command(line,command,commands,&cn))
        {
            err(filename,lc,INVLD_CMD,&err_flag);
            continue;
        }
        if (cn == 16 || cn == 17) /* .data or .string */
        {
            if (label_flag == LABELED)
            {
                if (!valid_label(label,*label_list,commands,0,0))
                {
                    err(filename,lc,INVLD_LBL,&err_flag);
                    continue;
                }
                make_label_node(*dc,label,label_list,0,0,1);
            }
            
            while (cn == 16) /* data - itirates through data values loading them to data image*/
            {
                if (!get_param(line,param_a))
                {
                    err(filename,lc,EOL,&err_flag);
                    break;
                }
                if (!is_valid_int(param_a))
                {
                    err(filename,lc,INVLD_INT,&err_flag);
                    break;
                }
                queue_word(NULL,&fd,&ld,param_a,cn,ic,dc);
                if (is_empty(line))
                {
                    break;
                }
                if (!clean_comma(line))
                {
                    err(filename,lc,INVLD_COMMA,&err_flag);
                    break;
                }
            }
            if (cn == 17) /* string */
            {
                if (!get_str(line,param_a))
                {
                    err(filename,lc,INVLD_STR,&err_flag);
                    continue;
                }
                for (i = 1; i < strlen(param_a)-1; i++) /* iterates through strings, queue chars to data list */
                {
                    queue_word(NULL,&fd,&ld,&param_a[i],cn,ic,dc);
                }
                param_a[i] = '\0';
                queue_word(NULL,&fd,&ld,&param_a[i],cn,ic,dc); /* loading '\0' at the end of the string in to data image */
            }
        }
        if (cn == 19) /* .extern - loads label operand to label list*/
        {
            if (!get_param(line,param_a))
            {
                err(filename,lc,EOL,&err_flag);
                continue;
            }
            if (!is_empty(line))
            {
                err(filename,lc,EXTR_TXT,&err_flag);
                continue;
            }
            if (!valid_label(param_a,*label_list,commands,1,0))
            {
                err(filename,lc,INVLD_LBL,&err_flag);
                continue;
            }
            make_label_node(0,param_a,label_list,1,0,0);
        }

        /* instructions */
        if (cn <= 15 && cn >= 0)
        {
            /* labeled line */
            if (label_flag == LABELED)
            {
                if (!valid_label(label,*label_list,commands,0,0))
                {
                    err(filename,lc,INVLD_LBL,&err_flag);
                    continue;
                }
                make_label_node(*ic+100,label,label_list,0,1,0);
            }

            /* getting operands */
            if (cn >= 0 && cn <= 4)  /* 2 operands */
            {
                if (!get_param(line,param_a))
                {
                    err(filename,lc,EOL,&err_flag);
                    continue;
                }
                if (!clean_comma(line))
                {
                    err(filename,lc,INVLD_COMMA,&err_flag);
                    continue;
                }
                if (!get_param(line,param_b))
                {
                    err(filename,lc,EOL,&err_flag);
                    continue;
                }
                if (!is_empty(line))
                {
                    err(filename,lc,EXTR_TXT,&err_flag);
                    continue;
                }
            }
            else if (cn >= 5 && cn <= 13) /* one operand */
            {
                if (!get_param(line,param_a))
                {
                    err(filename,lc,EOL,&err_flag);
                    continue;
                }
                if (!is_empty(line))
                {
                    err(filename,lc,EXTR_TXT,&err_flag);
                    continue;
                }
                param_b[0] = '\0'; /* marks no second operand */
            }
            else /* cn == 14 or cn == 15 - no operands*/
            {
                if (!is_empty(line))
                {
                    err(filename,lc,EXTR_TXT,&err_flag);
                    continue;
                }
                param_a[0] = '\0'; /* marks no operands */
                param_b[0] = '\0';
            }
            for (i = 0; i < CC;i++) /* checking operands validity - command operand clash */
            {
                if (!strcmp(param_a,commands[i]) || !strcmp(param_b,commands[i]))
                {
                    err(filename,lc,INVLD_OP,&err_flag);
                    continue;
                }
            }
            new__word_node = make_word_block(cn,param_a,param_b);
            if (new__word_node == NULL)
            {
                err(filename,lc,WB_FAIL,&err_flag);
                continue;
            }
            queue_word(new__word_node,&fi,&li,NULL,cn,ic,dc);
        }
    }
    /* connecting data words at the end of instruction words */
    if (li != NULL)     li->next = fd;
    else    fi = fd;
    /* loading words to memory image */
    for (i = 4; fi!=NULL; i++)
    {
        if (i > memory[0])
        {
            memory = realloc(memory,memory[0]*2*sizeof(int));
            memory[0] *= 2;
        }
        if (memory[0] >= MEMORYSIZE)
        {
            printf("ERROR:the word image for %s is bigger then memory size",filename);
            return NULL;
        }
        memory[i] = fi->word;
        fi = fi->next;
    }
    /* if found error quit now */
    if (err_flag)   return NULL;

    /* updating data labels location in memory according to instruction count */
    update_labels(*label_list,memory[1]+100);
    return memory;
}

/* makes a word block for a given line that will later be inserted into the memory image */
word_node *make_word_block(int cn, char *param_a,char *param_b)
{
    word_node *buffer = NULL, *node_b = NULL,*node_a = malloc(sizeof(word_node)); 
    int i,funct,dest_param,src_param;
    if (node_a == NULL)     return NULL;
    if (param_a[0] == '\0' && param_b[0] == '\0') /* stp or rts commands - no operands - no extra words */
    {
        node_a->word = A|(1<<cn); /* sets the 18th bit and the cnth bit to 1 */
        node_a->next = NULL;
        return node_a;
    }
    /* getting command number, function number and operands bits */
    else if (param_b[0] == '\0') /* one operand for command */
    {
        if (cn >= 5 && cn <= 8)
        {
            funct = (cn+5)<<12;
            cn = 5;
        }
        else if (cn >= 9 && cn <= 12)
        {
            funct = (cn+1)<<12;
            cn = 9;
            if (is_registry(param_a))
            {
                free(node_a);
                return NULL;
            }
        }
        else    funct = 0;
        dest_param = get_param_bits(param_a);
        src_param = 0; 
    }
    else /* two operands */
    {
        if (cn == 2 || cn == 3)
        {
            funct = (cn+8)<<12;
            cn = 2;
        }
        else funct = 0;
        src_param = get_param_bits(param_a);
        dest_param = get_param_bits(param_b);
    }
    if (!valid_type_parameters(src_param,dest_param,cn)) /* checking operand address type are valid for a given command */
    {
        free(node_a);
        return NULL;
    }

    /* initializing first and second words for current line */
    node_a->word = A|(1<<cn); /* sets the 18th bit and the command num bit to 1 */
    node_b = malloc(sizeof(word_node));
    node_a->next = node_b;
    if (node_b == NULL)
    {
        free(node_a); return NULL;
    }
    node_b->word = A|(FIRST_16_ON&(funct|dest_param|(src_param<<6)));
    node_b->next = NULL;

    /* creating additional words if needed */
    if (src_param&1 && src_param&2) /* addressing type 3 */
    {
        /* add no additional words */
    }
    else if (src_param&1 || src_param&2) /* addressing type 1 or 2 */
    { /* two additional words */
        for (i=0; i<2; i++)
        {
            buffer = node_b;
            node_b = malloc(sizeof(word_node));
            if (node_b == NULL)
            {
                free(node_a); return NULL;
            }
            buffer->next = node_b;
            node_b->word = 0;
            node_b->next = NULL;
        }
    }
    else if (param_b[0] != '\0')  /* source operand addressing type 0 */
    { /* one additional word */
        buffer = node_b;
        node_b = malloc(sizeof(word_node));
        if (node_b == NULL)
        {
            free(node_a); return NULL;
        }
        buffer->next = node_b;
        node_b->word = A|(FIRST_16_ON&atoi(&param_a[1]));
        node_b->next = NULL;
    }

    if (dest_param&1 && dest_param&2) /* addressing type 3 */
    {
        /* add no additional words */
    }
    else if (dest_param&1 || dest_param&2) /* addressing type 1 or 2 */
    { /* two additional word */
        for (i=0; i<2; i++)
        {
            buffer = node_b;
            node_b = malloc(sizeof(word_node));
            if (node_b == NULL)
            {
                free(node_a); return NULL;
            }
            buffer->next = node_b;
            node_b->word = 0;
            node_b->next = NULL;
        }
    }
    else /* destenation operand addressing type 0 */
    { /* one additional word */
        buffer = node_b;
        node_b = malloc(sizeof(word_node));
        if (node_b == NULL)
        {
            free(node_a); return NULL;
        }
        buffer->next = node_b;
        node_b->word = (param_b[0] == '\0') ? A|(FIRST_16_ON&atoi(&param_a[1])) : A|(FIRST_16_ON&atoi(&param_b[1]));
        node_b->next = NULL;
    }
    return node_a;
}

/* loads memory blocks into data and instruction lists, to be later loaded into memory image */
void queue_word(word_node* node,word_node** fw,word_node** lw,char *param,int cn,int *ic,int *dc) /*  */
{
    word_node *p;
    if (node == NULL)
    {
        node = malloc(sizeof(word_node));
        node->next = NULL;
    }
    if (cn == 16)
    {
        node->word = A|(FIRST_16_ON&(atoi(param)));
        (*dc)++;
    }
    else if (cn == 17)
    {
        node->word = A|(FIRST_16_ON&(int)*param);
        (*dc)++;
    }
    else
    {
        p = node;
        while (p != NULL)
        {
            (*ic)++;
            p = p->next;
        }
    }
    if (*fw == NULL)
    {
        *fw = node;
    }
    else (*lw)->next = node;
    p = node;
    while (p->next!=NULL)
    {
        p = p->next;
    }
    *lw = p;
    
}


/* checks if a given parameter is a registry type operand */
int is_registry(char* param)
{
    if (param[0] == 'r' && param[1] >= '0' && param[1] <= '9')
    {
        if (strlen(param) == 2) return 1;
        else if (strlen(param) == 3 && param[1] == '1' && param[2] >= '0' && param[2] <= '9')
        {
            return 1;
        }
    }
    return 0;
}

/* checks if a given parameter is of the second addressing type */
int is_ad_type2(char* param)
{
    int i,n = strlen(param);
    for (i = 0; i < n-4; i++)
    {
        if (param[i] == '[' && i == n-5)    
        {
            if (param[i+1] == 'r' && param[i+2] == '1' && param[i+3] >= '0' && param[i+3] <= '5' && param[i+4] == ']')
            {
                return 1;
            }
        }
    }
    return 0;
}

/* for an operand of addressing type 2, returns the registry number 
    registry num - LABEL[ri]
    returns i*/
int get_reg_index(char *param)
{
    int i=10,n = strlen(param);
    i += atoi(&param[n-2]);
    return i;
}

/* return the addressing type and operand bits to later fill the memory word */
int get_param_bits(char *param)
{   
    if (param[0] == '#') return 0;
    else if (is_registry(param))
    {
        return 3 | atoi(&param[1])<<2;
    }
    else
    {
        if (is_ad_type2(param)) return 2 | get_reg_index(param)<<2;
        else return 1;
    }
}

/* checks that an operand type is valid for a specific command */
int valid_type_parameters(int src,int dest,int cn)
{
    if (dest == 0)
    {
        if (cn >= 0 && cn <= 12 && cn != 1) return 0;
    }
    if (dest%4 == 3)
    {
        if (cn == 9)    return 0;
    }
    if (src == 0 || src%4 == 3)
    {
        if (cn == 4)    return 0;
    }
    return 1;
}

/* checks if an operand is a valid integer */
int is_valid_int(char* s)
{
    int i = 0;
    if (s[i] != '-' && s[i] != '+' && !isdigit(s[i]))
        return 0;
    while (s[++i] != '\0')
    {
        if (!isdigit(s[i]))  return 0;
    }
    i = atoi(s);
    if (i > 32767 || i < -32768) /* checks for buffer overflow */
        printf("Warning: the number %i will cause a buffer overflow, did you mean that?\n",i);
    return 1;
}

