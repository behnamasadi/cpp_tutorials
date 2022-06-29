# Unions

The purpose of the union is to save memory by using the same portion of memory for accessing different data types. All three variables `ui`, `hi, lo`, and `uc[4]` 
are pointing to the exact location in the memory, changing one of them will be seen by others.

```cpp
union mix_type 
{
    unsigned int ui; // 4 bytes
    struct 
    {
        unsigned short hi; // 2 bytes
        unsigned short lo; // 2 bytes
    } s;
    unsigned char uc[4];// 4x1 bytes
} ;
```
The above union is stored as the following in the memory:
```    
  ---------------------    
  |    |    |    |    |
  ---------------------
  mix_type
  hi         lo
  c[0] c[1] c[2] c[4] 
```


Lets creat a variable of type `mix_type`

```cpp
mix_type mix;
```
Now this:

```cpp
mix.uc[0]=1;
mix.uc[1]=1;
mix.uc[2]=1;
mix.uc[3]=1;
```
will be stored in the meomory as this: 

```
00000001000000010000000100000001
```
which can be printed with the following casting:
```cpp
std::cout<< mix.ui <<std::endl;  
std::cout<< std::bitset<8*sizeof(mix_type)>(mix.ui) <<std::endl;
std::bitset<8*sizeof(mix_type)> union_bitset;
union_bitset[0]=1;
union_bitset[8]=1;
union_bitset[16]=1;
union_bitset[24]=1;
std::cout<< union_bitset.to_ulong() <<std::endl;
std::cout<< union_bitset.to_string() <<std::endl;
std::cout<<sizeof(unsigned int)  <<std::endl;
```

The union could also be used for aliasing. For instance:

```cpp
union Vector3
{
  struct{ double x,y,z ; } ;
  double elements[3];
}
```

Iterating by index:

```cpp
for( int i = 0 ; i < 3 ; i++ )
  Vector3_object.elements[i]=1.0;
```
Accessing by name:

```cpp
Vector3_object.x=1.0;
Vector3_object.y=1.0;
Vector3_object.z=1.0;
```




