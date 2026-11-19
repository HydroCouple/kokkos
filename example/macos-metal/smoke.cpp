// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <Kokkos_Core.hpp>
#include <cstdio>
#include <string_view>

#ifndef KOKKOS_IMPL_SYCL_ADAPTIVECPP_METAL
#error Configure against the Apple silicon AdaptiveCpp build of Kokkos
#endif

int main(int argc, char** argv) {
  if (argc != 2) return 2;
  const std::string_view mode(argv[1]);
  if (mode != "metal" && mode != "cpu-control") return 2;
  const auto devices = Kokkos::Impl::get_sycl_devices();
  for (std::size_t i = 0; i < devices.size(); ++i) {
    const auto& device = devices[i];
    const bool metal =
        device.is_gpu() && device.get_backend() == sycl::backend::metal;
    if (mode == "metal" ? !metal : !device.is_cpu()) continue;
    Kokkos::ScopeGuard guard(
        Kokkos::InitializationSettings{}.set_device_id(static_cast<int>(i)));
    Kokkos::SYCL space;
    if (space.sycl_queue().get_device() != device) return 1;
    Kokkos::View<int*, Kokkos::SYCL::memory_space> values("values", 4099);
    Kokkos::parallel_for(
        "metal-smoke", Kokkos::RangePolicy<Kokkos::SYCL>(space, 0, 4099),
        KOKKOS_LAMBDA(int j) { values(j) = j * 3 + 7; });
    auto host = Kokkos::create_mirror(values);
    Kokkos::deep_copy(space, host, values);
    space.fence();
    for (int j = 0; j < 4099; ++j)
      if (host(j) != j * 3 + 7) return 1;
    std::printf("PASS %s device=%s values=4099\n", argv[1],
                device.get_info<sycl::info::device::name>().c_str());
    return 0;
  }
  std::fprintf(stderr, "Requested %s device unavailable\n", argv[1]);
  return 77;
}
