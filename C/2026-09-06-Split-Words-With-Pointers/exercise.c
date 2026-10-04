#include <stdlib.h>

char **split_words(const char *input, int *out_count)
{
    // count the number of words in the string.
    int count = 0;
    const char *p = input;
    while (*p != '\0')
    {
        while (*p == ' ')
        {
            p++;
        }

        if (*p != ' ' && *p != '\0')
        {
            count++;
        }

        while (*p != ' ' && *p != '\0')
        {
            p++;
        }
    }

    if (count == 0)
    {
        *out_count = 0;
        return NULL;
    }

    // create array of char pointers
    char **string_array = (char **)malloc(count * (sizeof(char *)));
    if (string_array == NULL)
    {
        return NULL;
    }

    // stride through each character, get the length, allocate a memory malloc and fix each character
    int i = 0;
    while (*input != '\0' && i < count)
    {
        while (*input == ' ')
        {
            input++;
        }

        // if (*input == '\0')
        // {
        //     break;
        // }

        // get length of word
        const char *start = input;
        while (*input != ' ' && *input != '\0')
        {
            input++;
        }
        int length = input - start;
        char *string = (char *)malloc(length + 1);
        if (string == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(string_array[j]);
            }
            free(string_array);
            *out_count = 0;
            return NULL;
        }

        // fix all characters into the string and add null terminator to the end
        char *s = string;
        while (start < input)
        {
            *string = *start;
            string++;
            start++;
        }
        *string = '\0';

        // pass first address of the string to the address of pointer.
        string_array[i] = s;
        i++;
    }

    *out_count = count;
    return string_array;
}
