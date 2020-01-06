#include <iostream>
#include <iomanip>

#include <rte_eal.h>
#include <rte_ethdev.h>

#include "tools.h"

int main(int argc_, char** argv_)
{
	const unsigned RX_RING_SIZE = 1024;
	const unsigned BURST_SIZE   = 32;

    auto init_status = rte_eal_init(argc_, argv_);

    if (init_status < 0)
        rte_exit(EXIT_FAILURE, "failed initializing EAL");

    argc_ -= init_status;
	argv_ += init_status;

	struct rte_mempool* mbuf_pool = dpdk_tools::init_mbuf_pool();
	
	unsigned port_index = 0;
	dpdk_tools::init_port(port_index, mbuf_pool, 1, 0);
	dpdk_tools::start_port(port_index);

	// get device mac address
	struct rte_ether_addr addr;
	rte_eth_macaddr_get(port_index, &addr);
	std::cout << "mac addr: ";
	for (unsigned i = 0; i < 6; i++)
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned) addr.addr_bytes[i];
	std::cout << std::endl;


	for (;;) {
		struct rte_mbuf* rx_buf[BURST_SIZE];
		const uint16_t nb_rx = rte_eth_rx_burst(port_index, 0, rx_buf, BURST_SIZE);
		
		if (nb_rx == 0) {
			std::cout << 0 << std::endl;
			continue;
		} 

		std::cout << nb_rx << std::endl;
	}
	
	return 0;
}
