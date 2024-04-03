#ifndef utils_details_cxz_h
#define utils_details_cxz_h

namespace utils
{
namespace details
{
template <typename T, typename IntT>
struct less_eq_pred : std::bool_constant<sizeof(IntT) <= sizeof(T)>
{
};
}  // namespace details
}  // namespace utils

#endif /* utils_details_cxz_h */
