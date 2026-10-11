#pragma once

typedef struct Song
{
    const char *title;
    struct Song *next;
} Song;

void playlist_push_front(Song **head, Song *node);
void playlist_push_back(Song **head, Song *node);
Song *playlist_pop_front(Song **head);
Song *playlist_find(Song *head, const char *title);
Song *playlist_extract(Song **head, const char *title);
void playlist_clear(Song **head);
