// NOTE(ezexff): Floating point fix
extern "C"
{
    int _fltused;
}

#define DllExport __declspec(dllexport)

extern "C" int DllExport Square(int Value)
{
    int Result = Value * Value;
    
    return (Result);
}


extern "C" int DllExport Int32DigitsCount(int Value)
{
    if(Value == 0)
    {
        return (1);
    }
    
    int Count = 0;
    while(Value != 0)
    {
        Value /= 10;
        Count++;
    }
    
    return (Count);
}

extern "C" unsigned char DllExport *Int32ToCharStr(unsigned char *Memory, 
                                                   int Value, 
                                                   unsigned int ValueDigitsNumber)
{
    if(Value > 0)
    {
        int Base = 10;
        
        int Index = 0;
        int Offset = 0;
        while (Value != 0) 
        {
            int Remainder = Value % Base;
            Index = ValueDigitsNumber - 1 - Offset++;
            Memory[Index] = (char)('0' + Remainder);
            Value /= Base;
        }
    } 
    else if(Value == 0 && ValueDigitsNumber == 1)
    {
        Memory[0] = '0';
    } 
    else
    {
        // Process negative value
        // Memory[Index] = (char)((Remainder > 9) ? (Remainder - 10) + 'a' : Remainder + '0');
    }
    Memory[ValueDigitsNumber] = '\n';
    
    
    return (Memory);
}