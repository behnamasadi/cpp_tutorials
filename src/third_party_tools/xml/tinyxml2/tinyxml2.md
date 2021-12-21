# XML
XML is the abbreviation for eXtensible Markup Language and it has been designed with the purpose of storing and transporting data. XML was created with the goal of being both human and machine readable.
XML does not DO anything. XML is just information wrapped in tags. XML, unlike HTML does not use predefined tags
```
<?xml version="1.0" encoding="UTF-8"?>
<message>
  <to>you</to>
  <from>me</from>
  <title>reminder</title>
  <body>any text</body>
</message>
```
XML is extensible, that means you can add the followings:

```
<?xml version="1.0" encoding="UTF-8"?>
<message>
  <time>10:00 </time>
  <date>01.01.2020 </date>
  <to>you</to>
  <from>me</from>
  <title>reminder</title>
  <body>any text</body>
</message>
```
## XML Tree
An XML tree begins with a root element and branches to child elements from there. child elements are allowed to have sub-child elements as well.
XML documents must contain one root element which is the parent of all other elements:
```
<root>
  <child>
    <subchild>.....</subchild>
  </child>
</root>
```
## Encoding Detection

## Escaping Character
- `&lt;` represents "<";
- `&gt;` represents ">";
- `&amp;` represents "&";
- `&apos;` represents "'";
- `&quot;` represents '"'.

## XML Declaration
`<?xml version = "version_number" encoding = "encoding_declaration" standalone = "standalone_status" ?>`

- Version:	`1.0,  1.1`
- Encoding:	`UTF-8, UTF-16, ISO-10646-UCS-2, ISO-10646-UCS-4, ISO-8859-1 to ISO-8859-9, ISO-2022-JP, Shift_JIS, EUC-JP`
- Standalone:		`yes` or `no`



### The XML Prolog
The following line is called **Prolog**:
```
<?xml version="1.0" encoding="UTF-8"?>
```
It is optional, but if it exists, it must come as the first line in the document.
## Legal Character
Almost every legal Unicode character may appear in an XML document, i.e. `ø,æ,å,ê,è,é`. XML document is **case-sensitive**.

## Markup and Content
Strings that constitute markup either begin with the character `<` and end with a `>`, or they begin
with the character `&` and end with `a ;`, i.e. `&lt;` represents "<"
Strings of characters that are not markup are content.


## Tag
A tag is a markup construct that begins with `<` and ends with `>`
start-tag, such as `<section>`, end-tag, such as `</section>`
empty-element tag, such as `<line-break />`.


## Element
An element is a logical document component that either begins with a start-tag and ends with a matching end-tag

```
<element-name attributes="value"> contents</element-name>
```
or consists only of an empty-element tag:
```
<element-name attributes="value" /> 
```
An element can contain:
- other elements
- text
- attributes
- a mixture of the above, including none of them

```
<mediatech>
  <movie category="sci-fi">
    <title>The Matrix Resurrections</title>
    <director>Lana Wachowski</director>
    <year>2021 </year>
    <length>2h 28m</length>
  </movie>
  <book category="IT">
    <title>UML distilled</title>
    <author>Martin Fowler</author>
    <year>1997</year>
    <price>39.95</price>
  </book>
</mediatech>
```

In the example above:

- `<title>, <director>, <author>, <length>, <year>`, and `<price>` have text content because they contain text (like 39.95).

- `<mediatech>, <movie>` and `<book>` have element contents, because they contain elements.

- `<book> and <movie>` have  attribute (category="sci-fi").


## Attribute
An attribute is a markup construct consisting of a name-value pair that exists within a tag:

`<movie category="sci-fi">`

Attributes can **not** contain tree structures and they are not easily expandable (for future changes)
Attribute values must always be quoted. Either single or double quotes can be used.



## XML Elements vs. Attributes
There are no rules about when to use attributes or when to use elements in XML.

Elements form:
```
<person>
  <gender>female</gender>
  <firstname>Anna</firstname>
  <lastname>Smith</lastname>
</person>
```

Attribute form:
```
<person gender="female">
  <firstname>Anna</firstname>
  <lastname>Smith</lastname>
</person>
```

## XML Prefix and Namespaces
suppose you have the followings XML fragments:

```
<library>
    <name>OpenCV</name>
    <language>C++</language>
</library>
```
and 

```
<library>
    <name>City Media-tech Library</name>
    <type>public</type>
</library>
```

If these were added together, there would be a name conflict and a user or an XML parser will not be able to handle these differences.This problem can be solved by using a prefix.


```
<root>
<s:library>
    <s:name>OpenCV</s:name>
    <s:language>C++</s:language>
</s:library>

<p:library>
    <p:name>City Media-tech Library</p:name>
    <p:type>public</p:type>
</p:library>
</root>
```


If you use prefixes in XML, a namespace for the prefix must be defined. The namespace can be defined by an `xmlns` attribute in the start tag of an element with the following syntax. `xmlns:prefix="URI"`. An URI (Uniform Resource Identifier) is a string of characters which identifies an Internet Resource. URL is the most common URI which identifies an Internet domain address. 




```
<root>

<s:library xmlns:s="http://exmple.com/software-libraries/">
    <s:name>OpenCV</s:name>
    <s:language>C++</s:language>
</s:library>

<p:library xmlns:p="http://exmple.com/physical-libraries" >
    <p:name>City Media-tech Library</p:name>
    <p:type>public</p:type>
</p:library>

</root>
```
The namespace URI is **not** used by the parser to look up information. The use of a URI serves to give the namespace a unique name. Companies, frequently employ the namespace as a link to a web page containing namespace information. 



## CDATA 
Stands for Character Data and has the following form:
```cpp
<![CDATA[  your-string  ]]>
```
and it means that the data in between `<![CDATA[` and `]]>` includes data that could be interpreted as XML markup, but should not be. 
Any character data (other than `]]>`) can appear within the section without needing to be escaped. i.e. angle brackets `<>` and ampersands `&`.

For instance imagine you have a tutorial page in which your data includes lots of characters that include `&` and `<` but those characters aren't meant to be xml.
Instead of writing it like this:
```
<example-code>
while (x &lt; len &amp;&amp; !done) {
    print( &quot;Still working, &apos;zzz&apos;.&quot; );
    ++x;
    }
</example-code>
```
 you can write it as:
 
```cpp
<example-code><![CDATA[
while (x < len && !done) {
    print( "Still working, 'zzzz'." );
    ++x;
    }
]]></example-code>
```

The key differences between CDATA and comments is CDATA is **still part of the document, while a comment is not**.


##  XML DTD (Document Type Definition)
A DTD defines what elements are required and what attributes can be set. "Well Formed" XML refers to an XML document having correct syntax. A DTD-validated XML document is both 
"Well Formed" and "Valid."
DTDs check vocabulary and validity of the structure of XML documents against grammatical rules of appropriate XML language.
An XML DTD can be either specified inside the document, or outside.

Example of internal DTD

```
<?xml version="1.0" encoding="utf-8" standalone="yes"?>

<!DOCTYPE address
[
    <!ELEMENT address (name,company,phone)>
    <!ELEMENT name (#PCDATA)>
    <!ELEMENT company (#PCDATA)>
    <!ELEMENT phone (#PCDATA)>
]
>
<address>
    <name>customer name</name>
    <company>co ltd</company>
    <phone>123456</phone>
</address>
```

Example of external DTD
```
<?xml version = "1.0" encoding = "UTF-8" standalone = "no" ?>
<!DOCTYPE address SYSTEM "address.dtd">
<address>
   <name>Tanmay Patil</name>
   <company>TutorialsPoint</company>
   <phone>(011) 123-4567</phone>
</address>
```

The file `address.dtd` contains:
```
<!ELEMENT address (name,company,phone)>
<!ELEMENT name (#PCDATA)>
<!ELEMENT company (#PCDATA)>
<!ELEMENT phone (#PCDATA)>

```
PCDATA means parse-able text data.

# XML XSD (Schema Definition)
It is also used to describe and validate the structure and the content of XML data. 
The fundamental distinction between DTDs and XML Schema is that XML Schema uses an XML-based syntax, whereas DTDs use a unique syntax that dates back to SGML DTDs. Although DTDs are frequently chastised for requiring users to learn a new grammar, the syntax is actually extremely simple. 
The converse is true for XML Schema, which is verbose but also uses tags and XML, making the syntax of XML Schema less scary.
DTDs were created with the objective of maintaining SGML compatibility for programs that might desire to convert SGML DTDs to XML DTDs. However, because "conciseness in XML markup is of minimum importance," one of the aims of XML, there is no actual care about keeping the syntax short.


### XSD Schema
The `<schema>` element is the root element of every XML Schema:

```
<?xml version="1.0"?>

<xs:schema>
.
.
.
</xs:schema>
```

Referencing a Schema in an XML Document: 

```
<?xml version="1.0"?>

<movie xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="https://www.example.com/movie.xsd">
    <title>The Matrix Resurrections</title>
    <director>Lana Wachowski</director>
    <year>2021-10-01 </year>
    <length>2h 28m</length>
</movie>
```

### XSD Simple Elements
A simple element is an XML element that can contain only text. It cannot contain any other elements or attributes.
some of the  built-in data types are:

- xs:boolean
- xs:decimal
- xs:integer
- xs:string
- xs:date
- xs:time

Here are some XML elements:

```
<title>The Matrix Resurrections</title>
<director>Lana Wachowski</director>
<year>2021-10-01 </year>
<length>2h 28m</length>
```

The corresponding simple element definitions:

```
<xs:element name="title" type="xs:string" />
<xs:element name="director" type="xs:string" />
<xs:element name="year" type="xs:date"/>
<xs:element name="length" type="xs:string" />
```


#### Default and Fixed Values for Elements
When no additional value is specified for an element, it is given a default value.
```
<xs:element name="year" type="xs:date" default="2020-01-01"/>
```

A fixed value is also automatically assigned to the element, and you cannot specify another value.

```
<xs:element name="year" type="xs:date" fixed="2020-01-01"/>
```
### XSD Attributes

The syntax for defining an attribute is:

```
<xs:attribute name = "x" type = "y"/>
```
Here is an XML element with an attribute:

```
<name lang="EN">Smith</name>
```
 
```
<xs:attribute name="lang" type="xs:string"/>
```

#### Default and Fixed Values for Attributes
Just like elements, when no additional value is specified, it is given a default value.
```
<xs:attribute name="lang" type="xs:string" default="EN"/>
```
#### Optional and Required Attributes
Attributes are optional by default. To specify that the attribute is required, use the "use" attribute:
```
<xs:attribute name="lang" type="xs:string" use="required"/>
```


### XSD Complex Type
An XML element that contains other elements and/or attributes is known as a complicated type element.

There are four kinds of complex elements:

1) empty elements.
2) elements that containing only other elements.
3) elements that containing only text.
4) elements that containing both other elements and text.

Any of above element may or may not contain attributes as well.



## XSD Data

Refs: [1](https://stackoverflow.com/questions/1544200/what-is-difference-between-xml-schema-and-dtd)

# XML Databases XQuery and XPath
Information in the XML format could be stored in an XML database. 
<!-- 
There are two major types of XML databases:

1) XML- enabled
2) Native XML (NXD)
-->

## XQuery
The data in the XML database can be queried, and exported in a variety of formats using **XQuery**.
XQuery is to XML what SQL is to databases. XQuery is built on **XPath** expressions.

## XPath
XPath: is made of a path like syntax similar to directory structures.
XQuery: is very similar to SQL query and is a superset of XPath. You need an XQuery engine to “run” the query.
For - selects a sequence of nodes
Let - binds a sequence to a variable
Where - filters the nodes
Order by - sorts the nodes
Return - what to return (gets evaluated once for every node)



Suppose we have the following XML:

```
<?xml version="1.0" encoding="UTF-8" ?>
<?xml-stylesheet type="text/xsl" href="example.xsl" ?>
<book>
    <title>The lord of the ring </title>
    <ISBN> 123654</ISBN>
    <author>J. R. R. Tolkien </author>

    <title>On the Origin of Species </title>
    <ISBN>987123 </ISBN>
    <author>Charles Darwin </author>

    <title>Mumbo jumbo </title>
    <ISBN>789654 </ISBN>
    <author>first author </author>
    <author>second author </author>

</book>
```
example of XPath:

"/" all books
"/book/title" all titles

examples of XQuery:

for $x in "/book"
where $x/ISBN>123
return $x/title


# XML EXtensible Stylesheet Language Transformations (XSLT)
They can be used to render the XML file as HTML doc
 It is a language for transforming XML documents into other XML documents/ formats such as HTML, plain text or XSL Formatting Objects,
which may subsequently be converted to other formats, such as PDF, PostScript and PNG.


```
<?xml version="1.0" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body>
  <h2>The Collections</h2>
  <table border="1">
    <tr bgcolor="#9acd32">
      <th>Title</th>
      <th>Author</th>
    </tr>

    <xsl:for-each select="/book">
    <tr>
      <td><xsl:value-of select="title"/></td>
      <td><xsl:value-of select="author"/></td>
    </tr>
    </xsl:for-each>

  </table>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>
```
The output is:

```
<html>
<body>
<h2>The Collections</h2>
<table border="1">
<tr bgcolor="#9acd32">
<th>Title</th>
<th>Author</th>
</tr>
<tr>
<td>The lord of the ring </td>
<td>J. R. R. Tolkien </td>
</tr>
</table>
</body>
</html>
```

