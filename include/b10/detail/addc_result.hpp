#ifndef B10_INCLUDED_DETAIL_ADDC_RESULT_HPP
#define B10_INCLUDED_DETAIL_ADDC_RESULT_HPP

namespace b10::detail {

template <typename T>
struct addc_result {
    T value;
    bool carry;
};

} // namespace b10::detail

#endif // !defined(B10_INCLUDED_DETAIL_ADDC_RESULT_HPP)
