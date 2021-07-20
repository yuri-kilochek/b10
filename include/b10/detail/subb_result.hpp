#ifndef B10_INCLUDED_DETAIL_SUBB_RESULT_HPP
#define B10_INCLUDED_DETAIL_SUBB_RESULT_HPP

namespace b10::detail {

template <typename T>
struct subb_result {
    T value;
    bool borrow;
};

} // namespace b10::detail

#endif // !defined(B10_INCLUDED_DETAIL_SUBB_RESULT_HPP)
