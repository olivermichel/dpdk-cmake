#include <chrono>
#include <csignal>
#include <cstdio>
#include <iomanip>
#include <iostream>

#include <cxxopts/cxxopts.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_ip.h>
#include <rte_udp.h>

#include "tools.h"

namespace tx {
	struct config {
		bool verbose = false;
        std::string mac_dst;
        std::string ip_src;
        std::string ip_dst;
	};

	void print_help(cxxopts::Options& opts_, int exit_code_ = 0)
	{
		std::ostream& os = (exit_code_ ? std::cerr : std::cout);
		os << opts_.help({""}) << std::endl;
		exit(exit_code_);
	}

	cxxopts::Options set_options()
	{
		cxxopts::Options opts("tx", "");

		opts.add_options()
            ("m,mac-dst", "mac destination address", cxxopts::value<std::string>(), "MAC-ADDR")
            ("s,ip-src", "ip source address", cxxopts::value<std::string>(), "IP-ADDR")
            ("d,ip-dst", "ip destination address", cxxopts::value<std::string>(), "IP-ADDR")
			("v,verbose", "print intermediate output (optional)")
			("h,help", "print this help message");

		return opts;
	}

	config parse_options(cxxopts::Options opts_, int argc_, char** argv_)
	{
		config config {};
		auto parsed = opts_.parse(argc_, argv_);

        if (parsed.count("m"))
            config.mac_dst = parsed["m"].as<std::string>();
        else
            tx::print_help(opts_, 1);

        if (parsed.count("s"))
            config.ip_src = parsed["s"].as<std::string>();
        else
            tx::print_help(opts_, 1);

        if (parsed.count("d"))
            config.ip_dst = parsed["d"].as<std::string>();
        else
            tx::print_help(opts_, 1);

		if (parsed.count("h"))
			print_help(opts_);

		config.verbose = (bool) parsed.count("v");

		return config;
	}

    bool tx = true;
    unsigned long tx_count = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> tx_start;

    static void sig_handler(int sig_)
    {
        tx = false;
        auto end = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - tx_start);
        double time_s = (double) dur.count() / 1000000;

        std::cout << std::endl;
        std::cout << "packets:          " << tx_count << std::endl;
        std::cout << "duration [s]:     " << time_s << std::endl;
        std::cout << "throughput [pps]: " << tx_count / time_s << std::endl;
    }
}

int main(int argc_, char** argv_)
{
	const unsigned BURST_SIZE   = 16;
	const unsigned TX_RING_SIZE = 1024;
    const unsigned PORT_INDEX   = 0;

    signal(SIGINT, tx::sig_handler);

    auto eal_init_status = dpdk_tools::init_eal(argc_, argv_);
    auto mbuf_pool = dpdk_tools::init_mbuf_pool();
    dpdk_tools::init_port(PORT_INDEX, mbuf_pool, 0, 1);
    dpdk_tools::start_port(PORT_INDEX);

    argc_ -= eal_init_status;
	argv_ += eal_init_status;

    auto config = tx::parse_options(tx::set_options(), argc_, argv_);

    auto mac_src = dpdk_tools::rte_ether_addr_from_port(PORT_INDEX);
    auto mac_dst = dpdk_tools::rte_ether_addr_from_string(config.mac_dst);
    auto ip_src  = dpdk_tools::ipv4_addr_from_string(config.ip_src);
    auto ip_dst  = dpdk_tools::ipv4_addr_from_string(config.ip_dst);

    auto ipv4_hdr_offset = sizeof(struct rte_ether_hdr);
    auto udp_hdr_offset  = ipv4_hdr_offset + sizeof(struct rte_ipv4_hdr);

    struct rte_mbuf* pkt[BURST_SIZE] = { nullptr };

    rte_ether_hdr eth_hdr = {0};
    rte_ether_addr_copy(&mac_dst, &eth_hdr.d_addr);
    rte_ether_addr_copy(&mac_src, &eth_hdr.s_addr);
    eth_hdr.ether_type = rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4);

    rte_ipv4_hdr ipv4_hdr = {0};
    auto pkt_len = (uint16_t) (sizeof(struct rte_udp_hdr) + sizeof(struct rte_ipv4_hdr));
    ipv4_hdr.version_ihl     = 0x45;
    ipv4_hdr.time_to_live    = 64;
    ipv4_hdr.next_proto_id   = 17;
    ipv4_hdr.total_length    = rte_cpu_to_be_16(pkt_len);
    ipv4_hdr.src_addr        = rte_cpu_to_be_32(ip_src);
    ipv4_hdr.dst_addr        = rte_cpu_to_be_32(ip_dst);
    ipv4_hdr.hdr_checksum    = rte_ipv4_cksum(&ipv4_hdr);

    rte_udp_hdr udp_hdr = {0};
    udp_hdr.src_port    = rte_cpu_to_be_16(42742);
    udp_hdr.dst_port    = rte_cpu_to_be_16(42742);
    udp_hdr.dgram_len   = rte_cpu_to_be_16(sizeof(struct rte_udp_hdr));

    for (auto i = 0; i < BURST_SIZE; i++) {
        pkt[i] = rte_pktmbuf_alloc(mbuf_pool);
        pkt[i]->data_len = pkt[i]->pkt_len = sizeof(struct rte_ether_hdr)
            + sizeof(struct rte_ipv4_hdr) + sizeof(struct rte_udp_hdr);

        auto ether_mbuf_offset = rte_pktmbuf_mtod_offset(pkt[i], char*, 0);
        auto ipv4_mbuf_offset  = rte_pktmbuf_mtod_offset(pkt[i], char*, ipv4_hdr_offset);
        auto udp_mbuf_offset   = rte_pktmbuf_mtod_offset(pkt[i], char*, udp_hdr_offset);

        rte_memcpy(ether_mbuf_offset, &eth_hdr, sizeof(rte_ether_hdr));
        rte_memcpy(ipv4_mbuf_offset, &ipv4_hdr, sizeof(rte_ipv4_hdr));
        rte_memcpy(udp_mbuf_offset, &udp_hdr, sizeof(rte_udp_hdr));
        rte_pktmbuf_dump(stdout, pkt[i], pkt[i]->pkt_len);
    }

    tx::tx_start = std::chrono::high_resolution_clock::now();
    while(tx::tx) tx::tx_count += rte_eth_tx_burst(PORT_INDEX, 0, pkt, BURST_SIZE);

    return 0;
}
