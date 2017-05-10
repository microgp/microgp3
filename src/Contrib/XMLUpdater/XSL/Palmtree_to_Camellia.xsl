<?xml version="1.0"?>

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>

<!-- Copy everything -->
<xsl:template match="@*|node()">
  <xsl:copy>
    <xsl:apply-templates select="@*|node()" />
  </xsl:copy>
</xsl:template>

<!-- replace/add some attributes and put the whole document in $result -->
<xsl:template match="evolutionaryAlgorithm">
  <xsl:copy>
    <xsl:apply-templates select="@*"/>
    <xsl:attribute name="randomType">rand48</xsl:attribute>
    <xsl:if test=".//individual/@allopatricTag">
      <xsl:attribute name="infinityStringIndividualAllopatricTag">
        <!-- Select max of currently found allopatric tags -->
        <xsl:for-each select=".//individual/@allopatricTag">
          <xsl:sort select="." data-type="text"/>
          <xsl:if test="position() = last()">
            <xsl:value-of select="." />
          </xsl:if>
        </xsl:for-each>
      </xsl:attribute>
    </xsl:if>
    <xsl:if test=".//groups/@infinityStringGroup">
      <xsl:attribute name="infinityStringGroup">
        <xsl:value-of select=".//groups/@infinityStringGroup" />
      </xsl:attribute>
    </xsl:if>
    <xsl:attribute name="elapsedTimeCurrentRun">
      <xsl:value-of select="floor(number(@seconds))" />
    </xsl:attribute>
    <xsl:attribute name="elapsedTimePreviousRuns">0</xsl:attribute>
    <xsl:text>&#xa;</xsl:text>
    <xsl:apply-templates select="node()"/>
  </xsl:copy><xsl:text>&#xa;</xsl:text>
</xsl:template>

<!-- remove old attributes -->
<xsl:template match="groups/@allopatricTagGroup | groups/@infinityStringGroup">
</xsl:template>

<!-- will be used to deduplicate operators -->
<xsl:key name="opByRef" match="operatorsStatistics/operator | groupOperatorsStatistics/operator | operators/operator" use="@ref"/>

<xsl:template match="parameters">
  <xsl:copy>
    <xsl:apply-templates select="@*"/>
    <xsl:text>&#10;</xsl:text>
    <xsl:if test="@type = 'group'">
      <!-- add groupNu and groupMu if not already present -->
      <xsl:if test="not(groupMu)">
        <xsl:comment> The maximum number of groups </xsl:comment><xsl:text>&#xa;</xsl:text>
        <xsl:element name="groupMu">
          <xsl:attribute name="value">
            <xsl:value-of select="maxGroups/@value" />
          </xsl:attribute>
        </xsl:element><xsl:text>&#xa;</xsl:text>
      </xsl:if>
      <xsl:if test="not(groupNu)">
        <xsl:comment> The initial number of groups </xsl:comment><xsl:text>&#xa;</xsl:text>
        <xsl:element name="groupNu">
          <xsl:attribute name="value">
            <xsl:value-of select="maxGroups/@value" />
          </xsl:attribute>
        </xsl:element><xsl:text>&#xa;</xsl:text>
      </xsl:if>
    </xsl:if>
    <!-- copy other elements -->
    <xsl:apply-templates select="node()"/>
    <!-- build operator list from group/genetic operator statistics -->
    <xsl:element name="operators">
      <xsl:attribute name="default">none</xsl:attribute>
      <xsl:text>&#xa;</xsl:text>
      <xsl:for-each select="operatorsStatistics/node() | groupOperatorsStatistics/node() | operators/node()">
        <xsl:choose>
          <xsl:when test="self::* and name() = 'operator'">
            <!-- declare each operator only once -->
            <xsl:if test="generate-id() = generate-id(key('opByRef', @ref)[1])">
              <xsl:copy>
                <xsl:apply-templates select="@*"/>
                <xsl:if test="./weight/@current = 0">
                  <xsl:attribute name="enabled">0</xsl:attribute>
                </xsl:if>
              </xsl:copy><xsl:text>&#xa;</xsl:text>
            </xsl:if>
          </xsl:when>
          <xsl:when test="self::comment()">
            <!-- copy comments -->
            <xsl:copy/><xsl:text>&#xa;</xsl:text>
          </xsl:when>
        </xsl:choose>
      </xsl:for-each>
    </xsl:element><xsl:text>&#xa;</xsl:text>
  </xsl:copy><xsl:text>&#xa;</xsl:text>
</xsl:template>

<!-- renamed operators -->
<xsl:template match="operator/@ref[. = 'groupMultiPointCrossover']">
  <xsl:if test="not(../operator[@ref = 'groupBalancedCrossover'])">
    <xsl:attribute name="ref">groupBalancedCrossover</xsl:attribute>
  </xsl:if>
</xsl:template>
<xsl:template match="operator/@ref[. = 'groupMultiPointImpreciseCrossover']">
  <xsl:if test="not(../operator[@ref = 'groupUnalancedCrossover'])">
    <xsl:attribute name="ref">groupUnbalancedCrossover</xsl:attribute>
  </xsl:if>
</xsl:template>

<!-- round number of milliseconds (we now expect uint) -->
<xsl:template match="evaluator/totalMilliseconds/@value">
  <xsl:attribute name="value">
    <xsl:value-of select="floor(number(.))"/>
  </xsl:attribute>
</xsl:template>

<!-- maybe force cacheSize = 0 -->
<xsl:template match="evaluator">
  <xsl:copy>
    <xsl:apply-templates select="@*"/>
    <xsl:text>&#10;</xsl:text>
    <xsl:if test="../invalidateFitnessAfterGeneration[@value = '1']">
      <xsl:comment> Because 'invalidateFitnessAfterGeneration' is set </xsl:comment>
      <cacheSize value="0" />
    </xsl:if>
    <xsl:apply-templates select="node()"/>
  </xsl:copy>
</xsl:template>

<!-- delete deprecated elements-->
<xsl:template match="parameters[@type='group']/kappa | parameters[@type='group']/minGroups | parameters[@type='group']/maxGroups">
  <xsl:comment>
    <xsl:text> Deprecated: </xsl:text>
    <xsl:apply-templates select="." mode="serialize"/>
    <xsl:text> </xsl:text>
  </xsl:comment>
</xsl:template>

<!-- remove old operators sections -->
<xsl:template match="operatorsStatistics | groupOperatorsStatistics | operators">
</xsl:template>

<xsl:template match="population/individuals | population/groups">
  <xsl:copy>
    <xsl:attribute name="rawBest">
      <xsl:value-of select="@best"/>
    </xsl:attribute>
    <xsl:attribute name="rawWorst">
      <xsl:value-of select="@worst"/>
    </xsl:attribute>
    <!-- copy other elements and attributes -->
    <xsl:apply-templates select="@*|node()"/>
  </xsl:copy><xsl:text>&#xa;</xsl:text>
</xsl:template>

<!-- update individual state -->
<xsl:template match="individual/life">
<xsl:copy>
  <xsl:attribute name="state">
    <xsl:choose>
      <xsl:when test="../@isZombie = 'true'">
        <xsl:text>1</xsl:text>
      </xsl:when>
      <xsl:when test="../@isHero = 'true'">
        <xsl:text>3</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>2</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:attribute>
  <!-- copy other elements and attributes -->
  <xsl:apply-templates select="@*|node()"/>
</xsl:copy><xsl:text>&#xa;</xsl:text>
</xsl:template>

<!-- remove old attributes -->
<xsl:template match="individual/@isHero | individual/@isZombie">
</xsl:template>

<!-- create group life -->
<xsl:template match="group">
<xsl:copy>
  <xsl:apply-templates select="@*"/>
  <xsl:element name="life">
    <xsl:attribute name="birth">
      <xsl:value-of select="@birth"/>
    </xsl:attribute>
    <xsl:attribute name="age">
      <xsl:value-of select="@age"/>
    </xsl:attribute>
  </xsl:element><xsl:text>&#xa;</xsl:text>
  <!-- copy other elements -->
  <xsl:apply-templates select="node()"/>
</xsl:copy><xsl:text>&#xa;</xsl:text>
</xsl:template>

<!-- remove these attributes -->
<xsl:template match="group/@birth | group/@age">
</xsl:template>

<!-- update lineage -->
<xsl:template match="lineage/parents/individual">
  <xsl:element name="parent">
    <xsl:apply-templates select="@*|node()"/>
  </xsl:element><xsl:text>&#xa;</xsl:text>
</xsl:template>
  
<!-- Utils: simple serializer -->
<xsl:template match="*" mode="serialize">
    <xsl:text>&lt;</xsl:text>
    <xsl:value-of select="name()"/>
    <xsl:apply-templates select="@*" mode="serialize" />
    <xsl:choose>
        <xsl:when test="node()">
            <xsl:text>&gt;</xsl:text>
            <xsl:apply-templates mode="serialize" />
            <xsl:text>&lt;/</xsl:text>
            <xsl:value-of select="name()"/>
            <xsl:text>&gt;</xsl:text>
        </xsl:when>
        <xsl:otherwise>
            <xsl:text> /&gt;</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="@*" mode="serialize">
    <xsl:text> </xsl:text>
    <xsl:value-of select="name()"/>
    <xsl:text>="</xsl:text>
    <xsl:value-of select="."/>
    <xsl:text>"</xsl:text>
</xsl:template>

<xsl:template match="text()" mode="serialize">
    <xsl:value-of select="."/>
</xsl:template>

</xsl:stylesheet>
