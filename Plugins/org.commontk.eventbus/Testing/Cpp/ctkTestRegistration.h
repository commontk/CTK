/*
 *  ctkTestRegistration.cpp
 *  ctkTestSuiteEngine
 *
 *  Created by Paolo Quadrani on 17/09/09.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#ifndef CTKTESTREGISTRATION_
#define CTKTESTREGISTRATION_

// Includes list
#include "ctkTestRegistry.h"

namespace ctkQA {

/**
 * A macro to register a test class.
 *
 * This macro will create a static variable which registers the
 * testclass with the TestRegistry, and creates an instance of the
 * test class.
 *
 * Execute this macro in the body of your unit test's .cpp file, e.g.
 *    class MyTest {
 *			...
 *		};
 *
 *		CTK_REGISTER_TEST(MyTest);
 */

#define CTK_REGISTER_TEST(TestClass) \
    static ctkTestRegistration<TestClass> TestClass##Registration

	/**
	 * A wrapper class around a test to manage registration and static
	 * creation of an instance of the test class.
     * This class is used by CTK_REGISTER_TEST(), and you should not
	 * use this class directly.
	 */
    template<typename T>
    class ctkTestRegistration {
		public:
            ///!brief Registration class constructor.
            /** The constructor also register also the test passed as typename into the test suite registry. */
            ctkTestRegistration() {
                m_TestToRegister = new T();
                ctkTestRegistry::instance()->registerTest(m_TestToRegister);
			}

            ///!brief Registration class destructor
            ~ctkTestRegistration() {
                delete m_TestToRegister;
			}
		
		private:
            T* m_TestToRegister; ///< Test to be registered into the test suite.
	};

}

#endif //CTKTESTREGISTRATION_
