
#include <rte_eal.h>
#include <rte_ethdev.h>

namespace dpdk_tools {

	int init_eal(int argc_, char** argv_)
	{
		int init_status = rte_eal_init(argc_, argv_);

		if (init_status < 0)
			throw std::runtime_error("rte_eal_init: failed initializing eal");

		return init_status;
	}

	struct rte_mempool* init_mbuf_pool(unsigned num_mbufs_ = 8191, unsigned port_count_ = 1,
		unsigned mbuf_cache_size_ = 250)
	{
		struct rte_mempool* mbuf_pool = nullptr;

		mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", num_mbufs_ * port_count_,
			mbuf_cache_size_, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

		if (mbuf_pool == nullptr)
			throw std::runtime_error("rte_pktmbuf_pool_create: failed");
	
		return mbuf_pool;
	}

	void init_port(unsigned port_index_, struct rte_mempool* mbuf_pool_, unsigned rx_rings_ = 1, unsigned tx_rings_ = 1)
	{
		const unsigned RX_RING_SIZE = 1024;
		const unsigned TX_RING_SIZE = 1024;

		struct rte_eth_conf port_conf_default;
		port_conf_default.rxmode.max_rx_pkt_len = RTE_ETHER_MAX_LEN;
		port_conf_default.rxmode.mq_mode        = ETH_MQ_RX_NONE;
		port_conf_default.rxmode.offloads       = 0;
		port_conf_default.txmode.mq_mode        = ETH_MQ_TX_NONE;
		port_conf_default.txmode.offloads       = 0;
		
		struct rte_eth_conf port_conf = port_conf_default;
		uint16_t nb_rxd = RX_RING_SIZE;
		uint16_t nb_txd = TX_RING_SIZE;

		if (!rte_eth_dev_is_valid_port(port_index_))
			throw std::invalid_argument("rte_eth_dev_is_valid_port: invalid port index");

		if ((rte_eth_dev_configure(port_index_, rx_rings_, tx_rings_, &port_conf)) != 0)
			throw std::runtime_error("rte_eth_dev_configure: failed");
			
		if ((rte_eth_dev_adjust_nb_rx_tx_desc(port_index_, &nb_rxd, &nb_txd)) != 0)
			throw std::runtime_error("rte_eth_dev_adjust_nb_rx_tx_descure: failed");

		auto socket_id = rte_eth_dev_socket_id(port_index_);

		for (unsigned q = 0; q < rx_rings_; q++) {
			if ((rte_eth_rx_queue_setup(port_index_, q, nb_rxd, socket_id, nullptr, mbuf_pool_)) < 0) {
				throw std::runtime_error("rte_eth_rx_queue_setup: failed");
			}
		}

		for (unsigned q = 0; q < tx_rings_; q++) {
			if ((rte_eth_tx_queue_setup(port_index_, q, nb_txd, socket_id, nullptr)) < 0) {
				throw std::runtime_error("rte_eth_tx_queue_setup: failed");
			}
		}
	}

	void start_port(unsigned port_index_)
	{
		if (rte_eth_dev_start(port_index_) < 0)
			throw std::runtime_error("rte_eth_dev_start: failed");
	}

	struct rte_ether_addr rte_ether_addr_from_port(unsigned port_index_)
	{
		struct rte_ether_addr addr;
		if (rte_eth_macaddr_get(port_index_, &addr) != 0)
			throw std::runtime_error("rte_eth_macaddr_get: failed");
		return addr;
	}

	struct rte_ether_addr rte_ether_addr_from_string(const std::string& s_)
	{
		struct rte_ether_addr addr;

		int bytes = std::sscanf(s_.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			addr.addr_bytes + 0, 
			addr.addr_bytes + 1,
			addr.addr_bytes + 2,
			addr.addr_bytes + 3,
			addr.addr_bytes + 4,
			addr.addr_bytes + 5);

		if (bytes != RTE_ETHER_ADDR_LEN)
			throw std::invalid_argument("invalid mac address");

		return addr;
	}
}
