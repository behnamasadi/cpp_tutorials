# Unnamed/Anonymous Namespaces

Refs: [1](https://stackoverflow.com/questions/357404/why-are-unnamed-namespaces-used-and-what-are-their-benefits)


# Nested Namespaces, 
Below is the syntax to use the nested namespace in one line 

```cpp
namespace Game { 
  
    namespace Graphics { 
  
        namespace Physics { 
  
           class 2D { 
           }; 
        } 
    } 
}


  
namespace Game::Graphics::Physics { 
  
    class 2D { 

    }; 
} 
```



[code](../src/nested_namespaces.cpp)
