# Doxygen


Executing: `doxygen -g. Doxyfile` will generate a `Doxyfile`.

The most important sections are:


**PROJECT_NAME**

```PROJECT_NAME= "Simple Example"
```


**INPUT**
path to your source code:
```
INPUT=./source
```

**DOT_PATH**
on windows 

```
DOT_PATH = C:\Program Files\Graphviz\bin
```
on Linux:

```
DOT_PATH =/usr/bin/dot
```

**OUTPUT**

```
HTML_OUTPUT            = doc/html
OUTPUT_DIRECTORY       = .
```

or you can run 

```
doxywizard Doxyfile
```

and see your modified values in red:


<img src="images/doxygen1.png" width="80%"  height="80%"/>

<img src="images/doxygen2.png" width="80%"  height="80%" />
<img src="images/doxygen3.png"  width="80%"  height="80%"/>

<img src="images/doxygen4.png" width="80%"  height="80%"/>
<img src="images/doxygen5.png"  width="80%"  height="80%"/>


## Doxygen UML Diagram and Graph Legend

[Doxygen uml diagram and graph legend](https://www.doxygen.nl/manual/examples/diagrams/html/graph_legend.html)
