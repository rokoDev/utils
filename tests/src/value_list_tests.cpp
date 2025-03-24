#include <gtest/gtest.h>
#include <utils/value_list.h>

enum class eReaderError : uint8_t
{
    kError1,
    kError2,
};

enum class eWriterError : uint8_t
{
    kError3,
    kError4,
    kError5,
};

enum class eFileError : uint8_t
{
    kOpenError,
    kEOF,
    kAccessDenied,
    kPermission,
    kBusyDescriptor,
    kFileNotFound,
    kSystemError
};

TEST(UtilsValueList, Size)
{
    static_assert(utils::value_list<>::size == 0);
    static_assert(utils::value_list<eFileError::kSystemError>::size == 1);
    static_assert(
        utils::value_list<eFileError::kEOF, 10, eReaderError::kError2>::size ==
        3);
}

TEST(UtilsValueList, At)
{
    {
        using value_list = utils::value_list<eFileError::kSystemError>;

        static_assert(value_list::at<0> == eFileError::kSystemError);
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2>;

        static_assert(value_list::at<0> == eFileError::kEOF);
        static_assert(value_list::at<1> == 10);
        static_assert(value_list::at<2> == eReaderError::kError2);
    }

    {
        static constexpr int i = 5;
        constexpr const int &ri = i;

        using list = utils::value_list<'t', ri, i, 10, eFileError::kPermission>;

        static_assert(std::is_same_v<decltype(list::at<0>), const char>);
        static_assert(std::is_same_v<decltype(list::at<1>), const int &>);
        static_assert(std::is_same_v<decltype(list::at<2>), const int>);
        static_assert(std::is_same_v<decltype(list::at<3>), const int>);
        static_assert(std::is_same_v<decltype(list::at<4>), const eFileError>);
        static_assert(list::at<0> == 't');
        static_assert(list::at<1> == 5);
        static_assert(list::at<2> == 5);
        static_assert(list::at<3> == 10);
        static_assert(list::at<4> == eFileError::kPermission);
        static_assert(std::is_same_v<list::at_t<0>, char>);
        static_assert(std::is_same_v<list::at_t<1>, const int &>);
        static_assert(std::is_same_v<list::at_t<2>, int>);
        static_assert(std::is_same_v<list::at_t<3>, int>);
        static_assert(std::is_same_v<list::at_t<4>, eFileError>);
    }
}

TEST(UtilsValueList, IndexOf)
{
    {
        using value_list = utils::value_list<>;

        static_assert(value_list::index_of<eFileError::kEOF> ==
                      value_list::size);
        static_assert(value_list::index_of<10> == value_list::size);
    }

    {
        using value_list = utils::value_list<eFileError::kSystemError>;

        static_assert(value_list::index_of<eFileError::kSystemError> == 0);
        static_assert(value_list::index_of<1> == value_list::size);
        static_assert(value_list::index_of<eFileError::kPermission> ==
                      value_list::size);
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 8, eReaderError::kError2,
                              eFileError::kOpenError, eWriterError::kError3,
                              eFileError::kEOF>;

        static_assert(value_list::index_of<eFileError::kEOF> == 0);
        static_assert(value_list::index_of<8> == 1);
        static_assert(value_list::index_of<eReaderError::kError2> == 2);
        static_assert(value_list::index_of<eFileError::kOpenError> == 3);
        static_assert(value_list::index_of<eWriterError::kError3> == 4);
        static_assert(value_list::index_of<0> == value_list::size);
    }
}

TEST(UtilsValueList, IsSame)
{
    {
        using value_list = utils::value_list<>;

        static_assert(value_list::is_same_v);
    }

    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(value_list::is_same_v);
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 0, eReaderError::kError1,
                              eFileError::kPermission>;

        static_assert(not value_list::is_same_v);
    }

    {
        using value_list = utils::value_list<0, 0, 0, 0, 0, 0>;

        static_assert(value_list::is_same_v);
    }
}

TEST(UtilsValueList, CountOf)
{
    {
        using value_list = utils::value_list<>;

        static_assert(value_list::count_of<eFileError::kPermission> == 0);
        static_assert(value_list::count_of<1> == 0);
        static_assert(value_list::count_of<eReaderError::kError2> == 0);
    }

    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(value_list::count_of<eFileError::kPermission> == 1);
        static_assert(value_list::count_of<0> == 0);
        static_assert(value_list::count_of<eReaderError::kError1> == 0);
    }

    {
        using value_list =
            utils::value_list<eFileError::kPermission, 10,
                              eReaderError::kError2, eFileError::kPermission,
                              eWriterError::kError4, eFileError::kOpenError>;

        static_assert(value_list::count_of<eFileError::kPermission> == 2);
        static_assert(value_list::count_of<10> == 1);
        static_assert(value_list::count_of<eReaderError::kError2> == 1);
        static_assert(value_list::count_of<eWriterError::kError4> == 1);
        static_assert(value_list::count_of<eFileError::kOpenError> == 1);
        static_assert(value_list::count_of<static_cast<int8_t>(3)> == 0);
    }
}

TEST(UtilsValueList, Contains)
{
    {
        using value_list = utils::value_list<>;

        static_assert(not value_list::contains_v<0>);
        static_assert(not value_list::contains_v<eFileError::kEOF>);
        static_assert(not value_list::contains_v<static_cast<int8_t>(1)>);
    }

    {
        using value_list = utils::value_list<eFileError::kAccessDenied>;

        static_assert(value_list::contains_v<eFileError::kAccessDenied>);
        static_assert(not value_list::contains_v<10>);
        static_assert(not value_list::contains_v<eReaderError::kError2>);
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError1,
                              eFileError::kOpenError, eWriterError::kError3,
                              eFileError::kOpenError>;

        static_assert(value_list::contains_v<eFileError::kEOF>);
        static_assert(value_list::contains_v<10>);
        static_assert(value_list::contains_v<eReaderError::kError1>);
        static_assert(value_list::contains_v<eFileError::kOpenError>);
        static_assert(value_list::contains_v<eWriterError::kError3>);
        static_assert(not value_list::contains_v<static_cast<int8_t>(15)>);
        static_assert(not value_list::contains_v<1>);
    }
}

TEST(UtilsValueList, ContainsCopies)
{
    {
        using value_list = utils::value_list<>;

        static_assert(not value_list::contains_copies);
    }

    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(not value_list::contains_copies);
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eFileError::kEOF>;

        static_assert(value_list::contains_copies);
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eFileError::kSystemError>;

        static_assert(not value_list::contains_copies);
    }
}

TEST(UtilsValueList, ContainsType)
{
    {
        using value_list = utils::value_list<>;

        static_assert(not value_list::contains_type<eFileError>);
        static_assert(not value_list::contains_type<int>);
        static_assert(not value_list::contains_type<eReaderError>);
    }

    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(value_list::contains_type<eFileError>);
        static_assert(not value_list::contains_type<int>);
        static_assert(not value_list::contains_type<eReaderError>);
    }

    {
        using value_list =
            utils::value_list<eFileError::kPermission, 10,
                              eReaderError::kError2, eFileError::kPermission,
                              eWriterError::kError4, eFileError::kOpenError>;

        static_assert(value_list::contains_type<eFileError>);
        static_assert(value_list::contains_type<int>);
        static_assert(value_list::contains_type<eReaderError>);
        static_assert(value_list::contains_type<eWriterError>);
        static_assert(not value_list::contains_type<uint8_t>);
    }
}

TEST(UtilsValueList, MinValueOfTypeT)
{
    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(value_list::min<eFileError> == eFileError::kPermission);
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eReaderError::kError1, eFileError::kEOF>;

        static_assert(value_list::min<eFileError> == eFileError::kEOF);
        static_assert(value_list::min<int> == 10);
        static_assert(value_list::min<eReaderError> == eReaderError::kError1);
    }

    {
        using value_list =
            utils::value_list<eFileError::kSystemError, 10,
                              eReaderError::kError2, eFileError::kPermission,
                              eWriterError::kError5, eFileError::kEOF>;

        static_assert(value_list::min<eFileError> == eFileError::kEOF);
    }
}

TEST(UtilsValueList, MaxValueOfTypeT)
{
    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(value_list::max<eFileError> == eFileError::kPermission);
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eReaderError::kError1, eFileError::kEOF>;

        static_assert(value_list::max<eFileError> == eFileError::kPermission);
        static_assert(value_list::max<int> == 10);
        static_assert(value_list::max<eReaderError> == eReaderError::kError2);
        static_assert(value_list::max<eWriterError> == eWriterError::kError5);
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eFileError::kSystemError>;

        static_assert(value_list::max<eFileError> == eFileError::kSystemError);
    }
}

TEST(UtilsValueList, AllOfTypeT)
{
    {
        using empty_list = utils::value_list<>;

        static_assert(
            std::is_same_v<empty_list::list_all_of_t<eFileError>, empty_list>);
        static_assert(
            std::is_same_v<empty_list::list_all_of_t<int>, empty_list>);
    }

    {
        using value_list = utils::value_list<eFileError::kPermission>;
        using empty_list = utils::value_list<>;

        static_assert(
            std::is_same_v<value_list::list_all_of_t<eFileError>, value_list>);
        static_assert(
            std::is_same_v<value_list::list_all_of_t<int>, empty_list>);
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eReaderError::kError1, eFileError::kEOF>;

        static_assert(
            std::is_same_v<
                value_list::list_all_of_t<eFileError>,
                utils::value_list<eFileError::kEOF, eFileError::kPermission,
                                  eFileError::kEOF>>);

        static_assert(std::is_same_v<value_list::list_all_of_t<eReaderError>,
                                     utils::value_list<eReaderError::kError2,
                                                       eReaderError::kError1>>);
        static_assert(std::is_same_v<value_list::list_all_of_t<eWriterError>,
                                     utils::value_list<eWriterError::kError5>>);
        static_assert(std::is_same_v<value_list::list_all_of_t<float>,
                                     utils::value_list<>>);
    }
}

TEST(UtilsValueList, Concatenate)
{
    using utils::concatenate_t;
    using utils::value_list;

    {
        using list = value_list<>;
        using concatenate_result = concatenate_t<list>;
        static_assert(std::is_same_v<list, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list = value_list<111>;
        using concatenate_result = concatenate_t<list>;
        static_assert(std::is_same_v<list, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = value_list<>;
        using list2 = value_list<>;
        using concatenate_result = concatenate_t<list1, list2>;
        static_assert(std::is_same_v<value_list<>, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = value_list<>;
        using list2 = value_list<111>;
        using concatenate_result = concatenate_t<list1, list2>;
        static_assert(std::is_same_v<value_list<111>, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = value_list<111>;
        using list2 = value_list<222>;
        using concatenate_result = concatenate_t<list1, list2>;
        static_assert(std::is_same_v<value_list<111, 222>, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = value_list<2, eReaderError::kError2, uint8_t{3}>;
        using list2 = value_list<eWriterError::kError4,
                                 eFileError::kAccessDenied, int16_t{}>;
        using concatenate_result = concatenate_t<list1, list2>;
        static_assert(
            std::is_same_v<value_list<2, eReaderError::kError2, uint8_t{3},
                                      eWriterError::kError4,
                                      eFileError::kAccessDenied, int16_t{}>,
                           concatenate_result>,
            "invalid result of concatenation");
    }

    {
        using list1 = value_list<111, eReaderError::kError2, uint8_t{3}>;
        using list2 = value_list<eWriterError::kError4,
                                 eFileError::kAccessDenied, int16_t{}>;
        using list3 = value_list<'t', 333>;

        using concatenate_result = concatenate_t<list1, list2, list3>;
        static_assert(
            std::is_same_v<
                value_list<111, eReaderError::kError2, uint8_t{3},
                           eWriterError::kError4, eFileError::kAccessDenied,
                           int16_t{}, 't', 333>,
                concatenate_result>,
            "invalid result of concatenation");
    }
}
