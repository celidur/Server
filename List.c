//
//  List.c
//  server
//
//  Created by Gaetan on 2022-09-15.
//

#include "List.h"

char *get_buffer(unsigned long long int size) {
    char *buffer = malloc(size);
    if (buffer == NULL) {
        exit(EXIT_FAILURE);
    }
    return buffer;
}

char *split(char *str, char delim) {
    int i;
    char *token;
    int end = 0;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == delim) {
            end = i;
            i++;
            break;
        }
    }
    token = get_buffer(strlen(str) - i);
    for (int j = i; j < strlen(str); ++j) {
        token[j - i] = str[j];
    }
    str[end] = '\0';
    return token;
}


char *IntToString(unsigned int number) {
    char *buffer = get_buffer(10 * sizeof(char));
    char digit[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int i = 0;
    do {
        buffer[i] = digit[number % 10];
        number /= 10;
        i++;
    } while (number > 0);
    buffer[i] = '\0';
    for (int j = 0; j < i; ++j) {
        char temp = buffer[j];
        if (i - j - 1 < j)
            break;
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
    }
    return buffer;
}


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