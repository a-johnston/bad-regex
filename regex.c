#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char *str;
    int valid;
} state;

struct charlist_s {
    char c;
    struct charlist_s *next;
};

typedef struct charlist_s charlist;

typedef struct {
    int min;
    int max;
} quant;

int consume(charlist *class, state *s) {
    if (class->c == *(s->str)) {
        (s->str)++;
        return 1;
    }
    if (class->next != NULL) {
        return consume(class->next, s);
    }
    return 0;
}

void consume_q(charlist *class, quant *q, state *s) {
    int t = 0;
    while (t < q->max && consume(class, s) == 1) {
        t++;
    }
    if (t < q->min) {
        s->valid = 0;
    }
}

charlist* append_to_class(char c, charlist *head) {
    charlist* new = (charlist*) malloc(sizeof(charlist));
    new->c = c;
    new->next = head;
    return new;
}

charlist build_class(char **s) {
    if (**s == '[') {
        int esc = 0;
        (*s)++;
        charlist *cons = append_to_class(**s, NULL);
        (*s)++;
        while (**s != ']' || esc == 1) {
            if (**s == '\\' && esc == 0) {
                esc = 1;
            } else {
                esc = 0;
                cons = append_to_class(**s, cons);
            }
            (*s)++;
        }
        (*s)++;
        return *cons;
    }
    charlist c = (charlist) { **s, NULL };
    (*s)++;
    return c;
}

quant build_quant(char **str) {
    quant q = (quant) { 1, 1 };
    if (**str == '*') {
        q = (quant) { 0, 1000000000 };
    } else if (**str == '+') {
        q = (quant) { 1, 1000000000 };
    } else if (**str == '{') {
        quant q = (quant) { 0, 0 };
        sscanf(*str, "{%d,%d}%*s", &(q.min), &(q.max));
        while (**str != '}') (*str)++;
        (*str)++;
        return q;
    }
    (*str)++;
    return q;
}

state parse(char *re, state s) {
    while (*re != '\0' && s.valid == 1) {
        charlist class = build_class(&re);
        quant q = build_quant(&re);
        consume_q(&class, &q, &s);
    }
    return s;
}

int main(int argc, char **argv) {
    return *parse(argv[1], (state) { argv[2], 1 }).str != '\0';
}
