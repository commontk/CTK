/*
 * ctkLDAPSearchFilter.h
 *
 *  Created on: Mar 28, 2010
 *      Author: zelzer
 */

#ifndef CTKLDAPSEARCHFILTER_H
#define CTKLDAPSEARCHFILTER_H

#include "CTKCoreExport.h"

#include <QMap>
#include <QString>
#include <QVariant>

namespace ctk {

  class LDAPSearchFilterPrivate;

  class CTK_CORE_EXPORT LDAPSearchFilter {

  public:

    typedef QMap<QString, QVariant> Dictionary;

    LDAPSearchFilter(const QString& filter = "");
    LDAPSearchFilter(const LDAPSearchFilter& filter);

    ~LDAPSearchFilter();

    bool match(const Dictionary& dictionary) const;
    bool matchCase(const Dictionary& dictionary) const;

    bool operator==(const LDAPSearchFilter& other) const;
    LDAPSearchFilter& operator=(const LDAPSearchFilter& filter);

  protected:

    LDAPSearchFilterPrivate * d;

  };

}

#endif // CTKLDAPSEARCHFILTER_H
