#include <iostream>
#include <iomanip>

#include <rte_eal.h>
#include <rte_ethdev.h>


int main(int argc, char** argv) {

	const unsigned NUM_MBUFS = 8191;
	const unsigned MBUF_CACHE_SIZE = 250;

	const unsigned RX_RING_SIZE = 1024;
	const unsigned TX_RING_SIZE = 0;

	const unsigned BURST_SIZE = 32;

	if (rte_eal_init(argc, argv) < 0) {
		rte_exit(EXIT_FAILURE, "failed initializing EAL");
	}

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

	struct rte_mempool *mbuf_pool;
	
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * port_count,
		MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

	if (mbuf_pool == NULL)
		rte_exit(EXIT_FAILURE, "failed creating mbuf pool\n");

	// configure port 0:

	unsigned use_port = 0;

	const struct rte_eth_conf port_conf_default = {
		.rxmode = {
			.max_rx_pkt_len = RTE_ETHER_MAX_LEN,
		},
	};

	struct rte_eth_conf port_conf = port_conf_default;
	const uint16_t rx_rings = 1, tx_rings = 0;
	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;

	if (!rte_eth_dev_is_valid_port(use_port))
		rte_exit(EXIT_FAILURE, "port %u is not a valid port\n", use_port);

	if ((rte_eth_dev_configure(use_port, rx_rings, tx_rings, &port_conf)) != 0)
		rte_exit(EXIT_FAILURE, "rte_eth_dev_configure failed for port %u\n", use_port);
	
	if ((rte_eth_dev_adjust_nb_rx_tx_desc(use_port, &nb_rxd, &nb_txd)) != 0)
		rte_exit(EXIT_FAILURE, "rte_eth_dev_adjust_nb_rx_tx_desc failed for port %u\n", use_port);

	for (unsigned q = 0; q < rx_rings; q++) {
		
		if ((rte_eth_rx_queue_setup(use_port, q, nb_rxd, rte_eth_dev_socket_id(use_port), NULL, mbuf_pool)) < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_rx_queue_setup failed for port %u\n", use_port);
	}

	if (rte_eth_dev_start(use_port) < 0)
		rte_exit(EXIT_FAILURE, "rte_eth_dev_start failed for port %u\n", use_port);

	// if (rte_eth_promiscuous_enable(use_port) < 0)
	// 	rte_exit(EXIT_FAILURE, "rte_eth_promiscuous_enable failed for port %u\n", use_port);

	struct rte_ether_addr addr;
	rte_eth_macaddr_get(use_port, &addr);

	std::cout << "mac addr: ";
	for (unsigned i = 0; i < 6; i++)
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned) addr.addr_bytes[i];
	
	std::cout << std::endl;

	for (;;) {
		struct rte_mbuf *bufs[BURST_SIZE];
		const uint16_t nb_rx = rte_eth_rx_burst(use_port, 0, bufs, BURST_SIZE);
		
		if (nb_rx == 0) {
			std::cout << 0 << std::endl;
			continue;
		} 

		std::cout << nb_rx << std::endl;
		
	
	}
		
		

	return 0;
}
