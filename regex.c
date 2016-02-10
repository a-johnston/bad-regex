#include <stdlib.h>
#include <stdio.h>
#define MAX 10000000

typedef struct { char *str; int valid; } state;
typedef struct { char c; void *next; } charlist;
typedef struct { int min; int max; } quant;

int consume(charlist *class, state *s) {
    return (class->c == *(s->str) 
        || (class->next == NULL ? 0 : consume(class->next, s)));
}

void consume_q(charlist *class, quant *q, state *s) {
    while (q->max-->0 && q->min--<MAX && consume(class, s) == 1) (s->str)++;
    if (q->min >= 0) s->valid = 0;
}

charlist build_class(char **s, charlist *cons) {
    if (**s != '[') return (charlist) { *((*s)++), NULL };
    while (*(++(*s)) != ']' || *((*s)++) != ']') {
        if (**s == '\\') (*s)++;
        charlist *temp = malloc(sizeof(charlist));
        temp->c = **s;
        temp->next = cons;
        cons = temp;
    }
    return *cons;
}

quant build_quant(char **str, quant q) {
    if (**str == '+') {
        q.min = 1;
    } else if (**str == '{') {
        sscanf(*str, "{%d,%d}", &(q.min), &(q.max));
        while (**str != '}') (*str)++;
    } else if (**str != '*') {
        return (quant) { 1, 1 };
    }
    (*str)++;
    return q;
}

int run(char *re, state s) {
    while (*re != '\0' && s.valid == 1) {
        charlist class = build_class(&re, NULL);
        quant q = build_quant(&re, (quant) { 0, MAX });
        consume_q(&class, &q, &s);
    }
    return s.valid == 0 || *(s.str) != '\0';
}

int main(int argc, char **argv) {
    return run(argv[1], (state) { argv[2], 1 });
}
