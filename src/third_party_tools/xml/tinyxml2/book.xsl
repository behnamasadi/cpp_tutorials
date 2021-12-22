<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
<html>
<body>
<table>
<tr>
	<td>title</td>
	<td>author</td>
	<td>ISBN</td>
</tr>
<xsl:for-each select="shop/book">
<tr>
  	<td> <xsl:value-of select="title"/> </td>
	<td> <xsl:value-of select="author"/> </td>
	<td> <xsl:value-of select="ISBN"/> </td>
</tr>
</xsl:for-each>
</table>
</body>
</html>
</xsl:template>
</xsl:stylesheet>

