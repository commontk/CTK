/*
 *  ctkTestSuite.h
 *  ctkTestSuiteEngine
 *
 *  Created by Paolo Quadrani on 22/09/09.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#ifndef CTKTESTSUITE_
#define CTKTESTSUITE_

// Includes list
#include "ctkTestRegistration.h"
using namespace ctkQA;

#if !defined(EXCLUDE_SINGLETONS)
	#define CTK_TESTSUITE_MAIN() \
	int main(int argc, char *argv[]) \
	{ \
		QCoreApplication app(argc, argv); \
		int result= ctkTestRegistry::instance()->runTests(argc, argv); \
		return result; \
	}
    #define CTK_GUI_TESTSUITE_MAIN() \
    int main(int argc, char *argv[]) \
    { \
        QApplication app(argc, argv); \
        int result= ctkTestRegistry::instance()->runTests(argc, argv); \
        return result; \
    }
#else
	#define CTK_TESTSUITE_MAIN() \
	int main(int argc, char *argv[]) \
	{ \
		QCoreApplication app(argc, argv); \
		int result= ctkTestRegistry::instance()->runTests(argc, argv); \
		return result; \
	}
    #define CTK_GUI_TESTSUITE_MAIN() \
    int main(int argc, char *argv[]) \
    { \
        QApplication app(argc, argv); \
        int result= ctkTestRegistry::instance()->runTests(argc, argv); \
        return result; \
    }
#endif

#endif // CTKTESTSUITE_
