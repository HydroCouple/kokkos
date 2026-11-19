// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// SPDX-FileCopyrightText: Copyright Contributors to the Kokkos project

#ifndef KOKKOS_SYCL_ABORT_HPP
#define KOKKOS_SYCL_ABORT_HPP

#include <Kokkos_Printf.hpp>
#if defined(KOKKOS_ENABLE_SYCL)
#include <sycl/sycl.hpp>
#if defined(KOKKOS_IMPL_SYCL_ADAPTIVECPP_METAL)
#include <hipSYCL/sycl/libkernel/sscp/builtins/assert.hpp>
#endif

namespace Kokkos {
namespace Impl {

inline void sycl_abort(char const* msg) {
// FIXME_WINDOWS __assert_fail isn't defined
#if defined(KOKKOS_IMPL_SYCL_ADAPTIVECPP_METAL)
  __acpp_sscp_assert_fail(msg, __FILE__, __LINE__, __func__);
  __builtin_unreachable();
#elif defined(NDEBUG) || defined(_MSC_VER)
  Kokkos::printf("Aborting with message %s.\n", msg);
#else
  // Choosing "" here causes problems but a single whitespace character works.
  const char* empty = " ";
  __assert_fail(msg, empty, 0, empty);
#endif
}

}  // namespace Impl
}  // namespace Kokkos

#endif
#endif
