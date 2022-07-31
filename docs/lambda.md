# Lambda Expression

## syntax:
```cpp
[ captures ] ( params ) { body }
[ captures ] ( params ) -> ret { body }
```

##  inline:

```cpp
std::sort(v1.begin(),v1.end(),[](int x,int y){ return (x<y); }  );
```

##  outside:

```cpp

    auto increaser=[](student &s)
    {
        s.x++;
        return;
    };
    
    std::vector<student> students;
    std::for_each(students.begin(),students.end(),increaser);
```


##  outside as a function:

```cpp

auto increaserFunction=[](student &s)
{
    s.x++;
    return;
};

int main()
{
    std::for_each(students.begin(),students.end(),increaserFunction);
}

```

## captures

To access other variables other than what was passed to lambda within it, we can use capture-clause `[]`. You can capture by both reference and value, which you can specify using & and = respectively:

### by value
`[=]` capture all variables within scope by value:

```cpp
    int a;
    a=10;
    auto lambda =[=]()  mutable { std::cout<<++a<<std::endl; };
    lambda();
    std::cout<<a <<std::endl;
```

### by reference

- `[&]` capture all variables within scope by reference

```cpp
    int a;
    a=10;
    auto lambda =[&](){ std::cout<<++a<<std::endl;b++; };
    lambda();

```

- `[&var]` capture var by reference
- `[&, var]` specify that the default way of capturing is by reference and we want to capture var
- `[=, &var]` capture the variables in scope by value by default, but capture var using reference instead




Refs: [1](https://stackoverflow.com/questions/39789125/what-does-mean-before-function)



[source code](../src/lambda.cpp)
