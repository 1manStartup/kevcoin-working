#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-kev");
    BOOST_CHECK(GetBoolArg("-kev"));
    BOOST_CHECK(GetBoolArg("-kev", false));
    BOOST_CHECK(GetBoolArg("-kev", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-kevo"));
    BOOST_CHECK(!GetBoolArg("-kevo", false));
    BOOST_CHECK(GetBoolArg("-kevo", true));

    ResetArgs("-kev=0");
    BOOST_CHECK(!GetBoolArg("-kev"));
    BOOST_CHECK(!GetBoolArg("-kev", false));
    BOOST_CHECK(!GetBoolArg("-kev", true));

    ResetArgs("-kev=1");
    BOOST_CHECK(GetBoolArg("-kev"));
    BOOST_CHECK(GetBoolArg("-kev", false));
    BOOST_CHECK(GetBoolArg("-kev", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nokev");
    BOOST_CHECK(!GetBoolArg("-kev"));
    BOOST_CHECK(!GetBoolArg("-kev", false));
    BOOST_CHECK(!GetBoolArg("-kev", true));

    ResetArgs("-nokev=1");
    BOOST_CHECK(!GetBoolArg("-kev"));
    BOOST_CHECK(!GetBoolArg("-kev", false));
    BOOST_CHECK(!GetBoolArg("-kev", true));

    ResetArgs("-kev -nokev");  // -kev should win
    BOOST_CHECK(GetBoolArg("-kev"));
    BOOST_CHECK(GetBoolArg("-kev", false));
    BOOST_CHECK(GetBoolArg("-kev", true));

    ResetArgs("-kev=1 -nokev=1");  // -kev should win
    BOOST_CHECK(GetBoolArg("-kev"));
    BOOST_CHECK(GetBoolArg("-kev", false));
    BOOST_CHECK(GetBoolArg("-kev", true));

    ResetArgs("-kev=0 -nokev=0");  // -kev should win
    BOOST_CHECK(!GetBoolArg("-kev"));
    BOOST_CHECK(!GetBoolArg("-kev", false));
    BOOST_CHECK(!GetBoolArg("-kev", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--kev=1");
    BOOST_CHECK(GetBoolArg("-kev"));
    BOOST_CHECK(GetBoolArg("-kev", false));
    BOOST_CHECK(GetBoolArg("-kev", true));

    ResetArgs("--nokev=1");
    BOOST_CHECK(!GetBoolArg("-kev"));
    BOOST_CHECK(!GetBoolArg("-kev", false));
    BOOST_CHECK(!GetBoolArg("-kev", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-kev", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-kev", "eleven"), "eleven");

    ResetArgs("-kev -bar");
    BOOST_CHECK_EQUAL(GetArg("-kev", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-kev", "eleven"), "");

    ResetArgs("-kev=");
    BOOST_CHECK_EQUAL(GetArg("-kev", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-kev", "eleven"), "");

    ResetArgs("-kev=11");
    BOOST_CHECK_EQUAL(GetArg("-kev", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-kev", "eleven"), "11");

    ResetArgs("-kev=eleven");
    BOOST_CHECK_EQUAL(GetArg("-kev", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-kev", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-kev", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-kev", 0), 0);

    ResetArgs("-kev -bar");
    BOOST_CHECK_EQUAL(GetArg("-kev", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-kev=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-kev", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-kev=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-kev", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--kev");
    BOOST_CHECK_EQUAL(GetBoolArg("-kev"), true);

    ResetArgs("--kev=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-kev", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nokev");
    BOOST_CHECK(!GetBoolArg("-kev"));
    BOOST_CHECK(!GetBoolArg("-kev", true));
    BOOST_CHECK(!GetBoolArg("-kev", false));

    ResetArgs("-nokev=1");
    BOOST_CHECK(!GetBoolArg("-kev"));
    BOOST_CHECK(!GetBoolArg("-kev", true));
    BOOST_CHECK(!GetBoolArg("-kev", false));

    ResetArgs("-nokev=0");
    BOOST_CHECK(GetBoolArg("-kev"));
    BOOST_CHECK(GetBoolArg("-kev", true));
    BOOST_CHECK(GetBoolArg("-kev", false));

    ResetArgs("-kev --nokev");
    BOOST_CHECK(GetBoolArg("-kev"));

    ResetArgs("-nokev -kev"); // kev always wins:
    BOOST_CHECK(GetBoolArg("-kev"));
}

BOOST_AUTO_TEST_SUITE_END()
