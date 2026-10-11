#include <stdlib.h>
#include <string.h>

#include "exercise.h"

void playlist_push_front(Song **head, Song *node)
{

    if (node == NULL)
    {
        return;
    }

    if (head == NULL)
    {
        return;
    }

    node->next = *head;
    *head = node;
}

void playlist_push_back(Song **head, Song *node)
{
    if (head == NULL)
    {
        return;
    }

    if (node == NULL)
    {
        return;
    }

    if (*head == NULL)
    {
        *head = node;
        return;
    }

    Song *current = *head;
    while (current->next != NULL)
    {
        current = current->next;
    }

    current->next = node;
    node->next = NULL;
}

Song *playlist_pop_front(Song **head)
{
    if (head == NULL)
    {
        return NULL;
    }

    if (*head == NULL)
    {
        return NULL;
    }

    Song *first_node = *head;
    *head = (*head)->next;
    first_node->next = NULL;

    return first_node;
}

Song *playlist_find(Song *head, const char *title)
{
    if (head == NULL)
    {
        return NULL;
    }

    Song *current = head;
    while (current != NULL)
    {
        if (strcmp(current->title, title) == 0)
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

Song *playlist_extract(Song **head, const char *title)
{
    if (head == NULL)
    {
        return NULL;
    }

    if (title == NULL)
    {
        return NULL;
    }

    if (*head == NULL)
    {
        return NULL;
    }

    Song **current = head;
    while (*current != NULL)
    {
        if (strcmp((*current)->title, title) == 0)
        {
            Song *removed = *current;
            *current = removed->next;
            removed->next = NULL;
            return removed;
        }
        current = &((*current)->next);
    }
    return NULL;
}

void playlist_clear(Song **head)
{
    if (head == NULL)
    {
        return;
    }

    while (*head != NULL)
    {
        Song *next = (*head)->next;
        free(*head);
        *head = next;
    }
}
