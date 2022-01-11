# Lambda Expression

syntax:
```cpp
[ captures ] ( params ) { body }
[ captures ] ( params ) -> ret { body }
```

- inline:

```cpp
std::sort(v1.begin(),v1.end(),[](int x,int y){ return (x<y); }  );
```

- outside:

```cpp

    auto increaser=[](student &s)
    {
        s.x++;
        return;
    };
    
    std::vector<student> students;
    std::for_each(students.begin(),students.end(),increaser);
```


- outside as a function:

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

- lambda pass by value:


`[ = ]` this means passing parameters by value, but they are read only, you need to use mutable to modify read only


```cpp
    int a;
    a=10;
    auto lambda =[=]()  mutable { std::cout<<++a<<std::endl; };
    lambda();
    std::cout<<a <<std::endl;
```



- lambda pass by ref:


```cpp
    int a;
    a=10;
    auto lambda =[&](){ std::cout<<++a<<std::endl;b++; };
    lambda();

```

[source code](src/lambda.cpp)
