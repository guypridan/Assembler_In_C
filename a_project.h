#include <stdio.h>

#define MEMORYSIZE 8192
#define MAXLINE 81
#define MAX_LINES_IN_MACRO 6
#define MAXLABELLEN 31
#define MAXCOMLEN 8 /* maximum command length */
#define CC 20 /* commands count */
#define IN 1
#define OUT 0
#define LABELED 1
#define UNLABELED 0
#define FIRST_16_ON 0xffff /* 00001111111111111111 */
#define A (1<<18)
#define R (1<<17)
#define E (1<<16)

#define add_end(p,fname,to_add)     p = malloc(sizeof(fname)+sizeof(to_add));  \
                                    strcpy(p,fname); strcat(p,to_add);
#define def_commands()              char* commands[CC] = {"mov","cmp","add","sub","lea",\
                                                          "clr", "not","inc","dec","jmp",\
                                                          "bne","jsr","red","prn","rts","stop",\
                                                          ".data",".string",".entry",".extern"};

enum err_type {
    INVLD_STR = 1,
    INVLD_CMD,
    INVLD_OP,
    INVLD_COMMA,
    INVLD_LBL,
    INVLD_INT,
    EXTR_TXT,
    EOL,
    LNG_LN,
    WB_FAIL,
    MEM_FAIL
};

typedef struct word_node{
    int word;
    struct word_node *next;
} word_node;

typedef struct label_node{
    char label[MAXLABELLEN];
    int base;
    int offset;
    int ext_flag;
    int ent_flag;
    int c_flag;
    int d_flag;
    struct label_node* next;
} label_node;

typedef struct macro{
    char macro_name[MAXLINE];
    char macro_lines[MAX_LINES_IN_MACRO*MAXLINE];
    struct macro* next;
} macro;


/* phase 1 functions */
int* phase_1(char* filename,label_node** label_list);
void queue_word(word_node* node,word_node** fw,word_node** lw,char *param,int cn,int *ic,int *dc); /* FIFO protocol */
word_node *make_word_block(int cn, char *param_a,char *param_b);
int is_registry(char* param);
int is_ad_type2(char* param);
int get_reg_index(char *param);
int get_param_bits(char *param);
int valid_type_parameters(int src,int dest,int cn);
int is_valid_int(char* s);

/* phase 2 functions */
int phase_2(int memory[],char* filename,label_node *label_list,label_node** externals);
int fill_label_words(int memory[],label_node* label_list,char* param_a,char* param_b,label_node** externals);
int insert_label_loc(int memory[],char* label,label_node* label_list,label_node** externals);

/* macro handling */
int make_am(int argc,char* argv[]);
int macro_parsing(char* fname);
int is_macro_boe(char* s,char* boe); /* checks if begining or end of a macro declarationg */
int is_macro(FILE* am_f,macro* list,char* line);
void macro_list_insert(macro** list,macro* m);
void free_macro_list(macro* ml);

/* input handling */
int get_command(char* line,char* command,char* commands[CC],int *cn);
int get_param(char *line,char *param);
int get_str(char *line,char* str);
int clean_comma(char *line);
int valid_line_length(char* line);
int is_empty(char* line);
int is_comment_line(char *line);
void print_label(label_node* n);
void print_label_list(label_node* head);



/* label handling */
void make_label_node(int lc,char* label,label_node** list,int e_flag,int c_flag,int d_flag);
void insert_label_node(label_node** list,label_node* n);
int is_labeled(char* line,char* label);
int valid_label(char* label,label_node* label_list,char* commands[CC],int is_extern,int is_parameter);
void update_labels(label_node* list,int ic);
int is_entry_label(label_node *list,char *label);
void free_label_list(label_node* ll);

/* export */
void fprint_bitbase(FILE* f,int word);
void make_ob(int memory[],char* filename);
void make_ent(label_node* label_list,char* fname);
void make_ext(label_node* label_list,char* fname,label_node* ext_used);
void print_bitbase(int word);

/* error handling */
void err(char* fname,int lc,int err_type,int* err_flag);