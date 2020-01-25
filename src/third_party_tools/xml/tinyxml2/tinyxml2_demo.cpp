#include "tinyxml2.h"
#include <iostream>

/*
========================= Character =========================
Almost every legal Unicode character may appear in an XML document. All of an XML document is case-sensitive

========================= Markup and Content =========================
Strings that constitute markup either begin with the character < and end with a >, or they begin
with the character & and end with a ;
Strings of characters that are not markup are content.

========================= CDATA =========================
stands for Character Data and it means that the data in between these strings includes data that
could be interpreted as XML markup, but should not be.
They are used to escape blocks of text containing characters which would otherwise be recognized as markup.
An example of a CDATA section, in which " <greeting> " and " </greeting> " are recognized as character data, not markup:
<![CDATA[<greeting>Hello, world!</greeting>]]>
The key differences between CDATA and comments is CDATA is still part of the document, while a comment is not.


========================= Tag =========================
A tag is a markup construct that begins with < and ends with >
start-tag, such as <section>;
end-tag, such as </section>;
empty-element tag, such as <line-break />.


========================= Element =========================
An element is a logical document component that either begins with a start-tag and ends with a matching end-tag or
consists only of an empty-element tag.

========================= Attribute =========================
An attribute is a markup construct consisting of a name–value pair that exists within a tag.

========================= Encoding detection =========================

========================= Escaping Character =========================
&lt; represents "<";
&gt; represents ">";
&amp; represents "&";
&apos; represents "'";
&quot; represents '"'.

========================= XML Declaration =========================
<?xml version = "version_number" encoding = "encoding_declaration" standalone = "standalone_status" ?>

Version:	1.0,  1.1
Encoding:	UTF-8, UTF-16, ISO-10646-UCS-2, ISO-10646-UCS-4, ISO-8859-1 to ISO-8859-9, ISO-2022-JP, Shift_JIS, EUC-JP
Standalone:		yes or no

========================= XML DTD (Document Type Definition) =========================
A DTD defines what elements are required and what attributes can be set.
DTDs check vocabulary and validity of the structure of XML documents against grammatical rules of appropriate XML language.
An XML DTD can be either specified inside the document, or outside.

<! DOCTYPE <root-element-to-start-parsing> <DTD-identidfier(might be a path to a file on the system or URL)>
[
DTD Body
]
>

example of internal DTD
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


example of external DTD
<?xml version = "1.0" encoding = "UTF-8" standalone = "no" ?>
<!DOCTYPE address SYSTEM "address.dtd">
<address>
   <name>Tanmay Patil</name>
   <company>TutorialsPoint</company>
   <phone>(011) 123-4567</phone>
</address>

address.dtd
<!ELEMENT address (name,company,phone)>
<!ELEMENT name (#PCDATA)>
<!ELEMENT company (#PCDATA)>
<!ELEMENT phone (#PCDATA)>


#PCDATA means parse-able text data.

========================= XML XSD (Schema Definition) =========================
It is used to describe and validate the structure and the content of XML data. XML schema defines
the elements, attributes and data types. Basically it describe the legitimacy of a XML document.

<xs:element name = "name-of-the-element" type = "xs-type" />

xs-type: xs:integer, xs:boolean, xs:string, xs:date, ...

Complex Type
A complex type is a container for other element definitions.
This allows you to specify which child elements an element can contain and to provide some
structure within your XML documents.


Attributes
Attributes in XSD provide extra information within an element. Attributes have name and type property as shown below −
<xs:attribute name = "x" type = "y"/>

Full Example

<?xml version="1.0" encoding = "UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
    <xs:element name="contact">
        <xs:complexType>
            <xs:sequence>
                <xs:element name="name" type="xs:string" />
                <xs:element name="company" type="xs:string" />
                <xs:element name="phone" type="xs:int" />
            </xs:sequence>
        </xs:complexType>
    </xs:element>
</xs:schema>

XSD is used for example more by Microsoft schemas.There is no difference, it is just a matter of choice.

========================= XML Databases XQuery and XPath =========================
XML Database could be used to store information in the XML format.
The data stored in the database can be queried using XQuery, serialized, and exported into a desired format.

There are two major types of XML databases −
1)XML- enabled
2)Native XML (NXD)

XPath: is made of a path like syntax similar to directory structures.
XQuery: is very similar to SWL query and is a superset of XPath. You need an XQuery engine to “run” the query.
For - selects a sequence of nodes
Let - binds a sequence to a variable
Where - filters the nodes
Order by - sorts the nodes
Return - what to return (gets evaluated once for every node)



Suppose we have the following XML:


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

example of XPath:

"/" all books
"/book/title" all titles

examples of XQuery:

for $x in "/book"
where $x/ISBN>123
return $x/title


========================= XML EXtensible Stylesheet Language Transformations (XSLT) =========================
They can be used to render the XML file as HTML doc
 It is a language for transforming XML documents into other XML documents/ formats such as HTML, plain text or XSL Formatting Objects,
which may subsequently be converted to other formats, such as PDF, PostScript and PNG.



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

The output is:


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
*/



int main(int argc, char** argv)
{

}
