#include <iostream>
#include <iomanip>

#include <rte_eal.h>
#include <rte_ethdev.h>

#include "tools.h"

int main(int argc_, char** argv_)
{
    auto eal_init_status = dpdk_tools::init_eal(argc_, argv_);

    argc_ -= eal_init_status;
	argv_ += eal_init_status;

	unsigned port_count = rte_eth_dev_count_avail();
	std::cout << "rte_eth_dev_count_avail: " << port_count << std::endl;

	for (int if_index = 0; if_index < rte_eth_dev_count_avail(); if_index++) {
		
		struct rte_eth_dev_info dev_info;
		
		if (int retval = rte_eth_dev_info_get(if_index, &dev_info) != 0)
        	rte_exit(EXIT_FAILURE, "failed getting device info (port %u) info: %s\n",
			if_index, strerror(-retval));

		std::cout << "if_index: " << dev_info.if_index << std::endl;
		std::cout << "  driver_name: " << dev_info.driver_name << std::endl;
		std::cout << "  rx_queues: " << dev_info.nb_rx_queues << "/" << dev_info.max_rx_queues << std::endl;
		std::cout << "  tx_queues: " << dev_info.nb_tx_queues << "/" << dev_info.max_tx_queues << std::endl;
	}

    return 0;
}