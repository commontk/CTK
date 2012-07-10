<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:fn="http://www.w3.org/2005/xpath-functions"
  xmlns:xdt="http://www.w3.org/2005/xpath-datatypes"
  xmlns:err="http://www.w3.org/2005/xqt-errors"
  xmlns:ctk="http://www.commontk.org"
  exclude-result-prefixes="xs xdt err fn">

  <xsl:output method="xml" indent="yes"/>

  <!--
  ===================================================================
    Utility XSLT 2.0 functions
  ===================================================================
  -->

  <!-- Map xml parameter element names (types) to the proper QtDesigner UI element. -->
  <xsl:function name="ctk:mapTypeToQtDesigner">
    <xsl:param name="cliType"/>
    <xsl:choose>
      <xsl:when test="$cliType='boolean'">bool</xsl:when>
      <xsl:when test="$cliType='integer'">number</xsl:when>
      <xsl:when test="$cliType='float'">double</xsl:when>
      <xsl:when test="$cliType=('point', 'region', 'image', 'file', 'directory', 'geometry', 'integer-vector', 'double-vector', 'float-vector', 'string-vector', 'integer-enumeration', 'double-enumeration', 'float-enumeration', 'string-enumeration')">string</xsl:when>
      <xsl:otherwise><xsl:value-of select="$cliType"/></xsl:otherwise>
    </xsl:choose>
  </xsl:function>

  <!-- Map xml parameter element names (types) to C++ types. -->
  <xsl:function name="ctk:mapTypeToCpp">
    <xsl:param name="cliType"/>
    <xsl:choose>
      <xsl:when test="$cliType=('integer', 'integer-enumeration')">int</xsl:when>
      <xsl:when test="$cliType='boolean'">bool</xsl:when>
      <xsl:when test="$cliType=('string', 'string-enumeration', 'image', 'file', 'directory', 'geometry')">std::string</xsl:when>
      <xsl:when test="$cliType='integer-vector'">std::vector&lt;int&gt;</xsl:when>
      <xsl:when test="$cliType=('double-vector', 'point', 'region')">std::vector&lt;double&gt;</xsl:when>
      <xsl:when test="$cliType='float-vector'">std::vector&lt;float&gt;</xsl:when>
      <xsl:when test="$cliType='string-vector'">std::vector&lt;std::string&gt;</xsl:when>
      <xsl:when test="$cliType='float-enumeration'">float</xsl:when>
      <xsl:when test="$cliType='double-enumeration'">double</xsl:when>
      <xsl:otherwise><xsl:value-of select="$cliType"/></xsl:otherwise>
    </xsl:choose>
  </xsl:function>

  <!-- Map xml parameter element names (types) to the Qt widget property containing
       the current value. The property type should match the Qt C++ type above
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
  
  <!-- suppress elements not covered in "connections" mode -->
  <xsl:template match="*" mode="connections"/>

  <!-- suppress elements not covered in "signals" mode -->
  <xsl:template match="*" mode="signals"/>

  <!-- This guarantees that elements that don't match the dynamicProperties mode don't get output -->
  <xsl:template match="*" mode="dynamicProperties"/>

  <!--
  ===================================================================
    Templates for generating connections between generated widgets
  ===================================================================
  -->

  <xsl:template match="parameters/*[name()=('image','file','directory','geometry')]" mode="connections">
    <connection>
      <sender><xsl:value-of select="name"/>BrowseButton</sender>
      <signal>clicked()</signal>
      <receiver>parameter:<xsl:value-of select="name"/></receiver>
      <slot>browse()</slot>
    </connection>
  </xsl:template>

  <!--
  ===================================================================
    Utility templates
  ===================================================================
  -->

  <xsl:template match="parameters/label">
    <property name="title">
      <string><xsl:value-of select="text()"/></string>
    </property>
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
      <xsl:element name="{ctk:mapTypeToQtDesigner(name(..))}"><xsl:value-of select="text()"/></xsl:element>
    </property>
  </xsl:template>

  <!-- Set Qt widget (spinbox) specific properties for applying constraints of scalar parameters -->
  <xsl:template match="constraints/*[name()=('minimum','maximum')]">
    <property name="{name()}">
      <xsl:element name="{ctk:mapTypeToQtDesigner(name(../..))}"><xsl:value-of select="text()"/></xsl:element>
    </property>
  </xsl:template>
  <xsl:template match="constraints/step">
    <property name="singleStep">
      <xsl:element name="{ctk:mapTypeToQtDesigner(name(../..))}"><xsl:value-of select="text()"/></xsl:element>
    </property>
    <!-- Also add the 'step' information under the original name -->
    <property name="parameter:step">
      <string><xsl:value-of select="text()"/></string>
    </property>
  </xsl:template>

  <!-- A named template which will be called from each parameter (integer, float, image, etc.) element.
       It assumes that it will be called from an enclosing Qt grid layout element and adds a label item -->
  <xsl:template name="gridItemWithLabel">
    <item  row="{position()-1}" column="0">
      <widget class="QLabel">
        <property name="sizePolicy">
          <sizepolicy hsizetype="Fixed" vsizetype="Preferred">
            <horstretch>0</horstretch>
            <verstretch>0</verstretch>
          </sizepolicy>
        </property>
        <property name="text">
          <string><xsl:value-of select="./label"/></string>
        </property>
      </widget>
    </item>
  </xsl:template>

  <!-- General template for adding dynamic Qt properties to widgets -->
  <xsl:template match="*" mode="dynamicProperty">
    <property  name="parameter:{name()}">
      <string><xsl:value-of select="text()"/></string>
    </property>
  </xsl:template>

  <!-- Exclude certain elements from the set of dynamic properties -->
  <xsl:template match="*[not(name()=('name', 'constraints'))]" mode="dynamicProperties">
    <xsl:apply-templates select="." mode="dynamicProperty"/>
  </xsl:template>

  <!-- A named template for adding properties common to all Qt widgets -->
  <xsl:template name="commonWidgetProperties">
    <xsl:apply-templates select="description"/> <!-- tooltip -->
    <xsl:if test="@hidden='true'"> <!-- widget visibility -->
      <property  name="visible">
        <bool>false</bool>
      </property>
    </xsl:if>
    <property  name="parameter:cppType">
      <string><xsl:value-of select="ctk:mapTypeToCpp(name())"/></string>
    </property>
    <property name="parameter:valueProperty">
      <string><xsl:value-of select="ctk:mapTypeToQtValueProperty(name())"/></string>
    </property>

    <!-- add additional (optional) information as properties -->
    <xsl:apply-templates select="default"/>
    <xsl:apply-templates select="constraints"/>
    <xsl:apply-templates select="./child::*" mode="dynamicProperties"/>
  </xsl:template>
  
  <!-- A named template for creating a QtDesigner stringlist property -->
  <xsl:template name="createQtDesignerStringListProperty">
    <property name="nameFilters">
      <stringlist>
      <xsl:for-each select="tokenize(@fileExtensions, ',')">
        <string><xsl:value-of select="normalize-space(.)"/></string>
      </xsl:for-each>
      </stringlist>
    </property>
  </xsl:template>

  <!--
  ===================================================================
    Match elements from the XML description
  ===================================================================
  -->

  <!-- start matching at 'executable' element -->
  <xsl:template match="/executable">
    <xsl:variable name="moduleTitle"><xsl:value-of select="title"/></xsl:variable>
    <ui version="4.0" >
      <class><xsl:value-of select="translate(normalize-space($moduleTitle), ' ', '')"/></class>
      <widget class="QWidget" name="executable:{normalize-space($moduleTitle)}">
        <layout class="QVBoxLayout">

          <!-- This will generate QGroupBox items with the specific widgets -->
          <xsl:apply-templates select="parameters"/>

          <!-- Add a spacer at the bottom -->
          <item>
            <spacer name="verticalSpacer">
              <property name="orientation">
                <enum>Qt::Vertical</enum>
              </property>
            </spacer>
          </item>

        </layout>
      </widget>

      <connections>
        <xsl:apply-templates mode="connections" select="parameters/*"/>
      </connections>
    </ui>
  </xsl:template>


  <!-- Match the 'parameters' element and create the parameter groups (QGroupBox) -->
  <xsl:template match="parameters">
    <xsl:variable name="groupLabel"><xsl:value-of select="label"/></xsl:variable>
    <item>
      <widget class="ctkCollapsibleGroupBox" name="paramGroup:{$groupLabel}">
        <xsl:apply-templates select="./label"/>
        <xsl:apply-templates select="./description"/>
        <property name="checked">
          <bool><xsl:value-of select="not(@advanced)"></xsl:value-of></bool>
        </property>
        <layout class="QGridLayout">
          <xsl:apply-templates select="./description/following-sibling::*"/>
        </layout>
      </widget>
    </item>
  </xsl:template>
  
  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    BOOLEAN parameter (default: QCheckbox)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->

  <xsl:template match="parameters/boolean">
    <xsl:call-template name="gridItemWithLabel"/>
    <item  row="{position()-1}" column="1">
      <widget class="QCheckBox"  name="parameter:{name}">
        <xsl:call-template name="commonWidgetProperties"/>
        <property name="text">
         <string/>
        </property>
      </widget>
    </item>
  </xsl:template>

  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    INTEGER parameter (default: QSpinBox)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->

  <xsl:template match="parameters/integer">
    <xsl:call-template name="gridItemWithLabel"/>
    <item  row="{position()-1}" column="1">
      <widget class="QSpinBox"  name="parameter:{name}">
        <xsl:if test="not(constraints)">
          <property name="minimum">
            <number>-999999999</number>
          </property>
          <property name="maximum">
            <number>999999999</number>
          </property>
        </xsl:if>
        <xsl:call-template name="commonWidgetProperties"/>
      </widget>
    </item>
  </xsl:template>

  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    DOUBLE, FLOAT parameter (default: QDoubleSpinBox)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->

  <xsl:template match="parameters/*[name()=('double','float')]">
    <xsl:call-template name="gridItemWithLabel"/>
    <item  row="{position()-1}" column="1">
      <widget class="QDoubleSpinBox"  name="parameter:{name}">
        <property name="decimals">
          <number>6</number>
        </property>
        <xsl:if test="not(constraints)">
          <property name="minimum">
            <double>-999999999</double>
          </property>
          <property name="maximum">
            <double>999999999</double>
          </property>
        </xsl:if>
        <xsl:call-template name="commonWidgetProperties"/>
      </widget>
    </item>
  </xsl:template>
  
  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    INTEGER-VECTOR, DOUBLE-VECTOR, FLOAT-VECTOR, STRING-VECTOR parameter (default: QLineEdit)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->

  <xsl:template match="parameters/*[name()=('integer-vector', 'float-vector', 'double-vector', 'string-vector')]">
    <xsl:call-template name="gridItemWithLabel"/>
    <item  row="{position()-1}" column="1">
      <widget class="QLineEdit"  name="parameter:{name}">
        <xsl:call-template name="commonWidgetProperties"/>
      </widget>
    </item>
  </xsl:template>
  
  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    INTEGER-ENUMERATION, DOUBLE-ENUMERATION, FLOAT-ENUMERATION, STRING-ENUMERATION parameter (default: QComboBox)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->

  <xsl:template match="parameters/*[name()=('integer-enumeration', 'float-enumeration', 'double-enumeration', 'string-enumeration')]">
    <xsl:call-template name="gridItemWithLabel"/>
    <item  row="{position()-1}" column="1">
      <widget class="QComboBox"  name="parameter:{name}">
        <xsl:call-template name="commonWidgetProperties"/>
        <xsl:for-each select="element">
          <item>
            <property name="text">
              <string><xsl:value-of select="text()"/></string>
            </property>
          </item>
        </xsl:for-each>
      </widget>
    </item>
  </xsl:template>
  
  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    IMAGE, FILE, GEOMETRY parameter (default: ctkPathLineEdit)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->

  <xsl:template match="parameters/*[name()=('image', 'file', 'geometry')]">
    <xsl:call-template name="gridItemWithLabel"/>
    <item  row="{position()-1}" column="1">
      <layout class="QHBoxLayout">
        <item>
          <widget class="ctkPathLineEdit"  name="parameter:{name}">
            <xsl:call-template name="commonWidgetProperties"/>
            <xsl:call-template name="createQtDesignerStringListProperty"/>
          </widget>
        </item>
        <item>
          <widget class="QPushButton"  name="{name}BrowseButton">
            <property name="text">
              <string>Browse...</string>
            </property>
          </widget>
        </item>
      </layout>
    </item>
  </xsl:template>
  
  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    DIRECTORY parameter (default: ctkPathLineEdit)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->

  <xsl:template match="parameters/directory">
    <xsl:call-template name="gridItemWithLabel"/>
    <item  row="{position()-1}" column="1">
      <layout class="QHBoxLayout">
        <item>
          <widget class="ctkPathLineEdit"  name="parameter:{name}">
            <xsl:call-template name="commonWidgetProperties"/>
            <property name="options">
              <set>ctkPathLineEdit::ShowDirsOnly</set>
            </property>
          </widget>
        </item>
        <item>
          <widget class="QPushButton"  name="{name}BrowseButton">
            <property name="text">
              <string>Browse...</string>
            </property>
          </widget>
        </item>
      </layout>
    </item>
  </xsl:template>
  
  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    POINT, REGION parameter (default: ctkCoordinatesWidget)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->

  <xsl:template match="parameters/*[name()=('point', 'region')]">
    <xsl:call-template name="gridItemWithLabel"/>
    <item  row="{position()-1}" column="1">
      <widget class="ctkCoordinatesWidget"  name="parameter:{name}">
        <xsl:call-template name="commonWidgetProperties"/>
      </widget>
    </item>
  </xsl:template>
  
  <!--
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    NOT IMPLEMENTED YET
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  -->
  
  <xsl:template match="parameters/*" priority="-1">
    <xsl:call-template name="gridItemWithLabel"/>
    <item  row="{position()-1}" column="1">
      <widget class="QLabel"  name="{name}">
        <property name="text">
          <string>&lt;html&gt;&lt;head&gt;&lt;meta name="qrichtext" content="1" /&gt;&lt;style type="text/css"&gt;p, li { white-space: pre-wrap; }&lt;/style&gt;&lt;/head&gt;&lt;body&gt;&lt;p style="margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"&gt;&lt;span style=" color:#ff0000;">Element '<xsl:value-of select="name()"/>' not supported yet.&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
        </property>
        <property name="textFormat">
         <enum>Qt::RichText</enum>
        </property>
      </widget>
    </item>
  </xsl:template>

</xsl:stylesheet>
