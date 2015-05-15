<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet version="1.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:c="http://www.cad.polito.it/ugp3/schemas/constraints">

  <xsl:output method="html" omit-xml-declaration="yes"/>

  <xsl:template match="/">
    <![CDATA[<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">]]>
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <title><xsl:value-of select="/c:constraints/@language"/> constraints</title>
        <!--link href="constraintsScripted.css" rel="stylesheet" type="text/css" /-->
        <style type="text/css">
          a 
{
	color: #0099FF;
	cursor: pointer;
}

a.header
{
	color: #000000;
	cursor: pointer;
	font-weight:bold;
}
th.parameters
{
	text-align:left;
	font-style:italic;
	font-weight: bold;
}
body 
{
	margin:30px;
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: medium;
	text-align: center;
}

h1 
{
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: xx-large;
	font-style: normal;
}

pre
{
	font-size:larger;
	border: thin solid #CCCCCC;
		
}

div.library 
{
	border:1px solid #CCCCCC;
	background:#EAFFFF;
	padding:40px;

	margin-left: auto;
	margin-right: auto;
	width: 65%;
}


div.typeDefinitions
{
	border:1px solid #CCCCCC;
	background:#FFFFFF;
	padding:15px;


	margin-left: auto;
	margin-right: auto;
	
	text-align:left;
	vertical-align:top;
}

div.sections
{
	margin-left: auto;
	margin-right: auto;
}
div.ext
{
	text-align:left;
	margin:0 auto;
	min-height:100px;
	border:1px solid #ccc;
	padding:30px;
}
div.macro 
{
	text-align:left;
	background-color: #EEF7FF;
	border: thin solid #CCCCCC;
	margin-left: auto;
	margin-right: auto;
	vertical-align:top;
	padding:10px;
}

div.section 
{
	border:1px solid #CCCCCC;
	background:#DFF8FF;
	padding:20px;
	text-align:center;
	background-color: #DFF8FF;
	
	margin-left: auto;
	margin-right: auto;
}

table.typeDefinitions
{
	text-align:left;
	background-color: #FFFFFF;
	border: thin solid #CCCCCC;
	margin-left: auto;
	margin-right: auto;
	vertical-align:top;
}

tr.typeDefinition
{
	text-align:left;
	background-color: #FFFFFF;
	border: thin solid #CCCCCC;
	margin-left: auto;
	margin-right: auto;
	vertical-align:top;
}

table.subSection 
{
	text-align:center;
	background-color: #DFEBFF;
	border:thin solid #CCCCCC;
	margin-left: auto;
	margin-right: auto;
	padding:20px;
}

div.header 
{
	text-align:left;
	background-color: #F9FCFF;
	border: thin solid #CCCCCC;
	margin-left: auto;
	margin-right: auto;
	vertical-align:top;
	padding:10px;
}
        </style>
        <script type="text/javascript">

function switchMenu(obj) {
    var el = document.getElementById(obj);
    if ( el.style.display != "none" ) {
        el.style.display = 'none';
    }
    else {
        el.style.display = '';
    }
}

        </script>
      </head>
      <body>    
          <xsl:apply-templates select="/c:constraints" />    
      </body>
    </html>
  </xsl:template>

  <xsl:template match="/c:constraints">
    <div id="wrapper">
      <h1>
        <xsl:value-of select="@id"/>
      </h1>
      <div class="library">
        <a onclick="switchMenu('library');" title="Instruction Library" class="header">Constraints</a>
        <div id="library" >
          <p>
            <xsl:apply-templates select="/c:constraints/c:typeDefinitions" />
          </p>
          <p>
            <xsl:apply-templates select="/c:constraints/c:prologue" />
          </p>
          <p>
            <xsl:apply-templates select="/c:constraints/c:sections" />
          </p>
          <p>
            <xsl:apply-templates select="/c:constraints/c:epilogue" />
          </p>
        </div>
      </div>
    </div>
  </xsl:template>

  <xsl:template match="/c:constraints/c:typeDefinitions">
    <div class="typeDefinitions">
      <a onclick="switchMenu('typedefs');" title="Type definitions" class="header">Type definitions</a>
      <div id="typedefs" style="display:none">
        <table width="100%" cellspacing="0" cellpadding="0" class="typeDefinitions" border="1">
          <tr>
            <th>Name</th>
            <th>Base type</th>
            <th>Values</th>
          </tr>
          <xsl:for-each select="./c:item">
            <xsl:apply-templates select="." />
          </xsl:for-each>
        </table>
      </div>
    </div>
  </xsl:template>

  <xsl:template match="/c:constraints/c:typeDefinitions/c:item">
    <tr class="typeDefinition">
      <td>
        <a id="definedType{@id}" />
        <xsl:value-of select="@id"/>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="@xsi:type = 'bitArray'">
           bitArray<xsl:choose>
             <xsl:when test="@base = 'bin'">, binary
             </xsl:when>
             <xsl:when test="@base = 'oct'">, octal
             </xsl:when>
             <xsl:when test="@base = 'hex'">, hexadecimal
             </xsl:when>
             <xsl:otherwise>, binary
             </xsl:otherwise>
           </xsl:choose>
		  </xsl:when>		
          <xsl:when test="@xsi:type = 'integer'">
           integer
          </xsl:when>
          <xsl:when test="@xsi:type = 'float'">
           float
          </xsl:when>
          <xsl:when test="@xsi:type = 'constant'">
            constant
          </xsl:when>
          <xsl:when test="@xsi:type = 'outerLabel'">
            outer label
          </xsl:when>
          <xsl:when test="@xsi:type = 'innerGenericLabel'">
            inner label, generic
          </xsl:when>
          <xsl:when test="@xsi:type = 'innerForwardLabel'">
            inner label, forward only
          </xsl:when>
          <xsl:when test="@xsi:type = 'innerBackwardLabel'">
            inner label, backward only
          </xsl:when>
          <xsl:when test="@xsi:type = 'uniqueTag'">
            unique tag
          </xsl:when>
        </xsl:choose>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="@xsi:type = 'bitArray'">
           |<xsl:value-of select="@pattern"/>|
          </xsl:when>
          <xsl:when test="@xsi:type = 'integer' or @xsi:type = 'float'">
           [<xsl:value-of select="@minimum"/>; <xsl:value-of select="@maximum"/>]
          </xsl:when>
          <xsl:when test="@xsi:type = 'constant'">
            <xsl:for-each select="c:value">
              '<xsl:value-of select="." />'
              <xsl:text> </xsl:text>
            </xsl:for-each>
          </xsl:when>
          <xsl:when test="@xsi:type = 'outerLabel'">
            <xsl:for-each select="c:ref">
              <a href="#section{@section}">
                <xsl:value-of select="@section"/>
              </a>
              <xsl:text> </xsl:text>
            </xsl:for-each>
          </xsl:when>
          <xsl:when test="@xsi:type = 'innerGenericLabel' or @xsi:type = 'innerForwardLabel' or @xsi:type = 'innerBackwardLabel'">
            can refer to:
            <xsl:if test="@prologue = 'true'">prologue </xsl:if>
            <xsl:if test="@epilogue = 'true'">epilogue </xsl:if>
            <xsl:if test="@itself = 'true'">itself </xsl:if>
          </xsl:when>
        </xsl:choose>
      </td>
    </tr>
  </xsl:template>

  <xsl:template match="c:item">
    <xsl:param name="containerID" />
    <tr>
      <td>
        <a id="parameter{$containerID}{@id}" />
        <xsl:value-of select="@name"/>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="@xsi:type = 'bitArray'">
           bitArray<xsl:choose>
             <xsl:when test="@base = 'bin'">, binary
             </xsl:when>
             <xsl:when test="@base = 'oct'">, octal
             </xsl:when>
             <xsl:when test="@base = 'hex'">, hexadecimal
             </xsl:when>
             <xsl:otherwise>, binary
             </xsl:otherwise>
           </xsl:choose>
          </xsl:when>
          <xsl:when test="@xsi:type = 'integer'">
           integer
          </xsl:when>
          <xsl:when test="@xsi:type = 'float'">
           float
          </xsl:when>
          <xsl:when test="@xsi:type = 'constant'">
            constant
          </xsl:when>
          <xsl:when test="@xsi:type = 'outerLabel'">
            outer label
          </xsl:when>
          <xsl:when test="@xsi:type = 'innerGenericLabel'">
            inner label, generic
          </xsl:when>
          <xsl:when test="@xsi:type = 'definedType'">
            <a href="#definedType{@ref}">
              <xsl:value-of select="@ref"/>
            </a>
          </xsl:when>
          <xsl:when test="@xsi:type = 'innerForwardLabel'">
            inner label, forward only
          </xsl:when>
          <xsl:when test="@xsi:type = 'innerBackwardLabel'">
            inner label, backward only
          </xsl:when>
          <xsl:when test="@xsi:type = 'uniqueTag'">
            unique tag
          </xsl:when>
        </xsl:choose>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="@xsi:type = 'bitArray'">
           pattern: <xsl:value-of select="@pattern"/>
		   length: <xsl:value-of select="@length"/>
          </xsl:when>
          <xsl:when test="@xsi:type = 'integer' or @xsi:type = 'float'">
           [<xsl:value-of select="@minimum"/>; <xsl:value-of select="@maximum"/>]
          </xsl:when>
          <xsl:when test="@xsi:type = 'constant'">
            <xsl:for-each select="c:value">
              '<xsl:value-of select="." />'
              <xsl:text> </xsl:text>
            </xsl:for-each>
          </xsl:when>
          <xsl:when test="@xsi:type = 'outerLabel'">
            <xsl:for-each select="c:ref">
              <a href="#section{@section}">
                <xsl:value-of select="@section"/>
              </a>
              <xsl:text> </xsl:text>
            </xsl:for-each>
          </xsl:when>
          <xsl:when test="@xsi:type = 'innerGenericLabel' or @xsi:type = 'innerForwardLabel' or @xsi:type = 'innerBackwardLabel'">
            can refer to:
            <xsl:if test="@prologue = 'true'">prologue </xsl:if>
            <xsl:if test="@epilogue = 'true'">epilogue </xsl:if>
            <xsl:if test="@itself = 'true'">itself </xsl:if>
          </xsl:when>
        </xsl:choose>
      </td>
    </tr>
  </xsl:template>

  <xsl:template match="/c:constraints/c:sections">
    <div class="sections">
      <xsl:for-each select="./c:section">
        <p>
          <xsl:apply-templates select="." />
        </p>
      </xsl:for-each>
    </div>
  </xsl:template>

  <xsl:template match="/c:constraints/c:sections/c:section">
    <div class="section">
      <a onclick="switchMenu('section{@id}');" title="Section" class="header">Section '<xsl:value-of select="@id"/>'</a>
      <div id="section{@id}">
        <table width="100%" cellspacing="5">
          <tr>
            <td>
              <xsl:apply-templates select="./c:prologue" />
            </td>
          </tr>
          <xsl:apply-templates select="./c:subSections" />
          <tr>
            <td>
              <xsl:apply-templates select="./c:epilogue" />
            </td>
          </tr>
        </table>
      </div>
    </div>
  </xsl:template>

  <xsl:template match="/c:constraints/c:sections/c:section/c:subSections">
    <xsl:for-each select="./c:subSection">
      <tr>
        <td>
          <xsl:apply-templates select="." />
        </td>
      </tr>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="/c:constraints/c:sections/c:section/c:subSections/c:subSection">
    <table width="100%" cellspacing="10" class="subSection">
      <tr>
        <td>
          <xsl:apply-templates select="./c:prologue" />
        </td>
      </tr>
      <tr>
        <td>
            <xsl:apply-templates select="./c:macros" />
        </td>
      </tr>
      <tr>
        <td>
          <xsl:apply-templates select="./c:epilogue" />
        </td>
      </tr>
    </table>
  </xsl:template>

  <xsl:template match="/c:constraints/c:sections/c:section/c:subSections/c:subSection/c:macros">
    <table width="100%" cellspacing="0" class="macros">
      <xsl:for-each select="./c:macro">
        <tr>
          <td>
            <xsl:apply-templates select="." />
          </td>
        </tr>
      </xsl:for-each>
    </table>
  </xsl:template>

  <xsl:template match="/c:constraints/c:sections/c:section/c:subSections/c:subSection/c:macros/c:macro">
    <div class="macro">
      <a onclick="switchMenu('macro{@id}');" title="Macro" class="header">Macro '<xsl:value-of select="@id"/>'</a>
      <div id="macro{@id}" style="display:none">
        <table width="100%" cellspacing="5" >
          <tr>
            <xsl:apply-templates select="./c:expression">
              <xsl:with-param name="containerID" select="@id" />
            </xsl:apply-templates>
          </tr>
          <xsl:apply-templates select="./c:parameters">
            <xsl:with-param name="containerID" select="@id" />
          </xsl:apply-templates>
        </table>
      </div>
    </div>
  </xsl:template>

  <xsl:template match="c:prologue">
    <xsl:if test="not(./c:expression = '')">
      <div class="header">
        <a onclick="switchMenu('prologue{@id}');" title="Click to expand/collapse this prologue" class="header">
          Prologue '<xsl:value-of select="@id"/>'
        </a>
        <div id="prologue{@id}" style="display:none">
          <table width="100%" cellspacing="5" >
            <tr>
              <xsl:apply-templates select="./c:expression" >
                <xsl:with-param name="containerID" select="@id" />
              </xsl:apply-templates>
            </tr>
            <xsl:apply-templates select="./c:parameters">
              <xsl:with-param name="containerID" select="@id" />
            </xsl:apply-templates>
          </table>
        </div>
      </div>
    </xsl:if>
  </xsl:template>

  <xsl:template match="c:epilogue">
  <xsl:if test="not(./c:expression = '')">
    <div class="header">
      <a onclick="switchMenu('epilogue{@id}');" title="Click to expand/collapse this epilogue" class="header">Epilogue '<xsl:value-of select="@id"/>'</a>
      <div id="epilogue{@id}" style="display:none">
        <table width="100%" cellspacing="5" class="header">

          <tr>
            <xsl:apply-templates select="./c:expression" >
              <xsl:with-param name="containerID" select="@id" />
            </xsl:apply-templates>
          </tr>
          <xsl:apply-templates select="./c:parameters">
            <xsl:with-param name="containerID" select="@id" />
          </xsl:apply-templates>
        </table>
      </div>
    </div>
    </xsl:if>
  </xsl:template>

  <xsl:template match="c:expression" xml:space="preserve">
    <tt><pre><xsl:apply-templates select="node()"/></pre></tt>
  </xsl:template>

  <xsl:template match="c:param">
    <a>&lt;</a>
    <a href="#parameter{@ref}" title="the parameter '{@ref}'">
      <xsl:value-of select="@ref"/>
    </a>
    <a>&gt;</a>
  </xsl:template>

  <xsl:template match="c:parameters">
    <tr>
      <!-- empty row -->
      <td colspan="3"></td>
    </tr>
    <tr>
      <td colspan="3">
        <table>
          <tr>
            <th width="15%" class="parameters">Parameter</th>
            <th width="15%" class="parameters">Type</th>
            <th class="parameters">Values</th>
          </tr>
          <xsl:for-each select="./c:item" >
            <xsl:apply-templates select="." >
              <xsl:with-param name="containerID" select="../@id"/>
            </xsl:apply-templates>
          </xsl:for-each>
        </table>
      </td>
    </tr>

  </xsl:template>


</xsl:stylesheet>
