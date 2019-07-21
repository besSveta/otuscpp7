/*
 * nickname_test.cpp
 *
 *  Created on: 22 июл. 2019 г.
 *      Author: sveta
 */
#define BOOST_TEST_MODULE ip_filter_test_module
#include <boost/test/included/unit_test.hpp>
#include "nickname.h"

BOOST_AUTO_TEST_SUITE(nickname_test_suite)

BOOST_AUTO_TEST_CASE(nickname_test_case)
{

	RadixTree tr;
	tr.insert("aleksey");
	tr.insert("sasha");
	tr.insert("aleks");
	tr.insert("alek");
	tr.insert("alesha");
	BOOST_REQUIRE_EQUAL(tr.getParent("alesha"),"ale");
}



BOOST_AUTO_TEST_SUITE_END()



