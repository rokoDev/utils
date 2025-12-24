#include <gtest/gtest.h>
#include <type_name/type_name.h>
#include <utils/type_list.h>

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

TEST(UtilsTypeList, Size)
{
    static_assert(utils::type_list<>::size == 0, "invalid size");
    static_assert(utils::type_list<eFileError>::size == 1, "invalid size");
    static_assert(
        utils::type_list<eFileError, int32_t, eReaderError>::size == 3,
        "invalid size");
}

template <typename T>
struct is_file_error : std::bool_constant<std::is_same_v<T, eFileError>>
{
};

TEST(UtilsTypeList, At)
{
    using type_list = utils::type_list<eFileError, int32_t, eReaderError, float,
                                       eFileError, eWriterError>;

    static_assert(std::is_same_v<type_list::at<0>, eFileError>, "invalid type");
    static_assert(std::is_same_v<type_list::at<1>, int32_t>, "invalid type");
    static_assert(std::is_same_v<type_list::at<2>, eReaderError>,
                  "invalid type");
    static_assert(std::is_same_v<type_list::at<3>, float>, "invalid type");
    static_assert(std::is_same_v<type_list::at<4>, eFileError>, "invalid type");
    static_assert(std::is_same_v<type_list::at<5>, eWriterError>,
                  "invalid type");

    static_assert(std::is_same_v<type_list::at<0, is_file_error>, eFileError>,
                  "invalid type");
    static_assert(std::is_same_v<type_list::at<1, is_file_error>, eFileError>,
                  "invalid type");

    static_assert(std::is_same_v<type_list::at<0, std::is_enum>, eFileError>,
                  "invalid type");
    static_assert(std::is_same_v<type_list::at<1, std::is_enum>, eReaderError>,
                  "invalid type");
    static_assert(std::is_same_v<type_list::at<2, std::is_enum>, eFileError>,
                  "invalid type");
    static_assert(std::is_same_v<type_list::at<3, std::is_enum>, eWriterError>,
                  "invalid type");
}

TEST(UtilsTypeList, IndexOf)
{
    {
        using type_list = utils::type_list<>;

        static_assert(type_list::index_of_v<std::is_enum> == type_list::size,
                      "invalid index");
        static_assert(type_list::index_of_v<std::is_signed> == type_list::size,
                      "invalid index");
        static_assert(type_list::index_of_v<std::is_void> == type_list::size,
                      "invalid index");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(type_list::index_of_v<std::is_enum> == 0,
                      "invalid index");
        static_assert(type_list::index_of_v<std::is_signed> == type_list::size,
                      "invalid index");
        static_assert(type_list::index_of_v<std::is_void> == type_list::size,
                      "invalid index");
    }

    {
        using type_list = utils::type_list<eFileError, int32_t, eReaderError,
                                           float, eFileError, eWriterError>;

        static_assert(type_list::index_of_v<std::is_signed> == 1,
                      "invalid index");
        static_assert(type_list::index_of_v<std::is_floating_point> == 3,
                      "invalid index");
        static_assert(type_list::index_of_v<std::is_enum> == 0,
                      "invalid index");
        static_assert(type_list::index_of_v<std::is_void> == type_list::size,
                      "invalid index");
    }
}

TEST(UtilsTypeList, FirstIndexOfType)
{
    {
        using type_list = utils::type_list<>;

        static_assert(type_list::first_index_of_type<float> == type_list::size,
                      "invalid index");
        static_assert(type_list::first_index_of_type<int> == type_list::size,
                      "invalid index");
        static_assert(
            type_list::first_index_of_type<eFileError> == type_list::size,
            "invalid index");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(type_list::first_index_of_type<eFileError> == 0,
                      "invalid index");
        static_assert(type_list::first_index_of_type<int> == type_list::size,
                      "invalid index");
        static_assert(type_list::first_index_of_type<char> == type_list::size,
                      "invalid index");
    }

    {
        using type_list = utils::type_list<eFileError, int32_t, eReaderError,
                                           float, eFileError, eWriterError>;

        static_assert(type_list::first_index_of_type<eFileError> == 0,
                      "invalid index");
        static_assert(type_list::first_index_of_type<int32_t> == 1,
                      "invalid index");
        static_assert(type_list::first_index_of_type<eReaderError> == 2,
                      "invalid index");
        static_assert(type_list::first_index_of_type<float> == 3,
                      "invalid index");
        static_assert(type_list::first_index_of_type<eWriterError> == 5,
                      "invalid index");
        static_assert(
            type_list::first_index_of_type<uint16_t> == type_list::size,
            "invalid index");
    }
}

TEST(UtilsTypeList, IsSame)
{
    {
        using type_list = utils::type_list<>;

        static_assert(type_list::is_same, "invalid value");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(type_list::is_same, "invalid value");
    }

    {
        using type_list = utils::type_list<eFileError, int32_t, eReaderError,
                                           float, eFileError, eWriterError>;

        static_assert(not type_list::is_same, "invalid value");
    }

    {
        using type_list = utils::type_list<int32_t, int32_t, int32_t, int32_t,
                                           int32_t, int32_t>;

        static_assert(type_list::is_same, "invalid value");
    }
}

TEST(UtilsTypeList, CountOf)
{
    {
        using type_list = utils::type_list<>;

        static_assert(type_list::count_of<eFileError> == 0, "invalid value");
        static_assert(type_list::count_of<int32_t> == 0, "invalid value");
        static_assert(type_list::count_of<eReaderError> == 0, "invalid value");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(type_list::count_of<eFileError> == 1, "invalid value");
        static_assert(type_list::count_of<int32_t> == 0, "invalid value");
        static_assert(type_list::count_of<eReaderError> == 0, "invalid value");
    }

    {
        using type_list =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, eFileError>;

        static_assert(type_list::count_of<eFileError> == 3, "invalid value");
        static_assert(type_list::count_of<int32_t> == 1, "invalid value");
        static_assert(type_list::count_of<eReaderError> == 1, "invalid value");
        static_assert(type_list::count_of<float> == 1, "invalid value");
        static_assert(type_list::count_of<eWriterError> == 1, "invalid value");
        static_assert(type_list::count_of<double> == 0, "invalid value");
    }
}

TEST(UtilsTypeList, CountOfPredicateCompliant)
{
    {
        using type_list = utils::type_list<>;

        static_assert(
            type_list::count_of_predicate_compliant_v<std::is_enum> == 0,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant_v<std::is_integral> == 0,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant_v<std::is_floating_point> ==
                0,
            "invalid value");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(
            type_list::count_of_predicate_compliant_v<std::is_enum> == 1,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant_v<std::is_integral> == 0,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant_v<std::is_floating_point> ==
                0,
            "invalid value");
    }

    {
        using type_list =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, double, eFileError>;

        static_assert(
            type_list::count_of_predicate_compliant_v<std::is_enum> == 5,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant_v<std::is_floating_point> ==
                2,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant_v<std::is_integral> == 1,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant_v<std::is_void> == 0,
            "invalid value");
    }
}

TEST(UtilsTypeList, Contains)
{
    {
        using type_list = utils::type_list<>;

        static_assert(not type_list::contains_v<void>, "invalid value");
        static_assert(not type_list::contains_v<int32_t>, "invalid value");
        static_assert(not type_list::contains_v<eReaderError>, "invalid value");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(type_list::contains_v<eFileError>, "invalid value");
        static_assert(not type_list::contains_v<int32_t>, "invalid value");
        static_assert(not type_list::contains_v<eReaderError>, "invalid value");
    }

    {
        using type_list =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, double, eFileError>;

        static_assert(type_list::contains_v<eFileError>, "invalid value");
        static_assert(type_list::contains_v<int32_t>, "invalid value");
        static_assert(type_list::contains_v<eReaderError>, "invalid value");
        static_assert(type_list::contains_v<float>, "invalid value");
        static_assert(type_list::contains_v<eWriterError>, "invalid value");
        static_assert(type_list::contains_v<double>, "invalid value");
        static_assert(not type_list::contains_v<uint8_t>, "invalid value");
        static_assert(not type_list::contains_v<void>, "invalid value");
    }

    {
        using type_list1 = utils::type_list<>;
        using type_list2 = utils::type_list<>;

        static_assert(type_list1::contains_v<type_list2>);
    }

    {
        using type_list1 =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, double, eFileError>;

        using type_list2 = utils::type_list<>;

        static_assert(type_list1::contains_v<type_list2>);
        static_assert(not type_list2::contains_v<type_list1>);
    }

    {
        using type_list1 =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, double, eFileError>;

        using type_list2 = utils::type_list<int32_t, eWriterError>;

        static_assert(type_list1::contains_v<type_list2>);
    }

    {
        using type_list1 =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, double, eFileError>;

        using type_list2 = utils::type_list<int32_t, eWriterError, bool>;

        static_assert(not type_list1::contains_v<type_list2>);
    }

    {
        using type_list1 =
            utils::type_list<eFileError, int32_t, eReaderError, float>;
        using type_list2 =
            utils::type_list<eFileError, int32_t, eReaderError, float>;

        static_assert(type_list2::contains_v<type_list1>);
    }

    {
        using type_list1 =
            utils::type_list<eFileError, int32_t, eReaderError, float>;
        using type_list2 = utils::type_list<float, eFileError, int32_t,
                                            eReaderError, float, eReaderError>;

        static_assert(type_list1::contains_v<type_list2>);
    }
}

TEST(UtilsTypeList, ContainsPredicateCompliant)
{
    {
        using type_list = utils::type_list<>;

        static_assert(
            not type_list::contains_predicate_compliant_v<std::is_enum>,
            "invalid value");
        static_assert(
            not type_list::contains_predicate_compliant_v<std::is_integral>,
            "invalid value");
        static_assert(not type_list::contains_predicate_compliant_v<
                          std::is_floating_point>,
                      "invalid value");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(type_list::contains_predicate_compliant_v<std::is_enum>,
                      "invalid value");
        static_assert(
            not type_list::contains_predicate_compliant_v<std::is_integral>,
            "invalid value");
        static_assert(not type_list::contains_predicate_compliant_v<
                          std::is_floating_point>,
                      "invalid value");
    }

    {
        using type_list =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, double, eFileError>;

        static_assert(type_list::contains_predicate_compliant_v<std::is_enum>,
                      "invalid value");
        static_assert(
            type_list::contains_predicate_compliant_v<std::is_floating_point>,
            "invalid value");
        static_assert(
            type_list::contains_predicate_compliant_v<std::is_integral>,
            "invalid value");
        static_assert(
            not type_list::contains_predicate_compliant_v<std::is_void>,
            "invalid value");
    }
}

TEST(UtilsTypeList, ContainsCopies)
{
    {
        using type_list = utils::type_list<>;

        static_assert(not type_list::contains_copies, "invalid value");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(not type_list::contains_copies, "invalid value");
    }

    {
        using type_list =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, double, eFileError>;

        static_assert(type_list::contains_copies, "invalid value");
    }

    {
        using type_list =
            utils::type_list<eFileError, int32_t, eReaderError, float, int16_t,
                             eWriterError, double>;

        static_assert(not type_list::contains_copies, "invalid value");
    }
}

TEST(UtilsTypeList, ListOfPredicateCompliant)
{
    {
        using type_list = utils::type_list<>;

        using list_of_enums =
            type_list::list_of_predicate_compliant_t<std::is_enum>;

        static_assert(std::is_same_v<list_of_enums, type_list>,
                      "invalid list_of_enums");
    }

    {
        using type_list = utils::type_list<eFileError>;

        using list_of_enums =
            type_list::list_of_predicate_compliant_t<std::is_enum>;
        using list_of_int =
            type_list::list_of_predicate_compliant_t<std::is_integral>;

        static_assert(std::is_same_v<list_of_enums, type_list>,
                      "invalid list_of_enums");
        static_assert(std::is_same_v<list_of_int, utils::type_list<>>,
                      "invalid list_of_enums");
    }

    {
        using type_list =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, double, eFileError>;

        using list_of_enums =
            type_list::list_of_predicate_compliant_t<std::is_enum>;
        using list_of_float =
            type_list::list_of_predicate_compliant_t<std::is_floating_point>;
        using list_of_int =
            type_list::list_of_predicate_compliant_t<std::is_integral>;

        static_assert(std::is_same_v<
                          list_of_enums,
                          utils::type_list<eFileError, eReaderError, eFileError,
                                           eWriterError, eFileError>>,
                      "invalid list_of_enums");
        static_assert(
            std::is_same_v<list_of_float, utils::type_list<float, double>>,
            "invalid list_of_enums");
        static_assert(std::is_same_v<list_of_int, utils::type_list<int32_t>>,
                      "invalid list_of_enums");
    }

    {
        using type_list =
            utils::type_list<eFileError, int32_t, eReaderError, float, int16_t,
                             float, eWriterError, uint8_t, double>;

        using list_of_enums =
            type_list::list_of_predicate_compliant_t<std::is_enum>;
        using list_of_float =
            type_list::list_of_predicate_compliant_t<std::is_floating_point>;
        using list_of_int =
            type_list::list_of_predicate_compliant_t<std::is_integral>;
        using list_of_classes =
            type_list::list_of_predicate_compliant_t<std::is_class>;

        static_assert(
            std::is_same_v<
                list_of_enums,
                utils::type_list<eFileError, eReaderError, eWriterError>>,
            "invalid list_of_enums");
        static_assert(std::is_same_v<list_of_float,
                                     utils::type_list<float, float, double>>,
                      "invalid list_of_enums");
        static_assert(
            std::is_same_v<list_of_int,
                           utils::type_list<int32_t, int16_t, uint8_t>>,
            "invalid list_of_enums");
        static_assert(std::is_same_v<list_of_classes, utils::type_list<>>,
                      "invalid list_of_enums");
    }
}

TEST(UtilsTypeList, Concatenate)
{
    using utils::concatenate_t;
    using utils::type_list;

    {
        using list = type_list<>;
        using concatenate_result = concatenate_t<list>;
        static_assert(std::is_same_v<list, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list = type_list<int>;
        using concatenate_result = concatenate_t<list>;
        static_assert(std::is_same_v<list, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = type_list<>;
        using list2 = type_list<>;
        using concatenate_result = concatenate_t<list1, list2>;
        static_assert(std::is_same_v<type_list<>, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = type_list<>;
        using list2 = type_list<int>;
        using concatenate_result = concatenate_t<list1, list2>;
        static_assert(std::is_same_v<type_list<int>, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = type_list<int>;
        using list2 = type_list<int>;
        using concatenate_result = concatenate_t<list1, list2>;
        static_assert(std::is_same_v<type_list<int, int>, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = type_list<int, eReaderError, uint8_t>;
        using list2 = type_list<eWriterError, eFileError, int16_t>;
        using concatenate_result = concatenate_t<list1, list2>;
        static_assert(
            std::is_same_v<type_list<int, eReaderError, uint8_t, eWriterError,
                                     eFileError, int16_t>,
                           concatenate_result>,
            "invalid result of concatenation");
    }

    {
        using sub_list1_1 = type_list<int, eReaderError, uint8_t>;
        using sub_list1_2 = type_list<eWriterError, eFileError, int16_t>;
        using list1 = type_list<sub_list1_1, sub_list1_2>;

        using sub_list2_1 = type_list<double, int, char>;
        using sub_list2_2 = type_list<float, eReaderError, std::size_t>;
        using list2 = type_list<sub_list2_1, sub_list2_2>;

        using concatenate_result = concatenate_t<list1, list2>;
        static_assert(
            std::is_same_v<
                type_list<sub_list1_1, sub_list1_2, sub_list2_1, sub_list2_2>,
                concatenate_result>,
            "invalid result of concatenation");
    }
}

TEST(UtilsTypeList, MinSizeofIndex)
{
    using utils::min_sizeof_index_v;
    using utils::type_list;

    static_assert(min_sizeof_index_v<type_list<char>> == 0);
    static_assert(min_sizeof_index_v<type_list<char, std::uint8_t>> == 0);
    static_assert(min_sizeof_index_v<type_list<bool, char, std::uint8_t>> == 0);
    static_assert(
        min_sizeof_index_v<type_list<bool, char, std::int16_t, std::uint8_t>> ==
        0);
    static_assert(
        min_sizeof_index_v<
            type_list<std::int64_t, std::int32_t, bool, std::uint8_t>> == 2);
    static_assert(min_sizeof_index_v<type_list<std::int64_t, std::int32_t,
                                               std::int32_t, std::uint8_t>> ==
                  3);
}

TEST(UtilsTypeList, MaxSizeofIndex)
{
    using utils::max_sizeof_index_v;
    using utils::type_list;

    static_assert(max_sizeof_index_v<type_list<char>> == 0);
    static_assert(max_sizeof_index_v<type_list<char, std::uint8_t>> == 0);
    static_assert(max_sizeof_index_v<type_list<bool, char, std::uint8_t>> == 0);
    static_assert(
        max_sizeof_index_v<type_list<bool, char, std::int16_t, std::uint8_t>> ==
        2);
    static_assert(
        max_sizeof_index_v<
            type_list<std::int64_t, std::int32_t, bool, std::uint8_t>> == 0);
    static_assert(max_sizeof_index_v<
                      type_list<bool, std::uint64_t, std::int64_t, std::int32_t,
                                std::int32_t, std::uint8_t>> == 1);
}

TEST(UtilsTypeList, Sublist)
{
    using utils::sublist_t;
    using utils::type_list;

    static_assert(std::is_same_v<sublist_t<type_list<>, 0, 0>, type_list<>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char>, 0, 1>, type_list<char>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char>, 0, 0>, type_list<>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 0, 0>,
                       type_list<>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 0, 1>,
                       type_list<char>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 0, 2>,
                       type_list<char, bool>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 0, 3>,
                       type_list<char, bool, float>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 0, 4>,
                       type_list<char, bool, float, char>>);

    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 1, 0>,
                       type_list<>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 1, 1>,
                       type_list<bool>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 1, 2>,
                       type_list<bool, float>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 1, 3>,
                       type_list<bool, float, char>>);

    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 2, 0>,
                       type_list<>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 2, 1>,
                       type_list<float>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 2, 2>,
                       type_list<float, char>>);

    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 3, 0>,
                       type_list<>>);
    static_assert(
        std::is_same_v<sublist_t<type_list<char, bool, float, char>, 3, 1>,
                       type_list<char>>);
}

TEST(UtilsTypeList, ExcludeAt)
{
    using utils::exclude_at_t;
    using utils::type_list;

    static_assert(
        std::is_same_v<exclude_at_t<type_list<char>, 0>, type_list<>>);

    static_assert(std::is_same_v<exclude_at_t<type_list<char, std::uint8_t>, 0>,
                                 type_list<std::uint8_t>>);
    static_assert(std::is_same_v<exclude_at_t<type_list<char, std::uint8_t>, 1>,
                                 type_list<char>>);

    static_assert(
        std::is_same_v<exclude_at_t<type_list<bool, char, std::uint8_t>, 0>,
                       type_list<char, std::uint8_t>>);
    static_assert(
        std::is_same_v<exclude_at_t<type_list<bool, char, std::uint8_t>, 1>,
                       type_list<bool, std::uint8_t>>);
    static_assert(
        std::is_same_v<exclude_at_t<type_list<bool, char, std::uint8_t>, 2>,
                       type_list<bool, char>>);

    static_assert(
        std::is_same_v<
            exclude_at_t<type_list<float, char, std::int16_t, std::uint8_t>, 0>,
            type_list<char, std::int16_t, std::uint8_t>>);
    static_assert(
        std::is_same_v<
            exclude_at_t<type_list<float, char, std::int16_t, std::uint8_t>, 1>,
            type_list<float, std::int16_t, std::uint8_t>>);
    static_assert(
        std::is_same_v<
            exclude_at_t<type_list<float, char, std::int16_t, std::uint8_t>, 2>,
            type_list<float, char, std::uint8_t>>);
    static_assert(
        std::is_same_v<
            exclude_at_t<type_list<float, char, std::int16_t, std::uint8_t>, 3>,
            type_list<float, char, std::int16_t>>);
}

TEST(UtilsTypeList, Exclude)
{
    using utils::exclude_t;
    using utils::type_list;

    {
        using list = type_list<>;
        static_assert(std::is_same_v<list, exclude_t<list>>);
        static_assert(std::is_same_v<list, exclude_t<list, std::is_enum>>);
        static_assert(
            std::is_same_v<list,
                           exclude_t<list, std::is_enum, std::is_integral>>);
    }

    {
        using list = type_list<eReaderError>;
        static_assert(std::is_same_v<list, exclude_t<list>>);
        static_assert(
            std::is_same_v<type_list<>, exclude_t<list, std::is_enum>>);
        static_assert(std::is_same_v<list, exclude_t<list, std::is_integral>>);
        static_assert(
            std::is_same_v<type_list<>,
                           exclude_t<list, std::is_enum, std::is_integral>>);
    }

    {
        using list = type_list<int, bool, eReaderError, float, const bool,
                               eWriterError, double, char>;
        static_assert(std::is_same_v<list, exclude_t<list>>);
        static_assert(std::is_same_v<type_list<int, bool, eReaderError,
                                               const bool, eWriterError, char>,
                                     exclude_t<list, std::is_floating_point>>);
        static_assert(std::is_same_v<
                      type_list<int, bool, float, const bool, double, char>,
                      exclude_t<list, std::is_enum>>);
        static_assert(
            std::is_same_v<type_list<eReaderError, float, eWriterError, double>,
                           exclude_t<list, std::is_integral>>);
        static_assert(
            std::is_same_v<
                type_list<eReaderError, eWriterError>,
                exclude_t<list, std::is_integral, std::is_floating_point>>);
        static_assert(std::is_same_v<type_list<int, bool, eReaderError, float,
                                               eWriterError, double, char>,
                                     exclude_t<list, std::is_const>>);
    }
}

TEST(UtilsTypeList, TupleTypes)
{
    using utils::tuple_types_t;
    using utils::type_list;

    {
        using tuple = std::tuple<>;
        static_assert(std::is_same_v<type_list<>, tuple_types_t<tuple>>);
    }

    {
        using tuple = std::tuple<int>;
        static_assert(std::is_same_v<type_list<int>, tuple_types_t<tuple>>);
    }

    {
        using tuple = std::tuple<int, float&, bool, char>;
        static_assert(std::is_same_v<type_list<int, float&, bool, char>,
                                     tuple_types_t<tuple>>);
    }
}

TEST(UtilsTypeList, SortSizeofAscending)
{
    using utils::sort_sizeof_ascending_t;
    using utils::type_list;

    static_assert(
        std::is_same_v<sort_sizeof_ascending_t<type_list<>>, type_list<>>);

    static_assert(std::is_same_v<sort_sizeof_ascending_t<type_list<bool>>,
                                 type_list<bool>>);
    static_assert(std::is_same_v<sort_sizeof_ascending_t<type_list<bool, char>>,
                                 type_list<bool, char>>);
    static_assert(std::is_same_v<
                  sort_sizeof_ascending_t<type_list<bool, char, std::int8_t>>,
                  type_list<bool, char, std::int8_t>>);

    static_assert(
        std::is_same_v<sort_sizeof_ascending_t<type_list<bool, std::int16_t>>,
                       type_list<bool, std::int16_t>>);
    static_assert(
        std::is_same_v<sort_sizeof_ascending_t<type_list<std::int16_t, bool>>,
                       type_list<bool, std::int16_t>>);

    static_assert(std::is_same_v<
                  sort_sizeof_ascending_t<type_list<float, bool, std::int16_t>>,
                  type_list<bool, std::int16_t, float>>);
    static_assert(std::is_same_v<
                  sort_sizeof_ascending_t<type_list<float, std::int16_t, bool>>,
                  type_list<bool, std::int16_t, float>>);
    static_assert(
        std::is_same_v<
            sort_sizeof_ascending_t<type_list<float, std::int16_t, bool, char>>,
            type_list<bool, char, std::int16_t, float>>);
    static_assert(
        std::is_same_v<
            sort_sizeof_ascending_t<
                type_list<std::uint16_t, float, std::int16_t, bool, char>>,
            type_list<bool, char, std::uint16_t, std::int16_t, float>>);
}

TEST(UtilsTypeList, SortSizeofDescending)
{
    using utils::sort_sizeof_descending_t;
    using utils::type_list;

    static_assert(
        std::is_same_v<sort_sizeof_descending_t<type_list<>>, type_list<>>);

    static_assert(std::is_same_v<sort_sizeof_descending_t<type_list<bool>>,
                                 type_list<bool>>);
    static_assert(
        std::is_same_v<sort_sizeof_descending_t<type_list<bool, char>>,
                       type_list<bool, char>>);
    static_assert(std::is_same_v<
                  sort_sizeof_descending_t<type_list<bool, char, std::int8_t>>,
                  type_list<bool, char, std::int8_t>>);

    static_assert(
        std::is_same_v<sort_sizeof_descending_t<type_list<bool, std::int16_t>>,
                       type_list<std::int16_t, bool>>);
    static_assert(
        std::is_same_v<sort_sizeof_descending_t<type_list<std::int16_t, bool>>,
                       type_list<std::int16_t, bool>>);

    static_assert(
        std::is_same_v<
            sort_sizeof_descending_t<type_list<float, bool, std::int16_t>>,
            type_list<float, std::int16_t, bool>>);
    static_assert(
        std::is_same_v<
            sort_sizeof_descending_t<type_list<float, std::int16_t, bool>>,
            type_list<float, std::int16_t, bool>>);
    static_assert(
        std::is_same_v<sort_sizeof_descending_t<
                           type_list<float, std::int16_t, bool, char>>,
                       type_list<float, std::int16_t, bool, char>>);
    static_assert(
        std::is_same_v<
            sort_sizeof_descending_t<
                type_list<std::uint16_t, float, std::int16_t, bool, char>>,
            type_list<float, std::uint16_t, std::int16_t, bool, char>>);
}

TEST(UtilsTypeList, CartesianProduct)
{
    using utils::cartesian_product_t;
    using utils::type_list;
    using utils::value_list;

    {
        using T1 = char;
        using T2 = int;
        using T3 = float;

        using list1 = type_list<T1>;
        using list2 = type_list<T2, T3>;
        using expected = type_list<type_list<T1, T2>, type_list<T1, T3>>;
        using actual = cartesian_product_t<list1, list2>;
        static_assert(std::is_same_v<expected, actual>);
    }

    {
        using T1 = int;
        using T2 = float;
        using T3 = double;
        using T4 = char;
        using T5 = unsigned int;

        using list1 = type_list<T1, T2, T3>;
        using list2 = type_list<T4, T5>;
        using expected =
            type_list<type_list<T1, T4>, type_list<T1, T5>, type_list<T2, T4>,
                      type_list<T2, T5>, type_list<T3, T4>, type_list<T3, T5>>;
        using actual = cartesian_product_t<list1, list2>;
        static_assert(std::is_same_v<expected, actual>);
    }

    {
        using T1 = double;
        using T2 = value_list<1, 2>;
        using T3 = unsigned int;
        using T4 = type_list<short, int>;
        using T5 = float;
        using T6 = int;
        using T7 = char;

        using list1 = type_list<T1, T2, T3>;
        using list2 = type_list<T4, T5>;
        using list3 = type_list<T6, T7>;
        using expected =
            type_list<type_list<T1, T4, T6>, type_list<T1, T4, T7>,
                      type_list<T1, T5, T6>, type_list<T1, T5, T7>,
                      type_list<T2, T4, T6>, type_list<T2, T4, T7>,
                      type_list<T2, T5, T6>, type_list<T2, T5, T7>,
                      type_list<T3, T4, T6>, type_list<T3, T4, T7>,
                      type_list<T3, T5, T6>, type_list<T3, T5, T7>>;
        using actual = cartesian_product_t<list1, list2, list3>;
        static_assert(std::is_same_v<expected, actual>);
    }

    {
        using T1 = double;
        using T2 = char;
        using T3 = unsigned int;
        using T4 = short;
        using T5 = float;
        using T6 = int;

        using list1 = type_list<T1, T2, T3>;
        using list2 = type_list<T4, T5>;
        using list3 = type_list<T6>;
        using expected =
            type_list<type_list<T1, T4, T6>, type_list<T1, T5, T6>,
                      type_list<T2, T4, T6>, type_list<T2, T5, T6>,
                      type_list<T3, T4, T6>, type_list<T3, T5, T6>>;
        using actual = cartesian_product_t<list1, list2, list3>;
        static_assert(std::is_same_v<expected, actual>);
    }

    {
        using T1 = int;
        using T2 = char;

        using list1 = type_list<T1>;
        using list2 = type_list<T2>;
        using expected = type_list<type_list<T1, T2>>;
        using actual = cartesian_product_t<list1, list2>;
        static_assert(std::is_same_v<expected, actual>);
    }
}

template <typename T>
struct is_not_integral : std::negation<std::is_integral<T>>
{
};

TEST(UtilsTypeList, SelectTypes)
{
    using namespace utils;
    using T1 = double;
    using T2 = value_list<1, 2>;
    using T3 = unsigned int;
    using T4 = type_list<short, int>;
    using T5 = int;
    using T6 = float;
    using T7 = int;
    using T8 = void;
    using T9 = char;

    using misc_types = type_list<T1, T2, T3, T4, T5, T6, T7, T8, T9>;
    {
        using expected = type_list<T1, T6>;
        using actual = select_types_t<misc_types, std::is_floating_point>;
        static_assert(std::is_same_v<actual, expected>);
    }
    {
        using expected = type_list<T3, T5, T7, T9>;
        using actual = select_types_t<misc_types, std::is_integral>;
        static_assert(std::is_same_v<actual, expected>);
    }
    {
        using expected = type_list<T1, T2, T4, T6, T8>;
        using actual = select_types_t<misc_types, is_not_integral>;
        static_assert(std::is_same_v<actual, expected>);
    }
    {
        using expected = type_list<>;
        using actual = select_types_t<type_list<>, std::is_integral>;
        static_assert(std::is_same_v<actual, expected>);
    }
    {
        using expected = type_list<int>;
        using actual = select_types_t<type_list<int>, std::is_integral>;
        static_assert(std::is_same_v<actual, expected>);
    }
    {
        using expected = type_list<>;
        using actual = select_types_t<type_list<void>, std::is_integral>;
        static_assert(std::is_same_v<actual, expected>);
    }
}
