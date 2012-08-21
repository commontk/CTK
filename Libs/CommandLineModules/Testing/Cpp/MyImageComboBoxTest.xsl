 <!--
  ============================================================================
    For an input image, we use MyComboBox and no Browse button.
    But for output image, we use standard ctkPathLineEdit and a Browse button.
  ============================================================================
  -->
  <xsl:template match="parameters/*[name()=('image')]" priority="2">
    <xsl:call-template name="gridItemWithLabel"/>
    <item  row="{position()-1}" column="1">
      <layout class="QHBoxLayout">
      <xsl:choose>
        <xsl:when test="channel = 'input'">
          <item>
            <widget class="{$imageInputWidget}"  name="parameter:{name}">
              <xsl:call-template name="commonWidgetProperties"/>
              <xsl:call-template name="createQtDesignerStringListProperty"/>
              <property name="parameter:valueProperty"> <!-- property name containing current value -->
                <string>currentValue</string>
              </property>
            </widget>
          </item>
        </xsl:when>
        <xsl:otherwise>
          <item>
            <widget class="{$imageOutputWidget}"  name="parameter:{name}">
              <xsl:call-template name="commonWidgetProperties"/>
              <xsl:call-template name="createQtDesignerStringListProperty"/>
              <property name="filters">
                <set>ctkPathLineEdit::Files</set>
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
        </xsl:otherwise>
      </xsl:choose>
      </layout>
    </item>
  </xsl:template>

  <!--
  ============================================================================
    For an input image, we use MyComboBox and no Browse button.
    But for output image, we use standard ctkPathLineEdit and a Browse button.
  ============================================================================
  -->

  <xsl:template match="parameters/*[name()=('image')]" mode="connections" priority="2">
    <xsl:if test="channel = 'output'">
      <connection>
        <sender><xsl:value-of select="name"/>BrowseButton</sender>
        <signal>clicked()</signal>
        <receiver>parameter:<xsl:value-of select="name"/></receiver>
        <slot>browse()</slot>
      </connection>
    </xsl:if>
  </xsl:template>

