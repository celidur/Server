//
//  List.h
//  server
//
//  Created by Gaetan on 2022-09-15.
//
#include <stdlib.h>
#include <string.h>

#ifndef List_h
#define List_h

typedef struct List List, *test;
typedef struct Element Element;
typedef struct Client Client;

struct List {
    int size;
    Element *first;
    Element *last;

    test (*append)(List *, const Client *);

    int (*set)(List *, int, const Client *);

    Client *(*get)(List *, int);

    Client *(*remove)(List *, int);

    Client *(*find)(List *, char *);
};

struct Element {
    Client *client;
    Element *next;
    Element *previous;
};
struct Client {
    char *hash;
    int last_activity;

};

List *initialisation();

static int set_(List *list, int index, const Client *client);

static test add(List *list, const Client *client);

static Client *get_(List *list, int index);

int del(List *list);

static Client *removeElement(List *list, int index);

static Client *findElement(List *, char *);

#endif /* List_h */
