/// \file
///
/// Test for stack::vector
///
/// Adapted from libc++: https://libcxx.llvm.org
/// under the following license:
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <stack_vector>
#include "utils.hpp"

// Explicit instantiations
template struct std::experimental::stack_vector<int, 0>;  // trivial empty
template struct std::experimental::stack_vector<int, 1>;  // trivial non-empty
template struct std::experimental::stack_vector<int, 2>;  // trivial nom-empty
template struct std::experimental::stack_vector<int, 3>;  // trivial nom-empty

struct non_trivial {
  int i = 0;
  virtual void foo() {}
};

static_assert(!std::is_trivial<non_trivial>{}, "");

// non-trivial empty:
template struct std::experimental::stack_vector<non_trivial, 0>;
// non-trivial non-empty:
template struct std::experimental::stack_vector<non_trivial, 1>;
template struct std::experimental::stack_vector<non_trivial, 2>;
template struct std::experimental::stack_vector<non_trivial, 3>;

template <typename T, std::size_t N>
using vector = std::experimental::stack_vector<T, N>;

template <typename T, std::size_t N>
constexpr bool test_bounds(vector<T, N> const& v, std::size_t sz,
                           std::initializer_list<T> vs = {}) {
  assert(v.size() == sz);
  assert(v.max_size() == N);
  assert(v.capacity() == N);

  std::decay_t<T> count = std::decay_t<T>();
  for (std::size_t i = 0; i != sz; ++i) { assert(v[i] == ++count); }

  return true;
}

int main() {
  {  // const
    vector<const int, 0> v0 = {};
    test_bounds(v0, 0);

    constexpr vector<const int, 0> vc0 = {};
    test_bounds(vc0, 0);
    static_assert(test_bounds(vc0, 0), "");

    vector<const int, 1> v1 = {1};
    test_bounds(v1, 1);

    constexpr vector<const int, 3> vc1 = {1};
    test_bounds(vc1, 1);
    static_assert(test_bounds(vc1, 1), "");

    vector<const int, 3> v3 = {1, 2, 3};
    test_bounds(v3, 3);

    constexpr vector<const int, 3> vc3 = {1, 2, 3};
    test_bounds(vc3, 3);
    static_assert(test_bounds(vc3, 3), "");
  }

  auto test_contiguous = [](auto&& c) {
    for (size_t i = 0; i < c.size(); ++i) {
      assert(*(c.begin() + i) == *(std::addressof(*c.begin()) + i));
    }
  };

  {  // contiguous
    typedef int T;
    typedef vector<T, 3> C;
    // test_contiguous(C());
    // test_contiguous(C(3, 5));
  }

  /*
  { // default construct element
    typedef int T;
    typedef vector<T, 3> C;
    C c(1);
    assert(back(c) == 0);
    assert(front(c) == 0);
    assert(c[0] == 0);
  }

  { // iterator
    typedef int T;
    typedef vector<T, 3> C;
    C c;
    C::iterator i = begin(c);
    C::iterator j = end(c);
    assert(distance(i, j) == 0);
    assert(i == j);
  }
  { // const iterator
    typedef int T;
    typedef vector<T, 3> C;
    const C c{};
    C::const_iterator i = begin(c);
    C::const_iterator j = end(c);
    assert(distance(i, j) == 0);
    assert(i == j);
  }
  { // cbegin/cend
    typedef int T;
    typedef vector<T, 3> C;
    C c;
    C::const_iterator i = cbegin(c);
    C::const_iterator j = cend(c);
    assert(distance(i, j) == 0);
    assert(i == j);
    assert(i == end(c));
  }
  { // range constructor
    typedef int T;
    typedef vector<T, 10> C;
    const T t[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    C c(t);
    test::check_equal(t, c);
    C::iterator i = begin(c);
    assert(*i == 0);
    ++i;
    assert(*i == 1);
    *i = 10;
    assert(*i == 10);
    assert(distance(c) == 10);
  }
  { // iterator constructor
    typedef int T;
    typedef vector<T, 10> C;
    const T t[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    C c(begin(t), end(t));
    test::check_equal(t, c);
    C::iterator i = begin(c);
    assert(*i == 0);
    ++i;
    assert(*i == 1);
    *i = 10;
    assert(*i == 10);
    assert(distance(c) == 10);
  }
  { // N3644 testing
    typedef vector<int, 10> C;
    C::iterator ii1{}, ii2{};
    C::iterator ii4 = ii1;
    C::const_iterator cii{};
    assert(ii1 == ii2);
    assert(ii1 == ii4);

    assert(!(ii1 != ii2));

    assert((ii1 == cii));
    assert((cii == ii1));
    assert(!(ii1 != cii));
    assert(!(cii != ii1));
    assert(!(ii1 < cii));
    assert(!(cii < ii1));
    assert((ii1 <= cii));
    assert((cii <= ii1));
    assert(!(ii1 > cii));
    assert(!(cii > ii1));
    assert((ii1 >= cii));
    assert((cii >= ii1));
    assert((cii - ii1) == 0);
    assert((ii1 - cii) == 0);
  }
  { // types
    auto check_types = [](auto &&c, auto &&t) {
      using C = uncvref_t<decltype(c)>;
      using T = uncvref_t<decltype(t)>;
      static_assert(Container<C>{}, "");
      static_assert(Same<meta::_t<value_type<C>>, T>{}, "");
      // static_assert(Same<meta::_t<size_type<C>>, std::size_t>{}, "");
      static_assert(Same<meta::_t<difference_type<C>>, std::ptrdiff_t>{}, "");
      static_assert(Same<typename C::reference, T &>{}, "");
      static_assert(Same<typename C::const_reference, T const &>{}, "");
      static_assert(Same<typename C::pointer, T *>{}, "");
      static_assert(Same<typename C::const_pointer, T const *>{}, "");
      static_assert(Same<typename std::iterator_traits<
                             typename C::iterator>::iterator_category,
                         std::random_access_iterator_tag>{},
                    "");
      static_assert(Same<typename std::iterator_traits<
                             typename C::const_iterator>::iterator_category,
                         std::random_access_iterator_tag>{},
                    "");
      static_assert(std::is_nothrow_default_constructible<C>::value, "");

      static_assert(std::is_nothrow_move_constructible<C>::value, "");

    };

    using m = vector<int, 10>;
    using c = const m;
    check_types(m{}, int{});
    check_types(c{}, int{});

    using mm = vector<std::unique_ptr<int>, 10>;
    using mc = const mm;
    check_types(mm{}, std::unique_ptr<int>{});
    check_types(mc{}, std::unique_ptr<int>{});
  }

  { // capacity
    vector<int, 10> a;
    assert(a.capacity() == 10_u);
    for (int i = 0; i != 10; ++i) {
      a.push_back(0);
    }
    assert(a.capacity() == 10_u);
    assert(a.size() == 10_u);
  }

  {
      // erase
  }

  { // resize copyable
    using Copyable = int;
    vector<Copyable, 10> a(10_u, 5);
    assert(a.size() == 10_u);
    assert(a.capacity() == 10_u);
    test_contiguous(a);
    assert(a[0] == 5);
    assert(a[9] == 5);
    a.resize(5);
    assert(a.size() == 5_u);
    assert(a.capacity() == 10_u);
    test_contiguous(a);
    a.resize(9);
    assert(a[4] == 5);
    assert(a[5] == 0);
    assert(a[8] == 0);
    assert(a.size() == 9_u);
    assert(a.capacity() == 10_u);
    test_contiguous(a);
    a.resize(10, 3);
    assert(a[4] == 5);
    assert(a[8] == 0);
    assert(a[9] == 3);
    assert(a.size() == 10_u);
    assert(a.capacity() == 10_u);
    test_contiguous(a);
  }

  { // resize move-only
    using MoveOnly = std::unique_ptr<int>;
    vector<MoveOnly, 10> a(10);
    assert(a.size() == 10_u);
    assert(a.capacity() == 10_u);
    a.resize(5);
    assert(a.size() == 5_u);
    assert(a.capacity() == 10_u);
    a.resize(9);
    assert(a.size() == 9_u);
    assert(a.capacity() == 10_u);
  }

  { // assign copy
    vector<int, 3> a = {0, 1, 2};
    assert(a.size() == 3_u);
    vector<int, 3> b;
    assert(b.size() == 0_u);
    b = a;
    assert(b.size() == 3_u);
    test::check_equal(a, b);
  }

  { // copy construct
    vector<int, 3> a = {0, 1, 2};
    assert(a.size() == 3_u);
    vector<int, 3> b(a);
    assert(b.size() == 3_u);

    test::check_equal(a, b);
  }

  { // assign move
    using MoveOnly = std::unique_ptr<int>;
    vector<MoveOnly, 3> a(3);
    assert(a.size() == 3_u);
    vector<MoveOnly, 3> b;
    assert(b.size() == 0_u);
    b = std::move(a);
    assert(b.size() == 3_u);
    assert(a.size() == 3_u);
  }

  { // move construct
    using MoveOnly = std::unique_ptr<int>;
    vector<MoveOnly, 3> a(3);
    assert(a.size() == 3_u);
    vector<MoveOnly, 3> b(std::move(a));
    assert(b.size() == 3_u);
    assert(a.size() == 3_u);
  }

  { // old tests

    using stack_vec = vector<int, 5>;
    stack_vec vec1(5);
    vec1[0] = 0;
    vec1[1] = 1;
    vec1[2] = 2;
    vec1[3] = 3;
    vec1[4] = 4;
    {
      stack_vec vec2;
      vec2.push_back(5);
      vec2.push_back(6);
      vec2.push_back(7);
      vec2.push_back(8);
      vec2.push_back(9);
      assert(vec1[0] == 0);
      assert(vec1[4] == 4);
      assert(vec2[0] == 5);
      assert(vec2[4] == 9);
    }
    {
      auto vec2 = vec1;
      assert(vec2[0] == 0);
      assert(vec2[4] == 4);
      assert(vec1[0] == 0);
      assert(vec1[4] == 4);
    }
    {
      int count_ = 0;
      for (auto i : vec1) {
        assert(i == count_++);
      }
    }

    {
      std::vector<int> vec2(5);
      vec2[0] = 4;
      vec2[1] = 3;
      vec2[2] = 2;
      vec2[3] = 1;
      vec2[4] = 0;
      stack_vec vec(vec2.size());
      copy(vec2, begin(vec));
      int count_ = 4;
      for (auto i : vec) {
        assert(i == count_--);
      }
    }
  }
  {
    using stack_vec = vector<int, 0>;
    static_assert(sizeof(stack_vec) == 1, "");

    constexpr auto a = stack_vec{};
    static_assert(a.size() == std::size_t{0}, "");
  }

  return test::result();
  */
}