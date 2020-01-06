
set(MSG_PRE "[dpdk]")

if (DEFINED ENV{RTE_SDK})
    set(RTE_SDK $ENV{RTE_SDK})
else()
    set(RTE_SDK "$ENV{HOME}/opt/dpdk-19.11")
endif()

if (DEFINED ENV{RTE_TARGET})
    set(RTE_TARGET $ENV{RTE_TARGET})
else()
    set(RTE_TARGET x86_64-native-linuxapp-gcc)
endif()

file(STRINGS "${RTE_SDK}/VERSION" RTE_VERSION)
message(STATUS "${MSG_PRE} Using RTE_SDK=${RTE_SDK}")
message(STATUS "${MSG_PRE} Using RTE_TARGET=${RTE_TARGET}")
message(STATUS "${MSG_PRE} Detected DPDK version: ${RTE_VERSION}")


set(RTE_LIBRARY_HANDLES acl bbdev bitratestats bpf bus_dpaa bus_fslmc bus_ifpga
    bus_pci bus_vdev bus_vmbus cfgfile cmdline common_cpt common_dpaax common_octeontx2
    common_octeontx compressdev cryptodev distributor eal efd ethdev eventdev fib
    flow_classify gro gso hash ip_frag ipsec jobstats kni kvargs latencystats lpm mbuf
    member mempool mempool_bucket mempool_dpaa2 mempool_dpaa mempool_octeontx2
    mempool_octeontx mempool_ring mempool_stack meter metrics net pci pdump pipeline
    pmd_af_packet pmd_ark pmd_atlantic pmd_avp pmd_axgbe pmd_bbdev_fpga_lte_fec
    pmd_bbdev_null pmd_bbdev_turbo_sw pmd_bnxt pmd_bond pmd_caam_jr pmd_crypto_scheduler
    pmd_cxgbe pmd_dpaa2 pmd_dpaa2_event pmd_dpaa2_sec pmd_dpaa pmd_dpaa_event pmd_dpaa_sec
    pmd_dsw_event pmd_e1000 pmd_ena pmd_enetc pmd_enic pmd_failsafe pmd_fm10k pmd_hinic
    pmd_hns3 pmd_i40e pmd_iavf pmd_ice pmd_ifc pmd_ixgbe pmd_kni pmd_lio pmd_memif pmd_netvsc
    pmd_nfp pmd_nitrox pmd_null pmd_null_crypto pmd_octeontx2 pmd_octeontx2_crypto
    pmd_octeontx2_event pmd_octeontx pmd_octeontx_crypto pmd_octeontx_ssovf pmd_octeontx_zip
    pmd_opdl_event pmd_qat pmd_qede pmd_ring pmd_sfc_efx pmd_skeleton_event pmd_softnic
    pmd_sw_event pmd_tap pmd_thunderx_nicvf pmd_vdev_netvsc pmd_vhost pmd_virtio
    pmd_virtio_crypto pmd_vmxnet3_uio port power rawdev rawdev_dpaa2_cmdif rawdev_dpaa2_qdma
    rawdev_ioat rawdev_ntb rawdev_octeontx2_dma rawdev_skeleton rcu reorder rib ring sched
    security stack table timer vhost)

set(DPDK_LIB "-Wl,--whole-archive")
set(DPDK_INCLUDE ${RTE_SDK}/include/dpdk)

foreach(handle ${RTE_LIBRARY_HANDLES})
    find_library(LIB_DPDK_RTE_${handle} rte_${handle} HINTS ${RTE_SDK}/lib)
    if(NOT LIB_DPDK_RTE_${handle})
        message(FATAL_ERROR "${MSG_PRE} librte_${handle} not found")
    endif()
    list(APPEND DPDK_LIB "${LIB_DPDK_RTE_${handle}}")
    message(STATUS "${MSG_PRE} Found librte_${handle}: ${LIB_DPDK_RTE_${handle}}")
endforeach()

list(APPEND DPDK_LIB "-Wl,--no-whole-archive")

message(STATUS "${MSG_PRE} DPDK_INCLUDE and DPDK_LIB set")
