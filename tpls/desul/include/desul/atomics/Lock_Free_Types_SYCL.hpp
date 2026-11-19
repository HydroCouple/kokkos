/*
Copyright (c) 2019, Lawrence Livermore National Security, LLC
and DESUL project contributors. See the COPYRIGHT file for details.
Source: https://github.com/desul/desul

SPDX-License-Identifier: (BSD-3-Clause)
*/

#ifndef DESUL_LOCK_FREE_TYPES_SYCL_HPP_
#define DESUL_LOCK_FREE_TYPES_SYCL_HPP_

#include <desul/atomics/Common.hpp>

#if defined(DESUL_IMPL_ADAPTIVECPP_MACOS_METAL)
#include <hipSYCL/sycl/libkernel/sscp/builtins/assert.hpp>
#endif

namespace desul {
namespace Impl {

#if defined(DESUL_IMPL_ADAPTIVECPP_MACOS_METAL)
// Single-pass compilation also sees host-only instantiations of wide atomics.
// Fail only if such a call actually executes on the device.
[[noreturn]] inline void unsupported_sycl_lock_based_atomic() {
  __acpp_sscp_assert_fail(
      "AdaptiveCpp SYCL requires lock-free 4- or 8-byte atomics", __FILE__,
      __LINE__, __func__);
  __builtin_unreachable();
}
#endif

template <class T>
inline constexpr bool device_atomic_always_lock_free<T, void> = (sizeof(T) == 4) ||
                                                                (sizeof(T) == 8);

}  // namespace Impl
}  // namespace desul
#endif
