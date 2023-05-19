//
//  List.c
//  server
//
//  Created by Gaetan on 2022-09-15.
//

#include "List.h"

List *initialisation() {

    List *list = malloc(sizeof(*list));
    Element *element = malloc(sizeof(*element));

    if (list == NULL || element == NULL) {
        exit(EXIT_FAILURE);
    }

    element->client = NULL;
    element->next = NULL;
    element->previous = NULL;
    list->first = element;
    list->last = element;
    list->size = 0;
    list->append = add;
    list->get = get_;
    list->set = set_;
    list->remove = removeElement;
    return list;
}

int set_(List *list, int index, const Client *client) {
    if (index < 0 || index >= list->size) {
        return -1;
    }
    int i = 0;
    Element *element = list->first;
    while (i < index) {
        element = element->next;
        i++;
    }
    element->client = malloc(sizeof(*element->client));
    if (element->client == NULL) {
        exit(EXIT_FAILURE);
    }
    *(element->client) = *client;
    return 0;
}

test add(List *list, const Client *client) {
    Element *last = list->last;
    if (last->client == NULL) {
        last->client = malloc(sizeof(*client));
        if (last->client == NULL) {
            exit(EXIT_FAILURE);
        }
        *(last->client) = *client;
        list->size++;
    } else {
        Element *element = malloc(sizeof(*element));
        if (element == NULL) {
            exit(EXIT_FAILURE);
        }
        element->client = malloc(sizeof(*client));
        if (element->client == NULL) {
            exit(EXIT_FAILURE);
        }
        *(element->client) = *client;
        element->next = NULL;
        last->next = element;
        element->previous = last;
        list->size++;
        list->last = element;
    }
    return 0;
}

Client *get_(List *list, int index) {
    if (index >= list->size || index < -list->size)
        return NULL;
    int i;
    Element *element;
    if (index < 0) {
        element = list->last;
        i = -1;
        while (i > index) {
            if (element->previous == NULL) {
                return NULL;
            }
            element = element->previous;
            i--;
        }
        return element->client;
    }
    element = list->first;
    i = 0;
    while (i < index) {
        element = element->next;
        i++;
    }
    return element->client;
}

int del(List *list) {
    Element *element = list->first;
    Element *next;
    while (element != NULL) {
        next = element->next;
        if (element->client != NULL) {
            if (element->client->hash != NULL) {
                free(element->client->hash);
            }
            free(element->client);
        }
        free(element);
        element = next;
    }
    free(list);
    return 0;
}

Client *removeElement(List *list, int index) {
    if (index >= list->size || index < -list->size)
        return NULL;
    int i;
    Element *element;
    if (index < 0) {
        element = list->last;
        i = -1;
        while (i > index) {
            element = element->previous;
            i--;
        }
    } else {
        element = list->first;
        i = 0;
        while (i < index) {
            element = element->next;
            i++;
        }
    }
    Client *client = element->client;
    if (element->previous != NULL) {
        element->previous->next = element->next;
    } else {
        list->first = element->next;
    }
    if (element->next != NULL) {
        element->next->previous = element->previous;
    } else {
        list->last = element->previous;
    }
    return client;
}


Client *findElement(List *list, char *hash) {
    int i = 0;
    Element *element = list->first;
    while (i < list->size) {
        if (element->client != NULL) {
            if (strcmp(element->client->hash, hash) == 0) {
                return element->client;
            }
        }
        if (element->next == NULL) {
            return NULL;
        }
        element = element->next;
        i++;
    }
    return NULL;
}