// Netify Daemon
// Copyright (C) 2015-2016 eGloo Incorporated <http://www.egloo.ca>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _ND_FLOW_H
#define _ND_FLOW_H

#define ND_FLOW_SSL_CERTLEN	48      // SSL certificate length

struct ndFlow
{
    uint8_t ip_version;
    uint8_t ip_protocol;

    uint16_t vlan_id;
    uint64_t ts_last_seen;

    ndNetlinkAddressType lower_type;
    ndNetlinkAddressType upper_type;

    uint8_t lower_mac[ETH_ALEN];
    uint8_t upper_mac[ETH_ALEN];

//    struct sockaddr_storage lower_addr;
//    struct sockaddr_storage upper_addr;

    struct in_addr lower_addr;
    struct in_addr upper_addr;

    struct in6_addr lower_addr6;
    struct in6_addr upper_addr6;

    char lower_ip[INET6_ADDRSTRLEN];
    char upper_ip[INET6_ADDRSTRLEN];

    uint16_t lower_port;
    uint16_t upper_port;

    uint64_t lower_bytes;
    uint64_t upper_bytes;
    uint64_t total_bytes;

    uint32_t lower_packets;
    uint32_t upper_packets;
    uint32_t total_packets;

    bool detection_complete;
    bool detection_guessed;

    ndpi_protocol detected_protocol;

    struct ndpi_flow_struct *ndpi_flow;

    struct ndpi_id_struct *id_src;
    struct ndpi_id_struct *id_dst;

    char host_server_name[HOST_NAME_MAX];
    struct {
        char client_cert[ND_FLOW_SSL_CERTLEN];
        char server_cert[ND_FLOW_SSL_CERTLEN];
    } ssl;

    void hash(const string &device, string &digest, bool full_hash = false);

    inline bool operator==(const ndFlow &f) const {
        if (lower_port != f.lower_port || upper_port != f.upper_port) return false;
        switch (ip_version) {
        case 4:
            if (memcmp(&lower_addr, &f.lower_addr, sizeof(struct in_addr)) == 0 &&
                memcmp(&upper_addr, &f.upper_addr, sizeof(struct in_addr)) == 0)
                return true;
            break;
        case 6:
            if (memcmp(&lower_addr6, &f.lower_addr6, sizeof(struct in6_addr)) == 0 &&
                memcmp(&upper_addr6, &f.upper_addr6, sizeof(struct in6_addr)) == 0)
                return true;
            break;
        }
        return false;
    }

    inline void release(void) {
        if (ndpi_flow != NULL) { ndpi_free_flow(ndpi_flow); ndpi_flow = NULL; }
        if (id_src != NULL) { delete id_src; id_src = NULL; }
        if (id_dst != NULL) { delete id_dst; id_dst = NULL; }
    }

    void print(const char *tag, struct ndpi_detection_module_struct *ndpi);

    json_object *json_encode(const string &device, ndJson &json,
        struct ndpi_detection_module_struct *ndpi, bool counters = true);
};

typedef unordered_map<string, struct ndFlow *> nd_flow_map;
typedef map<string, nd_flow_map *> nd_flows;
typedef pair<string, struct ndFlow *> nd_flow_pair;
typedef pair<nd_flow_map::iterator, bool> nd_flow_insert;

#endif // _ND_FLOW_H
// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4
