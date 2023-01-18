#include <gtest/gtest.h>

#include "utils/type_list.h"

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

        static_assert(type_list::index_of<std::is_enum> == type_list::size,
                      "invalid index");
        static_assert(type_list::index_of<std::is_signed> == type_list::size,
                      "invalid index");
        static_assert(type_list::index_of<std::is_void> == type_list::size,
                      "invalid index");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(type_list::index_of<std::is_enum> == 0, "invalid index");
        static_assert(type_list::index_of<std::is_signed> == type_list::size,
                      "invalid index");
        static_assert(type_list::index_of<std::is_void> == type_list::size,
                      "invalid index");
    }

    {
        using type_list = utils::type_list<eFileError, int32_t, eReaderError,
                                           float, eFileError, eWriterError>;

        static_assert(type_list::index_of<std::is_signed> == 1,
                      "invalid index");
        static_assert(type_list::index_of<std::is_floating_point> == 3,
                      "invalid index");
        static_assert(type_list::index_of<std::is_enum> == 0, "invalid index");
        static_assert(type_list::index_of<std::is_void> == type_list::size,
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
            type_list::count_of_predicate_compliant<std::is_enum> == 0,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant<std::is_integral> == 0,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant<std::is_floating_point> ==
                0,
            "invalid value");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(
            type_list::count_of_predicate_compliant<std::is_enum> == 1,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant<std::is_integral> == 0,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant<std::is_floating_point> ==
                0,
            "invalid value");
    }

    {
        using type_list =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, double, eFileError>;

        static_assert(
            type_list::count_of_predicate_compliant<std::is_enum> == 5,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant<std::is_floating_point> ==
                2,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant<std::is_integral> == 1,
            "invalid value");
        static_assert(
            type_list::count_of_predicate_compliant<std::is_void> == 0,
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
}

TEST(UtilsTypeList, ContainsPredicateCompliant)
{
    {
        using type_list = utils::type_list<>;

        static_assert(not type_list::contains_predicate_compliant<std::is_enum>,
                      "invalid value");
        static_assert(
            not type_list::contains_predicate_compliant<std::is_integral>,
            "invalid value");
        static_assert(
            not type_list::contains_predicate_compliant<std::is_floating_point>,
            "invalid value");
    }

    {
        using type_list = utils::type_list<eFileError>;

        static_assert(type_list::contains_predicate_compliant<std::is_enum>,
                      "invalid value");
        static_assert(
            not type_list::contains_predicate_compliant<std::is_integral>,
            "invalid value");
        static_assert(
            not type_list::contains_predicate_compliant<std::is_floating_point>,
            "invalid value");
    }

    {
        using type_list =
            utils::type_list<eFileError, int32_t, eReaderError, float,
                             eFileError, eWriterError, double, eFileError>;

        static_assert(type_list::contains_predicate_compliant<std::is_enum>,
                      "invalid value");
        static_assert(
            type_list::contains_predicate_compliant<std::is_floating_point>,
            "invalid value");
        static_assert(type_list::contains_predicate_compliant<std::is_integral>,
                      "invalid value");
        static_assert(not type_list::contains_predicate_compliant<std::is_void>,
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
    {
        using list = utils::type_list<>;
        using concatenate_result = utils::concatenate_t<list>;
        static_assert(std::is_same_v<list, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list = utils::type_list<int>;
        using concatenate_result = utils::concatenate_t<list>;
        static_assert(std::is_same_v<list, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = utils::type_list<>;
        using list2 = utils::type_list<>;
        using concatenate_result = utils::concatenate_t<list1, list2>;
        static_assert(std::is_same_v<utils::type_list<>, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = utils::type_list<>;
        using list2 = utils::type_list<int>;
        using concatenate_result = utils::concatenate_t<list1, list2>;
        static_assert(std::is_same_v<utils::type_list<int>, concatenate_result>,
                      "invalid result of concatenation");
    }

    {
        using list1 = utils::type_list<int>;
        using list2 = utils::type_list<int>;
        using concatenate_result = utils::concatenate_t<list1, list2>;
        static_assert(
            std::is_same_v<utils::type_list<int, int>, concatenate_result>,
            "invalid result of concatenation");
    }

    {
        using list1 = utils::type_list<int, eReaderError, uint8_t>;
        using list2 = utils::type_list<eWriterError, eFileError, int16_t>;
        using concatenate_result = utils::concatenate_t<list1, list2>;
        static_assert(
            std::is_same_v<utils::type_list<int, eReaderError, uint8_t,
                                            eWriterError, eFileError, int16_t>,
                           concatenate_result>,
            "invalid result of concatenation");
    }
}
