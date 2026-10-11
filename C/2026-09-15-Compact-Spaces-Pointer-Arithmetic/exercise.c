void compact_spaces(char *s)
{
    char *read = s;
    char *write = s;

    while (*read == ' ')
    {
        read++;
    }

    while (*read != '\0')
    {
        if (*read == ' ')
        {
            while (*read == ' ')
            {
                read++;
            }

            if (*read != '\0')
            {
                *write = ' ';
                write++;
            }
        }
        else
        {
            *write = *read;
            write++;
            read++;
        }
    }

    *write = '\0';
}
