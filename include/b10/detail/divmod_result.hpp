#ifndef B10_INCLUDED_DETAIL_DIVMOD_RESULT_HPP
#define B10_INCLUDED_DETAIL_DIVMOD_RESULT_HPP

namespace b10::detail {

template <typename T>
struct divmod_result {
    T q;
    T r;
};

} // namespace b10::detail

#endif // !defined(B10_INCLUDED_DETAIL_DIVMOD_RESULT_HPP)
