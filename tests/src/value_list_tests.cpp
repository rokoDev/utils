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
    static_assert(utils::value_list<>::size == 0, "invalid size");
    static_assert(utils::value_list<eFileError::kSystemError>::size == 1,
                  "invalid size");
    static_assert(
        utils::value_list<eFileError::kEOF, 10, eReaderError::kError2>::size ==
            3,
        "invalid size");
}

TEST(UtilsValueList, At)
{
    {
        using value_list = utils::value_list<eFileError::kSystemError>;

        static_assert(value_list::at<0> == eFileError::kSystemError,
                      "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2>;

        static_assert(value_list::at<0> == eFileError::kEOF, "invalid value");
        static_assert(value_list::at<1> == 10, "invalid value");
        static_assert(value_list::at<2> == eReaderError::kError2,
                      "invalid value");
    }
}

TEST(UtilsValueList, IndexOf)
{
    {
        using value_list = utils::value_list<>;

        static_assert(
            value_list::index_of<eFileError::kEOF> == value_list::size,
            "invalid index");
        static_assert(value_list::index_of<10> == value_list::size,
                      "invalid index");
    }

    {
        using value_list = utils::value_list<eFileError::kSystemError>;

        static_assert(value_list::index_of<eFileError::kSystemError> == 0,
                      "invalid index");
        static_assert(value_list::index_of<1> == value_list::size,
                      "invalid index");
        static_assert(
            value_list::index_of<eFileError::kPermission> == value_list::size,
            "invalid index");
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 8, eReaderError::kError2,
                              eFileError::kOpenError, eWriterError::kError3,
                              eFileError::kEOF>;

        static_assert(value_list::index_of<eFileError::kEOF> == 0,
                      "invalid index");
        static_assert(value_list::index_of<8> == 1, "invalid index");
        static_assert(value_list::index_of<eReaderError::kError2> == 2,
                      "invalid index");
        static_assert(value_list::index_of<eFileError::kOpenError> == 3,
                      "invalid index");
        static_assert(value_list::index_of<eWriterError::kError3> == 4,
                      "invalid index");
        static_assert(value_list::index_of<0> == value_list::size,
                      "invalid index");
    }
}

TEST(UtilsValueList, IsSame)
{
    {
        using value_list = utils::value_list<>;

        static_assert(value_list::is_same_v, "invalid value");
    }

    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(value_list::is_same_v, "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 0, eReaderError::kError1,
                              eFileError::kPermission>;

        static_assert(not value_list::is_same_v, "invalid value");
    }

    {
        using value_list = utils::value_list<0, 0, 0, 0, 0, 0>;

        static_assert(value_list::is_same_v, "invalid value");
    }
}

TEST(UtilsValueList, CountOf)
{
    {
        using value_list = utils::value_list<>;

        static_assert(value_list::count_of<eFileError::kPermission> == 0,
                      "invalid value");
        static_assert(value_list::count_of<1> == 0, "invalid value");
        static_assert(value_list::count_of<eReaderError::kError2> == 0,
                      "invalid value");
    }

    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(value_list::count_of<eFileError::kPermission> == 1,
                      "invalid value");
        static_assert(value_list::count_of<0> == 0, "invalid value");
        static_assert(value_list::count_of<eReaderError::kError1> == 0,
                      "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kPermission, 10,
                              eReaderError::kError2, eFileError::kPermission,
                              eWriterError::kError4, eFileError::kOpenError>;

        static_assert(value_list::count_of<eFileError::kPermission> == 2,
                      "invalid value");
        static_assert(value_list::count_of<10> == 1, "invalid value");
        static_assert(value_list::count_of<eReaderError::kError2> == 1,
                      "invalid value");
        static_assert(value_list::count_of<eWriterError::kError4> == 1,
                      "invalid value");
        static_assert(value_list::count_of<eFileError::kOpenError> == 1,
                      "invalid value");
        static_assert(value_list::count_of<static_cast<int8_t>(3)> == 0,
                      "invalid value");
    }
}

TEST(UtilsValueList, Contains)
{
    {
        using value_list = utils::value_list<>;

        static_assert(not value_list::contains_v<0>, "invalid value");
        static_assert(not value_list::contains_v<eFileError::kEOF>,
                      "invalid value");
        static_assert(not value_list::contains_v<static_cast<int8_t>(1)>,
                      "invalid value");
    }

    {
        using value_list = utils::value_list<eFileError::kAccessDenied>;

        static_assert(value_list::contains_v<eFileError::kAccessDenied>,
                      "invalid value");
        static_assert(not value_list::contains_v<10>, "invalid value");
        static_assert(not value_list::contains_v<eReaderError::kError2>,
                      "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError1,
                              eFileError::kOpenError, eWriterError::kError3,
                              eFileError::kOpenError>;

        static_assert(value_list::contains_v<eFileError::kEOF>,
                      "invalid value");
        static_assert(value_list::contains_v<10>, "invalid value");
        static_assert(value_list::contains_v<eReaderError::kError1>,
                      "invalid value");
        static_assert(value_list::contains_v<eFileError::kOpenError>,
                      "invalid value");
        static_assert(value_list::contains_v<eWriterError::kError3>,
                      "invalid value");
        static_assert(not value_list::contains_v<static_cast<int8_t>(15)>,
                      "invalid value");
        static_assert(not value_list::contains_v<1>, "invalid value");
    }
}

TEST(UtilsValueList, ContainsCopies)
{
    {
        using value_list = utils::value_list<>;

        static_assert(not value_list::contains_copies, "invalid value");
    }

    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(not value_list::contains_copies, "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eFileError::kEOF>;

        static_assert(value_list::contains_copies, "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eFileError::kSystemError>;

        static_assert(not value_list::contains_copies, "invalid value");
    }
}

TEST(UtilsValueList, ContainsType)
{
    {
        using value_list = utils::value_list<>;

        static_assert(not value_list::contains_type<eFileError>,
                      "invalid value");
        static_assert(not value_list::contains_type<int>, "invalid value");
        static_assert(not value_list::contains_type<eReaderError>,
                      "invalid value");
    }

    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(value_list::contains_type<eFileError>, "invalid value");
        static_assert(not value_list::contains_type<int>, "invalid value");
        static_assert(not value_list::contains_type<eReaderError>,
                      "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kPermission, 10,
                              eReaderError::kError2, eFileError::kPermission,
                              eWriterError::kError4, eFileError::kOpenError>;

        static_assert(value_list::contains_type<eFileError>, "invalid value");
        static_assert(value_list::contains_type<int>, "invalid value");
        static_assert(value_list::contains_type<eReaderError>, "invalid value");
        static_assert(value_list::contains_type<eWriterError>, "invalid value");
        static_assert(not value_list::contains_type<uint8_t>, "invalid value");
    }
}

TEST(UtilsValueList, MinValueOfTypeT)
{
    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(value_list::min<eFileError> == eFileError::kPermission,
                      "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eReaderError::kError1, eFileError::kEOF>;

        static_assert(value_list::min<eFileError> == eFileError::kEOF,
                      "invalid value");
        static_assert(value_list::min<int> == 10, "invalid value");
        static_assert(value_list::min<eReaderError> == eReaderError::kError1,
                      "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kSystemError, 10,
                              eReaderError::kError2, eFileError::kPermission,
                              eWriterError::kError5, eFileError::kEOF>;

        static_assert(value_list::min<eFileError> == eFileError::kEOF,
                      "invalid value");
    }
}

TEST(UtilsValueList, MaxValueOfTypeT)
{
    {
        using value_list = utils::value_list<eFileError::kPermission>;

        static_assert(value_list::max<eFileError> == eFileError::kPermission,
                      "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eReaderError::kError1, eFileError::kEOF>;

        static_assert(value_list::max<eFileError> == eFileError::kPermission,
                      "invalid value");
        static_assert(value_list::max<int> == 10, "invalid value");
        static_assert(value_list::max<eReaderError> == eReaderError::kError2,
                      "invalid value");
        static_assert(value_list::max<eWriterError> == eWriterError::kError5,
                      "invalid value");
    }

    {
        using value_list =
            utils::value_list<eFileError::kEOF, 10, eReaderError::kError2,
                              eFileError::kPermission, eWriterError::kError5,
                              eFileError::kSystemError>;

        static_assert(value_list::max<eFileError> == eFileError::kSystemError,
                      "invalid value");
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
