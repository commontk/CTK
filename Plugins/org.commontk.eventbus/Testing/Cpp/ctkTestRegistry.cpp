/*
 *  ctkTestRegistry.cpp
 *  ctkTestSuiteEngine
 *
 *  Created by Paolo Quadrani on 17/09/09.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTestRegistry.h"

using namespace ctkQA;

ctkTestRegistry* ctkTestRegistry::instance() {
    static ctkTestRegistry registry;
	return &registry;
}

void ctkTestRegistry::registerTest(QObject* test) {
    m_TestSuite += test;
}

int ctkTestRegistry::runTests(int argc, char* argv[]) {
	int result = 0;
    foreach(QObject* test, m_TestSuite) {
		result |= QTest::qExec(test, argc, argv);
	}
	return result;
}
