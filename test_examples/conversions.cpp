#include <iostream>
#include <arpa/inet.h>  // for htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa
#include <netinet/in.h> // for sockaddr_in
#include <cstring>      // for memset

int main() {
    sockaddr_in addr;

    // Clear the struct
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    // Use inet_addr to convert IPv4 dotted string to network byte order integer
    const char* ip_str = "127.0.0.1";
    addr.sin_addr.s_addr = inet_addr(ip_str);
    if (addr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "inet_addr failed for IP: " << ip_str << std::endl;
        return 1;
    }

    // Convert port number from host byte order to network byte order
    unsigned short port_host = 8080;
    addr.sin_port = htons(port_host);

    // Print the original IP and port
    std::cout << "Original IP string: " << ip_str << std::endl;
    std::cout << "Original port (host byte order): " << port_host << std::endl;

    // Use inet_ntoa to convert network order IP to string
    char* ip_converted_back = inet_ntoa(addr.sin_addr);
    std::cout << "Converted back IP string (inet_ntoa): " << ip_converted_back << std::endl;

    // Convert port back from network to host byte order
    unsigned short port_net = addr.sin_port;
    unsigned short port_converted_back = ntohs(port_net);
    std::cout << "Port converted back (ntohs): " << port_converted_back << std::endl;

    // Show how htonl and ntohl work with an IP address (example)
    uint32_t ip_host_order = 0x7f000001;  // 127.0.0.1 in hex (host byte order)
    uint32_t ip_network_order = htonl(ip_host_order);
    uint32_t ip_back_host_order = ntohl(ip_network_order);

    std::cout << std::hex;
    std::cout << "Host order IP hex: 0x" << ip_host_order << std::endl;
    std::cout << "Network order IP hex (htonl): 0x" << ip_network_order << std::endl;
    std::cout << "Back to host order (ntohl): 0x" << ip_back_host_order << std::endl;
    std::cout << std::dec;

    return 0;
}
