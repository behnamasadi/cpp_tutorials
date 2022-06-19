# Scope resolution operator ::

The `::` (scope resolution) is a unary operator that is used to qualify  the namespace scope of names.


```cpp
int n = 12; // A global variable

int main()
{
  	int n = 13; // A local variable

	std::cout << ::n << '\n'; // Print the global variable: 12
	std::cout << n   << '\n'; // Print the local variable: 13
}
```



[code](../src/scope_resolution_operator.cpp)
