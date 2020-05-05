<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:fn="http://www.w3.org/2005/xpath-functions"
  xmlns:xdt="http://www.w3.org/2005/xpath-datatypes"
  xmlns:err="http://www.w3.org/2005/xqt-errors"
  xmlns:ctk="http://commontk.org"
  exclude-result-prefixes="xs xdt err fn">

  <xsl:output method="xhtml" indent="yes"/>

  <!--
  ===================================================================
    Utility XSLT 2.0 functions
  ===================================================================
  -->

  <!-- Map xml parameter element names (types) to a class attribute. -->
  <xsl:function name="ctk:mapTypeToXmlClass">
    <xsl:param name="cliType"/>
    <xsl:choose>
      <xsl:when test="$cliType='boolean'">bool</xsl:when>
      <xsl:when test="$cliType='integer'">number</xsl:when>
      <xsl:when test="$cliType='float'">double</xsl:when>
      <xsl:when test="$cliType=('point', 'region', 'image', 'file', 'directory', 'geometry', 'integer-vector', 'double-vector', 'float-vector', 'string-vector', 'integer-enumeration', 'double-enumeration', 'float-enumeration', 'string-enumeration')">string</xsl:when>
      <xsl:otherwise><xsl:value-of select="$cliType"/></xsl:otherwise>
    </xsl:choose>
  </xsl:function>

  <!-- Map xml parameter element names (types) to the Qt widget property containing
       the current value. The property value type should match the (Qt) C++ type
       (or be convertible to it). -->
  <xsl:function name="ctk:mapTypeToQtValueProperty">
    <xsl:param name="cliType"/>
    <xsl:choose>
      <xsl:when test="$cliType='boolean'">checked</xsl:when>
      <xsl:when test="$cliType= ('point', 'region')">coordinates</xsl:when>
      <xsl:when test="$cliType= ('image', 'file', 'directory', 'geometry')">currentPath</xsl:when>
      <xsl:when test="$cliType= ('string', 'integer-vector', 'float-vector', 'double-vector', 'string-vector')">text</xsl:when>
      <xsl:when test="$cliType= ('integer-enumeration', 'float-enumeration', 'double-enumeration', 'string-enumeration')">currentText</xsl:when>
      <xsl:otherwise>value</xsl:otherwise>
    </xsl:choose>
  </xsl:function>

  <!--
  ===================================================================
    Default templates for suppressing output if no more specific template exists
  ===================================================================
  -->

  <!-- suppress text and attribute nodes not covered in subsequent template rule -->
  <xsl:template match="text()|@*"/>

  <!--
  ===================================================================
    Utility templates
  ===================================================================
  -->

  <xsl:template match="parameters/label">
    <p><xsl:value-of select="text()"/></p>
  </xsl:template>

  <!-- Add a tooltip property to a widget -->
  <xsl:template match="description">
    <property name="toolTip">
      <string><xsl:value-of select="text()"/></string>
    </property>
  </xsl:template>

  <!-- Set the default value by generating a Qt widget specific property which holds
       the current value -->
  <xsl:template match="default">
    <property name="{ctk:mapTypeToQtValueProperty(name(..))}">
      <xsl:element name="{ctk:mapTypeToXmlClass(name(..))}"><xsl:value-of select="text()"/></xsl:element>
    </property>
  </xsl:template>

  <!-- Set Qt widget (spinbox) specific properties for applying constraints of scalar parameters -->
  <xsl:template match="constraints/*[name()=('minimum','maximum')]">
    <property name="{name()}">
      <xsl:element name="{ctk:mapTypeToXmlClass(name(../..))}"><xsl:value-of select="text()"/></xsl:element>
    </property>
  </xsl:template>
  <xsl:template match="constraints/step">
    <property name="singleStep">
      <xsl:element name="{ctk:mapTypeToXmlClass(name(../..))}"><xsl:value-of select="text()"/></xsl:element>
    </property>
    <!-- Also add the 'step' information under the original name -->
    <property name="parameter:step">
      <string><xsl:value-of select="text()"/></string>
    </property>
  </xsl:template>

  <!-- A named template which will be called from each parameter (integer, float, image, etc.) element.
       It assumes that it will be called from an enclosing Qt grid layout element and adds a label item -->
  <xsl:template name="gridItemWithLabel">
    <td><xsl:value-of select="./label"/></td>
  </xsl:template>

  <!-- A named template for adding properties common to all Qt widgets -->
  <xsl:template name="commonWidgetProperties">
    <xsl:apply-templates select="description"/> <!-- tooltip -->
    <xsl:if test="@hidden='true'"> <!-- widget visibility -->
      <property  name="visible">
        <bool>false</bool>
      </property>
    </xsl:if>
    <property name="parameter:valueProperty"> <!-- property name containing current value -->
      <string><xsl:value-of select="ctk:mapTypeToQtValueProperty(name())"/></string>
    </property>

    <!-- add additional (optional) information as properties -->
    <xsl:apply-templates select="default"/>
  </xsl:template>

  <!--
  ===================================================================
    Match elements from the XML description
  ===================================================================
  -->
  <!-- start matching at 'executable' element -->
  <xsl:template match="/executable">
    <xsl:variable name="moduleTitle"><xsl:value-of select="title"/></xsl:variable>
    <html>
      <head>
        <title><xsl:value-of select="title"/></title>
      </head>
      <body>
        <form>
        <div class="executable">
          <!-- This will generate DIV elements with the specific widgets -->
          <xsl:apply-templates select="parameters"/>
        </div>
        </form>
      </body>
    </html>
  </xsl:template>

  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Parameters
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->
  <!-- Match the 'parameters' element and create the parameter groups (QGroupBox) -->
  <xsl:template match="parameters">
    <xsl:variable name="groupLabel"><xsl:value-of select="label"/></xsl:variable>
    <div class="parameters">
      <xsl:apply-templates select="./label"/>
      <xsl:apply-templates select="./description"/>
      <table>
        <xsl:apply-templates select="./description/following-sibling::*"/>
      </table>
    </div>
  </xsl:template>

  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    BOOLEAN parameter (default: QCheckbox)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->

  <xsl:template match="parameters/boolean">
    <tr>
      <xsl:call-template name="gridItemWithLabel"/>
      <td><input type="checkbox" name="{name}"/></td>
    </tr>
  </xsl:template>
  
  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    IMAGE, FILE, GEOMETRY parameter (default: ctkPathLineEdit)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->

  <xsl:template match="parameters/*[name()=('image', 'file', 'geometry')]">
    <tr>
      <xsl:call-template name="gridItemWithLabel"/>
      <td><input type="file" name="{name}"/></td>
    </tr>
  </xsl:template>
  
  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    DEFAULT
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->
  
  <xsl:template match="parameters/*" priority="-1">
    <tr>
      <xsl:call-template name="gridItemWithLabel"/>
      <td>
        <input type="text" name="{name}">
          <xsl:attribute name="value">
            <xsl:value-of select="default/text()"/>
          </xsl:attribute>
        </input>
      </td>
    </tr>
  </xsl:template>

</xsl:stylesheet>
