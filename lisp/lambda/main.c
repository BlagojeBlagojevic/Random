#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>



#define WARNING(...)        fprintf(stdout, __VA_ARGS__)
#define ERROR_BREAK(...)    fprintf(stderr, __VA_ARGS__); exit(-1)
#define LOG(...)     			  fprintf(stdout, __VA_ARGS__)
#define PAUSE()           {char a; fputs(a, stdin);}
#define CLAMP(X, LOW, HIGH) {if((X) < (LOW)) (X) = (LOW); if((X) > (HIGH)) (X) = (HIGH);}
#define ASSERT(msg) {fprintf(stderr, "aseert in:\n\tFILE %s\n\tLINE %d\n\tmsg: %s" , __FILE__, __LINE__, msg); exit(-1);}
#define DROP(var) {(void)var;}



#define DA_SIZE 16
#define da_append(da, item)                                                            \
	do {                                                                                 \
		if ((da)->count >= (da)->capacity) {                                               \
			(da)->capacity = (da)->capacity == 0 ? DA_SIZE : (da)->capacity*3;               \
			(da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items));         \
			assert((da)->items != NULL && "Realloc fail !!!");                               \
			}                                                                                \
		\
		(da)->items[(da)->count++] = (item);                                               \
		} while (0)


#define da_remove_unordered(da, i)               					   \
	do {                                             					   \
		int j = (i);                              					   \
		if(j > (da)->count){ASSERT("Not that amount of elements in da");}  \
		(da)->items[j] = (da)->items[--(da)->count]; 					   \
		} while(0)

#define da_remove_last(da) do{ if((da)->count > 0){ (da)->count--; } }while(0)




typedef struct {
    char** items;
    int count;
    int capacity;
}String_B;



#define MAX_BUFFER 256
//Shod change as if arena
#define STRING_BUILD(str_bPTR, ...) \
    {char name[MAX_BUFFER] = {0};\
    int size = snprintf(name, MAX_BUFFER, __VA_ARGS__);\
    da_append(str_bPTR, strndup(name, size));}


#define STRING_FREE(str_b) \
    {for (int i = 0; i < str_b.count; i++){if(str_b.items[i] != NULL) free(str_b.items[i]);}; str_b.count = 0;}    


void string_print(String_B* strPTR){
    for(int i = 0; i < strPTR->count; i++){
        printf("%s", strPTR->items[i]);
    }
}



typedef enum{
    EXP_VAR,
    EXP_FUNC,
    EXP_APP,
}Exp_Kind;

typedef struct Exp Exp;

typedef struct Exp {
    Exp_Kind kind;
    union{
        
        const char* var;
        
        struct{
            const char* arg;
            Exp* body; 
        }func;

        struct {
            Exp *lhs;
            Exp *rhs;
        }app;
    }as;
    
}Exp;


Exp* var(const char* name){
    Exp* exp = malloc(sizeof(*exp));
    assert(exp != NULL);
    exp->kind = EXP_VAR;
    exp->as.var = strdup(name);


    return exp;

}

Exp* fun(const char* arg, Exp* body){
    Exp* exp = malloc(sizeof(*exp));
    assert(exp != NULL);
    exp->kind = EXP_FUNC;
    exp->as.func.arg = strdup(arg);
    exp->as.func.body = body;

    return exp;
    
}

Exp* app(Exp* lhs, Exp* rhs){
    Exp* exp = malloc(sizeof(*exp));
    assert(exp != NULL);
    exp->kind = EXP_APP;
    exp->as.app.lhs = lhs;
    exp->as.app.rhs = rhs;

    return exp;
}

void exp_display(Exp* exp, String_B* str_b){
    const Exp_Kind kind = exp->kind;

    switch (kind){
    case EXP_VAR:
        STRING_BUILD(str_b, "%s", exp->as.var);
        break;
    case EXP_FUNC:
        STRING_BUILD(str_b, "(lambda %s : ", exp->as.func.arg);
        exp_display(exp->as.func.body, str_b);
        STRING_BUILD(str_b, ")");
        break;

     case EXP_APP:
        STRING_BUILD(str_b, "(");
        exp_display(exp->as.app.lhs, str_b);
        STRING_BUILD(str_b, " ");
        exp_display(exp->as.app.rhs, str_b);
        STRING_BUILD(str_b, ")");        
        break;    

    default:
        ASSERT("Unrechabale");
        break;
    }

}


Exp* applay(const char* arg, Exp* body, Exp* val){
    const Exp_Kind kind = body->kind;
    switch (kind)
    {
    case EXP_VAR:
        if(strcmp(body->as.var, arg) == 0){
            return val;
        }
        else return body;
    case EXP_FUNC:
        return fun(body->as.func.arg, applay(arg, body->as.func.body, val));
        
    case EXP_APP:
        return app(applay(arg, body->as.app.lhs, val), applay(arg, body->as.app.rhs, val));
        
    
    default:
        ASSERT("Unrechable");
    }

}

Exp* eval(Exp* exp){
    const Exp_Kind kind = exp->kind;
    switch (kind){
        case EXP_VAR:
        case EXP_FUNC:
            return exp;
            break;
        
        case EXP_APP:
            Exp* lhs = eval(exp->as.app.lhs);
            if(lhs != exp->as.app.lhs){

            }
            if(exp->as.app.lhs->kind == EXP_FUNC){
                return applay(exp->as.app.lhs->as.func.arg, exp->as.app.lhs->as.func.body, exp->as.app.rhs);
            }
            Exp* rhs = eval(exp->as.app.rhs);
            if(rhs != exp->as.app.rhs){
                return app(lhs, rhs);
            }
            return exp;
            break;    
        
        default:
           ASSERT("Unrechable");
    }
}





int main(){
    String_B display = {0};

    //Identity (lambda x : x ) y -> y  
    //Exp* expres = app(fun("x", var("x")), var("y"));
    //Exp *expres = app(fun("x", app(fun("y", var("x")), var("20"))), var("10"));
    Exp* expres = app(fun("x", app(fun("y", var("x")), var("10")) ), var("20"));
    exp_display(expres, &display);   
    string_print(&display);
    printf("\n");
    STRING_FREE(display);
    Exp* eval1 = eval(expres);
    
    exp_display(eval1, &display);
    string_print(&display);
    STRING_FREE(display);
    printf("\n");
    
    Exp* eval2 = eval(eval1);
    
    exp_display(eval2, &display);
    string_print(&display);
    printf("\n");


    
    //Loop 




    return 0;
}