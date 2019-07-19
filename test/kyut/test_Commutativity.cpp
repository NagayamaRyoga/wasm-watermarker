#include <kyut/Commutativity.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(kyut)
BOOST_AUTO_TEST_SUITE(commutativity)

BOOST_AUTO_TEST_CASE(get_swapped_predicate) {
    BOOST_CHECK_EQUAL(getSwappedPredicate(wasm::AddInt32), wasm::AddInt32);
    BOOST_CHECK_EQUAL(getSwappedPredicate(wasm::AddInt64), wasm::AddInt64);

    BOOST_CHECK_EQUAL(getSwappedPredicate(wasm::SubInt32), boost::none);

    BOOST_CHECK_EQUAL(getSwappedPredicate(wasm::EqFloat32), wasm::EqFloat32);
    BOOST_CHECK_EQUAL(getSwappedPredicate(wasm::NeFloat32), wasm::NeFloat32);
    BOOST_CHECK_EQUAL(getSwappedPredicate(wasm::LtFloat32), wasm::GtFloat32);
    BOOST_CHECK_EQUAL(getSwappedPredicate(wasm::LeFloat32), wasm::GeFloat32);
    BOOST_CHECK_EQUAL(getSwappedPredicate(wasm::GtFloat32), wasm::LtFloat32);
    BOOST_CHECK_EQUAL(getSwappedPredicate(wasm::GeFloat32), wasm::LeFloat32);
}

BOOST_AUTO_TEST_CASE(is_commutative) {
    BOOST_CHECK_EQUAL(isCommutative(wasm::AddInt32), true);
    BOOST_CHECK_EQUAL(isCommutative(wasm::AddInt64), true);

    BOOST_CHECK_EQUAL(isCommutative(wasm::SubInt32), false);

    BOOST_CHECK_EQUAL(isCommutative(wasm::EqFloat32), true);
    BOOST_CHECK_EQUAL(isCommutative(wasm::NeFloat32), true);
    BOOST_CHECK_EQUAL(isCommutative(wasm::LtFloat32), true);
    BOOST_CHECK_EQUAL(isCommutative(wasm::LeFloat32), true);
    BOOST_CHECK_EQUAL(isCommutative(wasm::GtFloat32), true);
    BOOST_CHECK_EQUAL(isCommutative(wasm::GeFloat32), true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
