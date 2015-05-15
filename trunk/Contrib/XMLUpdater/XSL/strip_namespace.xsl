<?xml version="1.0"?>

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>

<!-- Copy elements without copying their namespace declarations -->
<xsl:template match="*" name="identity">
  <xsl:element name="{name()}" namespace="">
    <xsl:apply-templates select="node()|@*" />
  </xsl:element>
</xsl:template>

<!-- Copy content as is -->
<xsl:template match="node()|@*" priority="-2">
  <xsl:copy>
    <xsl:apply-templates select="node()|@*" />
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
