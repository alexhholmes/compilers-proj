int do_addition(int a, int b)
{
    if (a == 0)
        return b;
    if (b == 0)
        return a;

    return (a + b);
}

int main(void)
{
    int a = 0;
    int b = 0;
    int c = 0;

    printf("Hello world! %d %d", a, b);
    
    return 0; 
}
