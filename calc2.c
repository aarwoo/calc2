/**
 * @author master aarwoo
 */
#define _USE_MATH_DEFINES
#include<stdio.h>
#include<stdlib.h>
#include<tgmath.h>
#include<string.h>
#include<ctype.h>
typedef long double complex num_t;
typedef struct{
    char v[26]; //only 26 alpha
    rsize_t v_count;
    void* e;
    char tp;
    #define USER_DEF_FUNC ('u')
    #define INIT_FUNC ('i')
    #define EMPTY_FUNC ('e')
    #define __func_empty__(f_var) f_var.tp = EMPTY_FUNC 
}func;
typedef struct{
    char* name;
    //内置函数名必须为字符串常量(否则需手动管理内存)
    //参见 update_func_name
    func f;
}named_func;
typedef struct{
    union{
        bool b;
        num_t n;
        char* e;
        named_func* f;
    }val;
    char tp;
    #define FUNC_TYPE ('f') //function reference
    #define NUM_TYPE  ('n')
    #define BOOL_TYPE ('b')
    #define ERR_TYPE  ('e')
    #define VOID_TYPE ('v')
    #define STATEMENT_TYPE ('s') //a STATEMENT TYPE is only used for statement
}value;
typedef struct{
    value x[26]; //only 26 arg
    rsize_t x_count;
    #define TOO_MANY_ARG (27)
    #define EMPTY_ARG (28)
    #define ILL_ARG (29)
}arg;
typedef struct lookup lookup;
struct lookup{
    named_func nf;
    lookup* next;
};
typedef struct{
    named_func* (*search_func)(char*);
    char* expr_begin;
    char* expr_end;
    rsize_t v_count;
    char* v;
    value* vals;
}closure;

lookup table = {.nf.name = nullptr,__func_empty__(.nf.f),.next = nullptr}; //初始化table
void auto_free_name(named_func* nf,char* name){
    //智能释放
    if(nf -> name == name){
        return;
    }else{
        free(name); 
        return;
    }
}

void update_func_name(named_func* nf,char* name){
    if(nf -> name == name || nf -> f.tp == INIT_FUNC){
        //内置函数(INIT_FUNC) nf-> name 默认为字符串常量, 无需释放
        //可见 set_all_init_func
        nf -> name = name;
        return;
    }else{
        free(nf -> name); 
        //如果是用户自定义的函数，这一部分同步释放了nf -> f.e
        //可见 try_define_func
        nf -> name = name;
        return;
    }
}




void print_func_direct(named_func nf){
    if(nf.name != nullptr){
        printf("\e[1;33m%s\e[0m",nf.name);
    }else{
        printf("\e[1;33m?\e[0m");
    }
    func f = nf.f;
    switch(f.tp){
        case USER_DEF_FUNC:
            printf("\e[1m(\e[0m");
            if(f.v_count == 0){
                //无参数
                printf("\e[1m)=\e[0m");
            }else{
                //有参数
                for(rsize_t i = 0;i < (f.v_count - 1); i++){
                    printf("\e[1;36m%c\e[0m\e[1m,\e[0m",f.v[i]);
                }
                printf("\e[1;36m%c\e[0m\e[1m)=\e[0m",f.v[f.v_count - 1]);
            }
            printf("\e[1;34m%s\e[0m\n",(char*)f.e);
            return;
        case INIT_FUNC:
            printf("\e[1m(\e[0m");
            if(f.v_count == 0){
                //无参数
                printf("\e[1m)=\e[0m");
            }else{
                //有参数
                for(rsize_t i = 0;i < (f.v_count - 1); i++){
                    printf("\e[1;36m%c\e[0m\e[1m,\e[0m",'a' + i);
                }
                printf("\e[1;36m%c\e[0m\e[1m)=\e[0m", 'a' + (f.v_count - 1));
            }
            printf("\e[1;34m? as init function\e[0m\n");
            return;
        case EMPTY_FUNC:
            printf("\e[1;31m has not defined\e[0m\n");
            return;
        default:
            printf("\e[1;31m undelcared function type\e[0m\n");
            return;
    }
}
void print_num(num_t n){
    long double real = creal(n);
    long double imag = cimag(n);

    #define __sgn__(n,positive_sgn) ((signbit(n) != 0) ? "-" /*where n < 0*/: #positive_sgn /*where n >= 0*/)

    #define __print_num__(n) \
    if(isinf(n)){\
        printf("%sINF",__sgn__(n,));\
    }else if(isnan(n)){\
        printf("%sNAN",__sgn__(n,));\
    }else{\
        __mingw_printf("%.16Lg",n);\
    }
    
    if(imag == 0){
        __print_num__(real)
    }else{
        if(real != 0){
            __print_num__(real)
            printf(" %s ",__sgn__(imag,+));
            imag = fabs(imag);
        }
        if(imag == -1){
            /*real == 0*/
            printf("-I");
        }else if(imag == 1){
            printf("I");
        }else{
            __print_num__(imag)
            printf(" * I");
        }
    }
    printf("\n");
    #undef __print_num__
    #undef __sgn__
}
void print_value(value val){
    #define colorful_print(config,printer)\
        printf("\e["#config"m");\
        printer;\
        printf("\e[0m");

    switch (val.tp){
        case FUNC_TYPE:
            print_func_direct(*(val.val.f));
            return;
        case NUM_TYPE:
            colorful_print(
                1;32,
                print_num(val.val.n)
            )
            return;
        case BOOL_TYPE:
            colorful_print(
                1;32,
                printf("%s\n",(val.val.b)?("true"):("false"))
            );
            return;
        case ERR_TYPE:
            colorful_print(
                1;31,
                printf("%s\n",val.val.e)
            );
            return;
        case VOID_TYPE:
            colorful_print(
                1;32,
                printf("void\n")
            );
            return;
        case STATEMENT_TYPE:
            colorful_print(
                1;2;37,
                printf("done\n")
            );
            return;
        default:
            colorful_print(
                1;31,
                printf("undeclare type\n")
            );
            return;
    }
    #undef colorful_print
}
void print_func(named_func* (*search_func)(char*),char* name){
    named_func nf = *search_func(name);
    print_func_direct(nf);
}

closure closure_of(named_func* (*search_func)(char*),char* expr_begin,char* expr_end,rsize_t v_count, char* v, value* vals){
    return (closure){
        .search_func = search_func,
        .expr_begin = expr_begin,
        .expr_end = expr_end,
        .v_count = v_count,
        .v = v,
        .vals = vals
    };
}
value func_value(named_func* nf){
    return (value){.val.f = nf,.tp = FUNC_TYPE};
}
value bool_value(bool b){
    return (value){.val.b = b, .tp = BOOL_TYPE};
}
value num_value(num_t n){
    return (value){.val.n = n, .tp = NUM_TYPE};
}
value err_value(char* e){
    return (value){.val.e = e,.tp = ERR_TYPE};
}
value void_value(){
    return (value){.tp = VOID_TYPE};
}
value statement_value(){
    return (value){.tp = STATEMENT_TYPE};
}
func func_empty(){
    return (func){__func_empty__()};
}
func func_user_def(char* vs,char* expr){
    func tmp;
    tmp.e = expr;
    tmp.tp = USER_DEF_FUNC;
    bool set[26];
    for(int i = 0; i < 26; i++){
        set[i] = false;
    }
    for(
        tmp.v_count = 0; 
        tmp.v_count < 26 && ('a' <= vs[tmp.v_count] && vs[tmp.v_count] <= 'z') && set[vs[tmp.v_count] - 'a'] == false;
        tmp.v_count++
    ){
        tmp.v[tmp.v_count] = vs[tmp.v_count];
        set[vs[tmp.v_count] - 'a'] = true;
    }
    if(vs[tmp.v_count] == '\0'){
        return tmp;
    }else if(tmp.v_count == 26){
        puts("more than 26 variables");
        return func_empty();
    }else if(vs[tmp.v_count] < 'a' || 'z' < vs[tmp.v_count]){
        puts("invalid variable(only support a-z)");
        return func_empty();
    }else if(set[vs[tmp.v_count] - 'a']){
        printf("variable %c has been used\n", vs[tmp.v_count]);
        return func_empty();
    }else{
        puts("unknown error");
        return func_empty();
    }
}
func func_init_0(num_t (*f)()){
    return (func){
        .v_count = 0,
        .tp = INIT_FUNC,
        .e = f
    };
}
func func_init_1(num_t (*f)(num_t)){
    return (func){
        .v_count = 1,
        .tp = INIT_FUNC,
        .e = f
    };
}
func func_init_2(num_t (*f)(num_t,num_t)){
    return (func){
        .v_count = 2,
        .tp = INIT_FUNC,
        .e = f
    };
}
arg too_many_arg(){
    return (arg){.x_count = TOO_MANY_ARG};
}
arg empty_arg(){
    return (arg){.x_count = EMPTY_ARG};
}
arg ill_arg(){
    return (arg){.x_count = ILL_ARG};
}
void set(char* name,func f,named_func* (*search_func)(char*)){
    named_func* p = search_func(name);
    if(p -> f.tp == EMPTY_FUNC){
        update_func_name(p,name);
        p -> f = f;
        printf("\e[1;35m[Define]\e[0m ");
        print_func(search_func,name);
    }else{
        printf("\e[1;2;35m[Define]\e[0m ");
        print_func(search_func,name);
        update_func_name(p,name);
        p -> f = f;
        printf("\e[1;35m[Redefine]\e[0m ");
        print_func(search_func,name);
    }
    return;
}
value try_define_func(char* expr,named_func* (*search_func)(char*)){
    char* lq = strpbrk(expr,"([{");
    char* rq = strpbrk(expr,")]}");
    char* eq = strchr(expr,'=');
    if(
            ('a' <= *expr && *expr <= 'z')
        &&  (lq != NULL)
        &&  (rq != NULL)
        &&  (eq == rq + 1)
        &&  (isspace(*(eq + 1)) == 0)
    ){
        for(char* n = expr;n < lq;n++){
            if( ! ('a' <= *n && *n <= 'z')){
                return bool_value(false);
            }
        }
        char vs[27];
        char* pvs = vs;
        if(rq == lq + 1){
            //无参数
            *pvs = '\0';
        }else{
            for(char* v = lq + 1; v != eq; v = v + 2){
                if( ('a' <= *v && *v <= 'z') && ( strchr(",)]}",*(v + 1)) != nullptr )){
                    *pvs = *v;
                    pvs++;
                }else{
                    return bool_value(false);
                }
            }
            *pvs = '\0';
        }
        char* expr_copied = strdup(expr);
        if(expr_copied != nullptr){
            char* func_name = expr_copied;
            char* func_expr = expr_copied + (eq - expr + 1);
            *(func_name + (lq - expr)) = '\0';
            //这么做,最多浪费54字节/次定义
            set(
                func_name,
                func_user_def(vs,func_expr),
                search_func
            );
            return bool_value(true);
        }else{
            return err_value("copyied expr failed");
        }
    }else{
        return bool_value(false);
    }
}
#define ext_c(name,prefix) \
num_t ext_c##name##l(num_t val){\
    return prefix##name(val);\
}
//将实函数映射到复数的实部与虚部
#define ext_c_map_real_imag(name) \
num_t ext_c##name##l(num_t val){\
    return name(creal(val)) + name(cimag(val)) * I;\
}
ext_c(real,c)
ext_c(imag,c)
ext_c(abs,f)
ext_c(arg,c)
ext_c(conj,)
ext_c_map_real_imag(floor)
ext_c_map_real_imag(ceil)
ext_c_map_real_imag(round)
#undef ext_c_map_real_imag
#undef ext_c

void set_all_init_func(named_func* (*search_func)(char*)){
    #define __set_init_func__(name,arg_counts) set(#name,func_init_##arg_counts(c##name##l),search_func)
    #define __set_init_func__ext(name,arg_counts) set(#name,func_init_##arg_counts(ext_c##name##l),search_func)
    __set_init_func__(sin,1);
    __set_init_func__(cos,1);
    __set_init_func__(tan,1);

    __set_init_func__(asin,1);
    __set_init_func__(acos,1);
    __set_init_func__(atan,1);

    __set_init_func__(sinh,1);
    __set_init_func__(cosh,1);
    __set_init_func__(tanh,1);
    
    __set_init_func__(asinh,1);
    __set_init_func__(acosh,1);
    __set_init_func__(atanh,1);

    __set_init_func__(exp,1);
    __set_init_func__(log,1);
    __set_init_func__(sqrt,1);

    __set_init_func__ext(real,1);
    __set_init_func__ext(imag,1);
    __set_init_func__ext(abs,1);
    __set_init_func__ext(arg,1);
    __set_init_func__ext(conj,1);
    __set_init_func__(proj,1);

    __set_init_func__ext(floor,1);
    __set_init_func__ext(ceil,1);
    __set_init_func__ext(round,1);
    #undef __set_init_func__
    #undef __set_init_func__ext
}

bool is_quote_paired(char* src,rsize_t len){
    //括号是否匹配
    char* stack = calloc(len,sizeof(char));
    char* p = stack;
    #define push(ch) \
        *p = (ch);\
         p++;
    #define pop_if(ch) \
        if(*(p - 1) == (ch)){\
            p--;\
        }
    char* begin = src;
    char* end = src + len;
    for(char* c = begin; c < end; c++){
        switch(*c){
            case '(':
            case '[':
            case '{':
                push(*c)
                break;
            case ')':
                pop_if('(')
            case ']':
                pop_if('[')
            case '}':
                pop_if('{')
        }
        #undef push
        #undef pop_if
    }
    bool is_paired = (p == stack);
    free(stack);
    return is_paired;
}
char getc_zip_space(FILE* src,char* dst,rsize_t len){
    int ch = fgetc(src);
    if(isspace(ch) != 0){
        int _ch_ = ch;

        if(is_quote_paired(dst,len)){
            for(;isspace(_ch_) != 0 && _ch_ != '\n';_ch_ = fgetc(src));
        }else{
            for(;isspace(_ch_) != 0;_ch_ = fgetc(src));
        }

        if(_ch_ == '\n'){
            return '\n';
        }else if(
                (strchr("}])",_ch_) != nullptr)
            ||  (len == 0)
            ||  (strchr("([{", *(dst + len - 1) ) != nullptr)
        ){
            return _ch_;
        }else{
            ungetc(_ch_,src);
            return ' ';
        }

    }else{
        return ch;
    }
}
char* read_line(FILE* src){
    rsize_t buffer_size = 256;
    rsize_t str_size = 1;
    rsize_t total_read = 0;
    char* str = nullptr;
    char* tstr;
    while(true){
        //缓冲区扩容
        str_size = str_size + buffer_size;
        tstr = realloc(str,str_size * sizeof(char));
        if(tstr == nullptr){
            free(str);
            return nullptr;
        }else{
            str = tstr;
            *(str + (str_size - 1)) = '\0';
        }

        
        for( ; total_read < (str_size - 1); total_read++){
           *(str + total_read) = getc_zip_space(src,str,total_read);
            switch( *(str + total_read)){
                case '\n':
                case EOF:
                    *(str + total_read) = '\0';
                    return str;
            }
        }
    }
}
bool str_eq(char* l,char* r){
    return (l == nullptr && r == nullptr)
          || ( l != nullptr && r != nullptr && strcmp(l,r) == 0 );
}
bool str_slice_eq(char* val,char* begin,char* end){
    if(val == nullptr || begin == nullptr || end == nullptr || begin > end){
        return false;
    }else{
        char* p = val;
        char* q = begin;
        while(q <= end){
            if(*p != *q){
                return false;
            }else{
                p++;
                q++;
            }
        }
        return (*p == '\0');
    }
}

bool is_quote(char* expr_begin,char* expr_end){
        //表达式是否被括号包裹
        return  (*expr_begin == '(' && *expr_end == ')')
            ||  (*expr_begin == '[' && *expr_end == ']')
            ||  (*expr_begin == '{' && *expr_end == '}');
}
char* skip(char* expr_begin,char* expr_end){
    //跳过括号
    char s;
    switch (*expr_begin){
        case '(':
            s = ')';
            break;
        case '[':
            s = ']';
            break;
        case '{':
            s = '}';
            break;
        default:
            return expr_begin;
    }
    for(char* c = expr_begin + 1; c != (expr_end + 1); c++){
        if(*c == s){
            return c;
        }else if(*c == ')' || *c == ']' || *c == '}'){
            return  expr_end; //括号未匹配
        }else{
            c = skip(c,expr_end);
        }
    }
    return expr_end; //括号未匹配
}
char* rskip(char* expr_end,char* expr_begin){
    //跳过括号(反向)
    char s;
    switch(*expr_end){
        case ')':
            s = '(';
            break;
        case ']':
            s = '[';
            break;
        case '}':
            s = '{';
            break;
        default:
            return expr_end;
    }
    for(char* c = expr_end - 1; c != (expr_begin - 1);c--){
        if(*c == s){
            return c;
        }else if(*c == '(' || *c == '[' || *c == '{'){
            return expr_begin; //括号未匹配
        }else{
            c = rskip(c,expr_begin);
        }
    }
    return expr_begin; //括号未匹配
}

char* op_to(char* from){
    //return nullptr if from is not op
    if(*from == ' '){
        return strchr(from + 1,' ');
    }else{
        return nullptr;
    }
}
#define is_op(from) (op_to(from) != nullptr)
bool op_match(char* from,char** ops){
    //op should wrap by space, like 1 + 2
    //ops: the last element should be nullptr
    char* op;
    char* to = op_to(from);
    if(to != nullptr){
        char* op_begin = from + 1;
        char* op_end = to - 1;
        for(char** op = ops;*op != nullptr;op++){
            if(str_slice_eq(*op,op_begin,op_end)){
                return true;
            }
        }
        return false;
    }else{
        return false;
    }
}
char* op_search(char* expr_begin,char* expr_end){
    #define ALT_OP_LEVEL (-1)
    #define COND_OP_LEVEL (0)
    #define LOGIC_OP_LEVEL (1)
    #define CMP_OP_LEVEL (2)
    #define NUM_1_OP_LEVEL (3)
    #define NUM_2_OP_LEVEL (4)
    #define NUM_3_OP_LEVEL (5)
    #define NO_FOUND_OP_LEVEL (6)
    char* op = expr_end; //默认找不到运算符
    char* vop = nullptr; //广义运算符(总之它不是已知的就对了)
    int op_level = NO_FOUND_OP_LEVEL;
    #define update_op(level) \
        if(level <= op_level){\
            op = e;\
            op_level = level;\
        }

    for(
        char* e = expr_begin;
        e != (expr_end + 1);
        e = skip(e,expr_end) + 1
    ){
        if(op_match(e, (char*[]){"|",nullptr} )){
            update_op(ALT_OP_LEVEL)
        }else if(op_match(e, (char*[]){"=>",nullptr} )){
            update_op(COND_OP_LEVEL)
        }else if(op_match(e, (char*[]){"/\\","\\/",nullptr} )){
            update_op(LOGIC_OP_LEVEL)
        }else if(op_match(e, (char*[]){"<","<=",">",">=","=","/=",nullptr} )){
            update_op(CMP_OP_LEVEL)
        }else if(op_match(e, (char*[]){"+","-",nullptr} )){
            update_op(NUM_1_OP_LEVEL)
        }else if(op_match(e, (char*[]){"*","/",nullptr} )){
            update_op(NUM_2_OP_LEVEL)
        }else if(op_match(e, (char*[]){"^",nullptr} )){
            update_op(NUM_3_OP_LEVEL)
        }else if(is_op(e)){
            vop = e; //广义运算符
        }else{
            continue;
        }
    }
    if(op != expr_end){
        return op; //正常找到的op
    }else if(vop != nullptr){
        return vop; //广义运算符
    }else{
        return op; //未找到op, 实际上就是 expr_end
    }
}

named_func* find(char* name){
    lookup* p = &table;
    while(p -> next != nullptr){
        if(str_eq(p -> nf.name,name)){
            return &(p -> nf);
        }else{
            p = p -> next;
        }
    }
    if(str_eq(p -> nf.name,name)){
        return &(p -> nf);
    }else{
        if(p -> nf.name != nullptr){
            p -> next = (lookup*) calloc(1,sizeof(lookup));
            p = p -> next;
        }
        p -> nf.name = name;
        p -> nf.f = func_empty();
        p -> next = nullptr;
        return &(p -> nf);
    }
}
char* search_param(char v,char* v_start,rsize_t v_count){
    //定位形参, 以便做实参绑定
    char* v_end = v_start + v_count;
    for(char* p = v_start; p != v_end; p++){
        if(*p == v){
            return p;
        }
    }
    return nullptr;
}


value expr_calc(named_func* (*search_func)(char*),char* expr_begin,char* expr_end,rsize_t v_count, char* v, value* vals);
value entire_expr_calc(named_func* (*search_func)(char*),char* expr,rsize_t v_count, char* v, value* vals){
    char* expr_begin = expr;
    char* expr_end = expr + strlen(expr) - 1;
    return expr_calc(
        search_func,
        expr_begin,
        expr_end,
        v_count,
        v,
        vals
    );
}
value func_call_direct(func f,named_func* (*search_func)(char*),value* args,rsize_t args_count){
    switch(f.tp){
        case USER_DEF_FUNC:
        if(args_count == f.v_count){
            char* begin = (char*)f.e;
            char* end = begin + strlen(begin) - 1;
            return expr_calc(search_func,begin,end,f.v_count,f.v,args);
        }else{
            return err_value("func call with incorrect counts");
        }
        case INIT_FUNC:
        if(args_count == f.v_count){
            switch(f.v_count){
                case 0:
                return num_value(
                    ((num_t (*)())f.e)()
                );
                case 1:
                value x = *(args + 0);
                if(x.tp == NUM_TYPE){
                    num_t xn = x.val.n;
                    return num_value(
                        ((num_t (*)(num_t))f.e)(xn)
                    );
                }else{
                    return err_value("type error");
                }
                case 2:
                value l = *(args + 0);
                value r = *(args + 1);
                if(l.tp == NUM_TYPE && r.tp == NUM_TYPE){
                    num_t ln = l.val.n;
                    num_t rn = r.val.n;
                    return num_value(
                        ((num_t (*)(num_t,num_t))f.e)(ln,rn)
                    );
                }else{
                    return err_value("type error");
                }
                default:
                    return err_value("undeclare init func");
            }
        }else{
            return err_value("func call with incorrect counts");
        }
        case EMPTY_FUNC:
            return err_value("call empty func");
        default:
            return err_value("undeclare func");
    }

}
value func_call(value nf,named_func* (*search_func)(char*),value* args,rsize_t args_count){
    if(nf.tp == FUNC_TYPE){
        func f = nf.val.f -> f;
        return func_call_direct(f,search_func,args,args_count);
    }else{
        return err_value("illegal func call");
    }
}
value closure_calc(closure c){
    return expr_calc(
        c.search_func,
        c.expr_begin,
        c.expr_end,
        c.v_count,
        c.v,
        c.vals
    );
}
value op_calc(closure cl,char* op,closure cr){
    value l = closure_calc(cl);
    if(l.tp == BOOL_TYPE && str_eq(op,"=>")){
        // true => a   -> a
        // false => a  -> void
        if(l.val.b){
            return closure_calc(cr);
        }else{
            return void_value();
        }
        
    }else if(str_eq(op,"|")){
        // void | a   -> a
        // x | a      -> x
        if(l.tp == VOID_TYPE){
            return closure_calc(cr);
        }else{
            return l;
        }
    }else{
        value r = closure_calc(cr);
        if(l.tp == BOOL_TYPE && r.tp == BOOL_TYPE){
            bool lb = l.val.b;
            bool rb = r.val.b;
            //logic
            //!!! 逻辑运算没有短路
            if(str_eq(op,"/\\")){
                return bool_value(lb && rb);
            }else if(str_eq(op,"\\/")){
                return bool_value(lb || rb);
            }//unsupport
            else{
                return err_value("unsupport operator");
            }
        }else if(l.tp == NUM_TYPE && r.tp == NUM_TYPE){
            num_t ln = l.val.n;
            num_t rn = r.val.n;
            //numeric
            if(str_eq(op,"+")){
                return num_value(ln + rn);
            }else if(str_eq(op,"-")){
                return num_value(ln - rn);
            }else if(str_eq(op,"*")){
                return num_value(ln * rn);
            }else if(str_eq(op,"/")){
                return num_value(ln / rn);
            }else if(str_eq(op,"^")){
                return num_value(pow(ln,rn));
            }//compare
            #define complex_cmp(op) \
            if(cimag(ln) == 0 && cimag(rn) == 0){\
                return bool_value(creal(ln) op creal(rn));\
            }else{\
                return err_value("complex numbers are not comparable in size");\
            }
            else if(str_eq(op,"=")){
                return bool_value(ln == rn);
            }else if(str_eq(op,"/=")){
                return bool_value(ln != rn);
            }else if(str_eq(op,"<")){
                complex_cmp(<)
            }else if(str_eq(op,"<=")){
                complex_cmp(<=)
            }else if(str_eq(op,">")){
                complex_cmp(>)
            }else if(str_eq(op,">=")){
                complex_cmp(>=)
            }
            #undef complex_cmp
            //unsupport
            else{
                return err_value("unsupport operator");
            }
        }else{
            return err_value("type error");
        }
    }

}
value single_op_calc(char op,closure vc){
    value val = closure_calc(vc);
    switch(op){
        case '^'://logic not
            if(val.tp == BOOL_TYPE){
                return bool_value(!val.val.b);
            }else{
                return err_value("type error");
            }
        case '+':
            if(val.tp == NUM_TYPE){
                return num_value(+ val.val.n);
            }else{
                return err_value("type error");
            }
        case '-':
            if(val.tp == NUM_TYPE){
                return num_value(- val.val.n);
            }else{
                return err_value("type error");
            }
    }
}
arg arg_calc(named_func* (*search_func)(char*),char* expr_begin,char* expr_end,rsize_t v_count, char* v, value* vals){
    if(is_quote(expr_begin,expr_end)){
        char* arg_begin = expr_begin + 1;
        char* arg_end = expr_end; //遍历到 ')'
        char* q = arg_begin;
        arg args;
        args.x_count = 0;
        for(char* p = arg_begin; p != (arg_end + 1); p = skip(p,arg_end) + 1){
            if(*p == ',' || p == arg_end){
                if(p == q){
                    if(p == arg_end && args.x_count == 0){
                        //0个参数
                        return args;
                    }else{
                        return empty_arg();
                    }
                }else if(args.x_count < 26){
                    args.x[args.x_count] = expr_calc(search_func,q,p - 1,v_count,v,vals);
                    args.x_count = args.x_count + 1;
                    q = p + 1;
                }else{
                    return too_many_arg();
                }
            }
        }
        return args;
    }else{
        return  ill_arg();
    }
}
value func_calc(named_func* (*search_func)(char*),char* expr_begin,char* expr_end,rsize_t v_count, char* v, value* vals){
    //函数调用与函数引用

    switch(*expr_end){
        case ')':
        case ']':
        case '}':
            //函数调用
            char* func_begin = expr_begin;
            char* quote_begin = rskip(expr_end,expr_begin);
            char* func_end = quote_begin - 1;
            char* quote_end = expr_end;
            value func = expr_calc(search_func,func_begin,func_end,v_count,v,vals);
            arg args = arg_calc(search_func,quote_begin,quote_end,v_count,v,vals);
            switch(args.x_count){
                case TOO_MANY_ARG:
                    return err_value("func call with more than 26 arguments");
                case ILL_ARG:
                    return err_value("func call with illegal arguments");
                case EMPTY_ARG:
                    return err_value("func call with empty arguments");
                default:
                    return func_call(func,search_func,args.x,args.x_count);
            }
        default:
            //全局函数引用
            rsize_t len = expr_end - expr_begin + 1;
            char* name = calloc(len + 1,sizeof(char));
            char* p = name;
            for(char* c = expr_begin; c != (expr_end + 1); c++){
                if('a' <= *c && *c <= 'z'){
                    *p = *c;
                     p++;
                }else{
                    free(name);
                    return err_value("illegal variable");
                }
            }
            *p = '\0';
            named_func nf = search_func(name);
            value val = func_value(nf);
            auto_free_name(nf,name);
            return val;
    }

}
value expr_calc(named_func* (*search_func)(char*),char* expr_begin,char* expr_end,rsize_t v_count, char* v, value* vals){

    if(
            (expr_begin == nullptr)
        ||  (expr_end == nullptr)
        ||  (expr_begin > expr_end)
        ||  (v == nullptr)
        || (vals == nullptr)
    ){
        //守卫条件
        return err_value("illegal expr");
    }
    //------------------
    char* op_from = op_search(expr_begin,expr_end);
    if(op_from == expr_end){
        if(     is_quote(expr_begin,expr_end)
            &&  (skip(expr_begin,expr_end + 1) == expr_end)
        ){
            return expr_calc(search_func,expr_begin + 1, expr_end - 1,v_count,v,vals);
        }else{
            rsize_t len = expr_end - expr_begin + 1;
            switch(len){
                case 1:
                    switch(*expr_begin){
                        case 'I':
                            return num_value(I);
                        case 'E':
                            return num_value(M_E);
                        default:
                            char* p = search_param(*expr_begin,v,v_count);
                            if(p != nullptr){
                                return *(vals + (p - v));
                            }
                    }
                    /*fall though*/
                case 2:
                case 3:
                char* const_begin = expr_begin;
                char* const_end = expr_end;
                if(str_slice_eq("PI",const_begin,const_end)){
                    return num_value(M_PI);
                }else if(str_slice_eq("INF",const_begin,const_end)){
                    return num_value(INFINITY);
                }else if(str_slice_eq("NAN",const_begin,const_end)){
                    return num_value(NAN);
                }else{
                    /*fall though*/
                }
                default:
                    if(
                            (*expr_begin == '^')
                        ||  (*expr_begin == '+')
                        ||  (*expr_begin == '-')
                    ){
                        //单目运算
                        return single_op_calc(
                            *expr_begin,
                            closure_of(search_func,expr_begin + 1,expr_end,v_count,v,vals)
                        );
                    }else if(
                            ('a' <= *expr_begin && *expr_begin <= 'z')
                        ||  (*expr_end == ')' || *expr_end == ']' || *expr_end == '}')
                    ){
                        //函数调用与函数引用
                        return func_calc(search_func,expr_begin,expr_end,v_count,v,vals);
                    }else{
                        char * num_begin = expr_begin;
                        char * num_end = expr_end;
                        char * p;
                        value ans = num_value(strtold(num_begin,&p));

                        for(char* q = num_begin; q <= num_end; q++){
                            //数值不允许包含 A - Z
                            if('A' <= *q && *q <= 'Z'){
                                p = nullptr;
                                break;
                            }
                        }
                        if(p == num_end + 1){
                            return ans;
                        }else{
                            return err_value("illegal expr");
                        }
                    }
            }
        }
    }else{
        char* op_begin = op_from + 1;
        char* op_to = strchr(op_begin,' ');
        char* op_end = op_to - 1;
        // the struct like: [op_from =' '][op_begin]...[op_end][op_to = ' ']
        // where op is: [op_begin]...[op_end]
        char* op = calloc(op_end - op_begin + 2,sizeof(char));
        char* q = op;
        for(char* p = op_begin; p <= op_end;p++){
            *q = *p;
            q++;
        }
        *q = '\0';
        closure cl = closure_of(
            search_func,
            expr_begin,
            op_from - 1,
            v_count,
            v,
            vals
        );
        closure cr = closure_of(
            search_func,
            op_to + 1,
            expr_end,
            v_count,
            v,
            vals
        );
        value ans = op_calc(cl,op,cr);
        free(op);
        return ans;
    }
        
    
}
value calc(char* expr,named_func* (*search_func)(char*)){
    value val = try_define_func(expr,search_func);

    switch (val.tp){
        case BOOL_TYPE:
            if(val.val.b){
                return statement_value();
            }else{
                return entire_expr_calc(
                    search_func,
                    expr,
                    0,
                    "",
                    (value[]){}
                );
            }
        case ERR_TYPE:
            return val;
        default:
            return err_value("undeclare value");
    }



    
}

void print_help(){
    printf("\e[1;2;35m");
    puts("Welcome to use calc 2.0, a calc that you could define a function");
    puts("() {} [] are equals");
    puts("binary operator are left associative operator and have the following precedence(higher up):");
    puts("\t^(pow)");
    puts("\t* /");
    puts("\t+ -");
    puts("\t< <= > >= = /=(no equals)");
    puts("\t\\/(or) /\\(and)");
    puts("\t=>(see below)");
    puts("\t|(see below)");
    puts("usage: 1 + 2 (note: binary operator should wrap by space)");
    puts("L => R when L is bool true return R's value, or L is bool false return void(a special value)");
    puts("L | R when L is void, return R's value or else return L's value");
    puts("single operator are right associative operator:");
    puts("\t ^(not) + -");
    puts("usage: -1 (note: single operator should following expr immediately, without space)");
    puts("number will be parsed by strtold(C function) but you can not use A-Z in number, like 0xAZ is not allowed, use 0xaz instead");
    puts("special number:");
    puts("\tI(imaginary unit)");
    puts("\tNAN(not a number,special long double value)");
    puts("\tINF(infinity,special long double value)");
    puts("\tPI");
    puts("\tE");
    puts("define function:");
    puts("\tfoo(x,y)=expr");
    puts("\tfoo is function reference name which consist by a-z");
    puts("\tx and y are params(note: 0-26 params are supported, but the params should by single alpha a-z)");
    puts("\tcall foo:");
    puts("\t\tfoo(0,1)");
    puts("\tyou can pass foo into param x, so that:");
    puts("\t\tx(2,3)");
    puts("\tis equal to");
    puts("\t\tfoo(2,3)");
    puts("\tfoo(x,y)=expr");
    puts("\t        ^this = has not space around");
    puts("\tfunction define is a statement,so you can't define in expr");
    puts("\tyou can redefine any function in any time,include init function.");
    puts("\thowever,redefine is visible for all function:");
    puts("\t\t#1 foo(x)=x");
    puts("\t\t#2 goo(x)=foo(x)");
    puts("\t\t#3 foo(x)=x + 1");
    puts("\t\tcall #2 goo will call #3 foo, not #1 foo");
    puts("function reference name and params are variable");
    puts("\tfor variable v, v is function reference name iff v is not param");
    printf("\e[0m\n");
}
int main(){
    print_help();
    set_all_init_func(find);
    char* expr;
    while(true){
        printf("\e[1;2;32m>>>\e[0m\e[1;34m");
        expr = read_line(stdin);
        printf("\e[0m");
        print_value(
            calc(expr,find)
        );
        free(expr);
    }
    
}
