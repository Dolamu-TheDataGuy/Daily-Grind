int filter_even(int *numbers, int length)
{
    int count = 0;

    for (int i = 0; i < length; i++)
    {
        if (numbers[i] % 2 == 0)
        {
            numbers[count] = numbers[i];
            count++;
        }
    }
    return count;
}

void squeeze_spaces(char *text)
{
    char *read = text;

    while (*read != '\0')
    {
        // This loop runs until all spaces are removed from the string and moves to the next line of code.
        while (*read == ' ')
        {
            read++;
        }
        if (*read != '\0')
        {
            *text = *read;
            text++;
            read++;
        }
    }
    *text = '\0';
}
