#pragma once

/* ===== Enums for data types ===== */
typedef enum
{
    SNEK_INT,
    SNEK_STRING,
} snek_type_t;

/* ===== Struct for System Object. This struct contains the object type, an integer ref counter and a union (as) that shuffles between an long integer value and string (str) ===== */
typedef struct snek_obj
{
    snek_type_t type;
    int refcount;
    union
    {
        long int_value;
        struct
        {
            char *data;
            int length;
        } str;
    } as;
} snek_obj_t;

/* ===== Function definitions from exercise.c ===== */
snek_obj_t *snek_new_int(long value);
snek_obj_t *snek_new_string(const char *value);
void snek_free(snek_obj_t *obj);

snek_obj_t *snek_add(snek_obj_t *left, snek_obj_t *right);
