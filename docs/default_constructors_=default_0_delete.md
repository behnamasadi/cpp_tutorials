# Default constructors
A default constructor is a constructor which can be called with no arguments.


```cpp
class_name ( ) ;	(1)
```


	
class_name :: class_name ( ) body	(2)	
class_name() = delete ;	(3)	(since C++11)
class_name() = default ;	(4)	(since C++11)
class_name :: class_name ( ) = default ;	(5)	(since C++11)






[code](../src/class/default_0_delete_meaning.cpp)
