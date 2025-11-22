// markov.c
// Simple word-level order-1 Markov chain generator in C
// Compile: gcc -O2 -o markov markov.c
// Usage: ./markov [input.txt] [num_words]
// If no input file given, reads from stdin.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Next {
    char *word;
    int count;
    struct Next *next;
} Next;

typedef struct State {
    char *word;
    Next *nexts;
    struct State *next; // for chaining in the state list
} State;

State *states = NULL;

char *strdup_s(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *d = malloc(n);
    if (!d) { perror("malloc"); exit(1); }
    memcpy(d, s, n);
    return d;
}

State *find_state(const char *word) {
    State *s = states;
    while (s) {
        if (strcmp(s->word, word) == 0) return s;
        s = s->next;
    }
    return NULL;
}

State *ensure_state(const char *word) {
    State *s = find_state(word);
    if (s) return s;
    s = malloc(sizeof(State));
    if (!s) { perror("malloc"); exit(1); }
    s->word = strdup_s(word);
    s->nexts = NULL;
    s->next = states;
    states = s;
    return s;
}

void add_transition(const char *from, const char *to) {
    State *s = ensure_state(from);
    Next *n = s->nexts;
    while (n) {
        if (strcmp(n->word, to) == 0) {
            n->count++;
            return;
        }
        n = n->next;
    }
    // not found -> create
    Next *nn = malloc(sizeof(Next));
    if (!nn) { perror("malloc"); exit(1); }
    nn->word = strdup_s(to);
    nn->count = 1;
    nn->next = s->nexts;
    s->nexts = nn;
}

char *choose_next(State *s) {
    if (!s || !s->nexts) return NULL;
    // compute total counts
    int total = 0;
    Next *n = s->nexts;
    while (n) { total += n->count; n = n->next; }
    int r = rand() % total;
    n = s->nexts;
    while (n) {
        if (r < n->count) return n->word;
        r -= n->count;
        n = n->next;
    }
    // fallback
    return s->nexts->word;
}

void build_markov_from_file(FILE *f) {
    char buf[4096];
    char *prev = NULL;
    while (fscanf(f, "%4095s", buf) == 1) {
        char *word = buf; // note: buf reused, so strdup when storing
        if (prev) {
            add_transition(prev, word);
            free(prev);
            prev = NULL;
        }
        // store current as prev for next iteration
        prev = strdup_s(word);
    }
    if (prev) free(prev);
}

void free_markov() {
    State *s = states;
    while (s) {
        State *sn = s->next;
        free(s->word);
        Next *n = s->nexts;
        while (n) {
            Next *nn = n->next;
            free(n->word);
            free(n);
            n = nn;
        }
        free(s);
        s = sn;
    }
    states = NULL;
}

int count_states() {
    int c = 0;
    State *s = states;
    while (s) { c++; s = s->next; }
    return c;
}

State *random_state() {
    int nstates = count_states();
    if (nstates == 0) return NULL;
    int r = rand() % nstates;
    State *s = states;
    while (r > 0 && s) { s = s->next; r--; }
    return s;
}

int main(int argc, char **argv) {
    FILE *in = stdin;
    int num_words = 50;

    if (argc >= 2) {
        in = fopen(argv[1], "r");
        if (!in) { perror("fopen"); return 1; }
    }
    if (argc >= 3) {
        num_words = atoi(argv[2]);
        if (num_words <= 0) num_words = 50;
    }

    srand((unsigned)time(NULL));

    build_markov_from_file(in);

    if (in != stdin) fclose(in);

    if (!states) {
        fprintf(stderr, "No words read. Exiting.\n");
        return 1;
    }

    // start from a random state
    State *cur = random_state();
    if (!cur) { fprintf(stderr, "Couldn't pick start state.\n"); free_markov(); return 1; }

    // print generated words
    for (int i = 0; i < num_words; ++i) {
        printf("%s", cur->word);
        if (i < num_words - 1) printf(" ");
        char *nextw = choose_next(cur);
        if (!nextw) {
            // dead end: pick a random state to continue
            cur = random_state();
        } else {
            State *ns = find_state(nextw);
            if (ns) cur = ns;
            else cur = random_state();
        }
    }
    printf("\n");

    free_markov();
    return 0;
}
