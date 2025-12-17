//Simplish parser tree arena

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define S(s)            (Str){s, sizeof(s)-1}
#define new(a, n, t)    (t *)alloc(a, n, sizeof(t), _Alignof(t))

typedef struct {
    char *beg;
    char *end;
} Arena;

char *alloc(Arena *a, ptrdiff_t count, ptrdiff_t size, ptrdiff_t align)
{
    ptrdiff_t pad = (ptrdiff_t)-(uintptr_t)a->beg & (align - 1);
    assert(count < (a->end - a->beg - pad)/size);  // TODO: handle OOM
    char *r = a->beg + pad;
    a->beg += pad + count*size;
    return memset(r, 0, (size_t)(count*size));
}

typedef struct {
    char     *data;
    ptrdiff_t len;
} Str;

bool equals(Str a, Str b)
{
    return a.len==b.len && !memcmp(a.data, b.data, (size_t)a.len);
}

// Take the first n bytes of the string.
Str take(Str s, ptrdiff_t n)
{
    assert(n>=0 && n<=s.len);
    s.len = n;
    return s;
}

// Drop the first n bytes of the string.
Str drop(Str s, ptrdiff_t n)
{
    assert(n>=0 && n<=s.len);
    s.data += n;
    s.len  -= n;
    return s;
}

_Bool whitespace(char c)
{
    return c=='\t' || c=='\n' || c=='\r' || c==' ';
}

_Bool wordchar(char c)
{
    return !whitespace(c) && c!='[' && c!=']';
}

Str skipspace(Str s)
{
    while (s.len && whitespace(s.data[0])) {
        s.data++;
        s.len--;
    }
    return s;
}

typedef struct {
    Str  token;
    Str  tail;
    bool ok;
} Lex;

Lex next_token(Str s)
{
    Lex r = {};

    s = skipspace(s);
    if (!s.len) {
        r.ok = true;
        return r;  // EOF
    }

    switch (s.data[0]) {
    case 0:
        return r;  // error (forbid nulls)
    case '[':
    case ']':
        r.token = take(s, 1);
        r.tail  = drop(s, 1);
        r.ok    = true;
        return r;
    }

    ptrdiff_t i = 0;
    for (; i<s.len && wordchar(s.data[i]); i++) {}
    r.token = take(s, i);
    r.tail  = drop(s, i);
    r.ok    = true;
    return r;
}

typedef enum { Type_WORD, Type_ARRAY } Type;

typedef struct Node Node;

typedef struct {
    Type type;
    union {
        Str   word;
        Node *array;
    };
} Value;

struct Node {
    Node  *next;
    Value *value;
};

typedef struct {
    Value *value;
    Str    tail;
    bool   ok;
} Result;

Result parse(Str, int, Arena *);

Result parse_array(Str s, int depth, Arena *a)
{
    Result r = {};

    Node  *head = 0;
    Node **tail = &head;
    for (;;) {
        Lex peek = next_token(s);
        if (!peek.ok || !peek.token.len) {
            return r;  // error
        } else if (equals(peek.token, S("]"))) {
            r.value = new(a, 1, Value);
            r.value->type  = Type_ARRAY;
            r.value->array = head;
            r.tail = peek.tail;
            r.ok = true;
            return r;
        }

        Result rv = parse(s, depth, a);
        if (!rv.ok) return r;
        Node *node = new(a, 1, Node);
        node->value = rv.value;
        *tail = node;
        tail = &node->next;
        s = rv.tail;
    }
}

Result parse(Str s, int depth, Arena *a)
{
    Result r   = {};
    Lex    lex = {.tail = s};

    if (depth < 0) return r;  // too much recursion

    for (;;) {
        lex = next_token(lex.tail);
        if (!lex.ok || !lex.token.len) {
            return r;  // error
        }

        if (equals(lex.token, S("]"))) {
            return r;  // error

        } else if (equals(lex.token, S("["))) {
            Result ra = parse_array(lex.tail, depth-1, a);
            if (!ra.ok) return r;
            r.value = ra.value;
            r.tail = ra.tail;
            r.ok = true;
            return r;

        } else {
            r.value = new(a, 1, Value);
            r.value->type = Type_WORD;
            r.value->word = lex.token;
            r.tail = lex.tail;
            r.ok = true;
            return r;
        }
    }
}

// Print tree with all arrays reversed.
static void print_reverse(Value *v, Arena scratch)
{
    switch (v->type) {
    case Type_WORD:
        printf("%.*s", (int)v->word.len, v->word.data);
        break;
    case Type_ARRAY:
        putchar('[');
        Node *head = 0;
        for (Node *n = v->array; n; n = n->next) {
            Node *rev = new(&scratch, 1, Node);
            rev->value = n->value;
            rev->next = head;
            head = rev;
        }
        for (Node *n = head; n; n = n->next) {
            if (n != head) {
                putchar(' ');
            }
            print_reverse(n->value, scratch);
        }
        putchar(']');
        break;
    }
}

int main()
{
    size_t cap = (size_t)1<<21;
    char  *mem = malloc(cap);

    Str input = S(
        "[a b [c d e [f]] g]\n"
        "[[[[x] y] [] []]]\n"
        "[[[[[[[[[[[[[[[[[[[[[[[[[[[deep nesting]]]]]]]]]]]]]]]]]]]]]]]]]]]\n"
    );

    for (Str s = input; s.len;) {
        enum { max_depth = 1024 };
        Arena scratch = {mem, mem+cap};
        Result r = parse(s, max_depth, &scratch);
        if (!r.ok) return 1;
        print_reverse(r.value, scratch);
        putchar('\n');
        s = r.tail;
    }
}
