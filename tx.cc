#include <chrono>
#include <cstdio>
#include <iomanip>
#include <iostream>

#include <rte_eal.h>
#include <rte_ethdev.h>

#include "tools.h"

int main(int argc_, char** argv_)
{
	const unsigned BURST_SIZE   = 16;
	const unsigned TX_RING_SIZE = 1024;
    const unsigned PORT_INDEX   = 0;

    auto eal_init_status = dpdk_tools::init_eal(argc_, argv_);
    auto mbuf_pool = dpdk_tools::init_mbuf_pool();
    dpdk_tools::init_port(PORT_INDEX, mbuf_pool, 0, 1);
    dpdk_tools::start_port(PORT_INDEX);

    argc_ -= eal_init_status;
	argv_ += eal_init_status;

    auto mac_src = dpdk_tools::rte_ether_addr_from_port(PORT_INDEX);
    auto mac_dst = dpdk_tools::rte_ether_addr_from_string("02:3a:a4:9a:3d:a4");

    struct rte_mbuf* pkt[BURST_SIZE] = { nullptr };

    for (auto i = 0; i < BURST_SIZE; i++) {
        pkt[i] = rte_pktmbuf_alloc(mbuf_pool);
        pkt[i]->data_len = pkt[i]->pkt_len = sizeof(struct rte_ether_hdr);
        struct rte_ether_hdr* eth_hdr = rte_pktmbuf_mtod(pkt[i], struct rte_ether_hdr*);
        rte_ether_addr_copy(&mac_dst, &eth_hdr->d_addr);
        rte_ether_addr_copy(&mac_src, &eth_hdr->s_addr);
        eth_hdr->ether_type = rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4);
        // rte_pktmbuf_dump(stdout, pkt[i], pkt[i]->pkt_len);
    }

    unsigned long tx_count = 0;

    auto start = std::chrono::high_resolution_clock::now();
    for (;;) {
        tx_count += rte_eth_tx_burst(PORT_INDEX, 0, pkt, BURST_SIZE);

        if (tx_count >= 1000000)
            break;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double time_s = (double) dur.count() / 1000000;
    std::cout << time_s << "s" << std::endl;    
    std::cout << "sent: " << tx_count << std::endl;

    return 0;
}
