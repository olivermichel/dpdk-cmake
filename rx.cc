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
    dpdk_tools::init_port(PORT_INDEX, mbuf_pool, 1, 0);
    dpdk_tools::start_port(PORT_INDEX);

    argc_ -= eal_init_status;
	argv_ += eal_init_status;

	unsigned long rx_total = 0;

	for (;;) {
		struct rte_mbuf* rx_buf[BURST_SIZE];
		auto rx_count = rte_eth_rx_burst(PORT_INDEX, 0, rx_buf, BURST_SIZE);
		
		if (rx_count == 0) {
			std::cout << 0 << std::endl;
			continue;
		} 

		rx_total += rx_count;
		std::cout << rx_total << std::endl;
	}
	
	return 0;
}
