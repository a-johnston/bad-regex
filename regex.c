#include <stdlib.h>
#include <stdio.h>

typedef struct { char *str; int valid; } state;
typedef struct { char c; void *next; } charlist;
typedef struct { int min; int max; } quant;

int consume(charlist *class, state *s) {
    return (class->c == *(s->str) 
        || (class->next == NULL ? 0 : consume(class->next, s)));
}

void consume_q(charlist *class, quant *q, state *s) {
    int t = 0;
    while (t < q->max && consume(class, s) == 1) { t++; (s->str)++; }
    if (t < q->min) s->valid = 0;
}

charlist* append_to_class(char c, charlist *head, charlist *new) {
    new->c = c;
    new->next = head;
    return new;
}

charlist build_class(char **s, charlist *cons) {
    if (**s == '[') {
        while (*(++(*s)) != ']') {
            if (**s == '\\') (*s)++;
            cons = append_to_class(**s, cons, malloc(sizeof(charlist)));
        }
        (*s)++;
        return *cons;
    }
    charlist c = (charlist) { *((*s)++), NULL };
    return c;
}

quant build_quant(char **str, quant q) {
    if (**str == '*') {
        q = (quant) { 0, 1000000000 };
    } else if (**str == '+') {
        q.min = 1;
    } else if (**str == '{') {
        sscanf(*str, "{%d,%d}", &(q.min), &(q.max));
        while (**str != '}') (*str)++;
    } else {
        return (quant) { 1, 1 };
    }
    (*str)++;
    return q;
}

int run(char *re, state s) {
    while (*re != '\0' && s.valid == 1) {
        charlist class = build_class(&re, NULL);
        quant q = build_quant(&re, (quant) { 0, 1000000000 });
        consume_q(&class, &q, &s);
    }
    return s.valid == 0 || *(s.str) != '\0';
}

int main(int argc, char **argv) {
    return run(argv[1], (state) { argv[2], 1 });
}
