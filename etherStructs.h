#ifndef __etherStructs_h__
#define __etherStructs_h__

enum EtherType: uint16_t {
  IPv4  = 0x0008,
  ARP   = 0x0608
};

typedef struct ethernetHeader_s {
  uint16_t   pad;
  uint8_t   dstMAC[6];
  uint8_t   srcMAC[6];
  uint16_t  type;
} ethernetHeader_t;

typedef struct dhcpHeader_s {
  uint8_t   OP;
  uint8_t   HTYPE;
  uint8_t   HLEN;
  uint8_t   HOPS;
  uint16_t  XID;
  uint8_t   SECS;
  uint8_t   FLAGS;
  uint16_t  CIADDR;
  uint16_t  YIADDR;
  uint16_t  SIADDR;
  uint16_t  GIADDR;
  //uint16_t  YIADDR;
  uint8_t   CHADDR[6];
  uint16_t  MagicCookie;
} dhcpHeader_t;

typedef struct IPv4Header_s {
  //T_EthernetHeader  ethernetHeader;
  struct {
    uint8_t     IHL       : 4;
    uint8_t     version   : 4;
  }Version_IHL;
  struct {
    uint8_t     ECN       :2;
    uint8_t     DSCP      :6;
  }DSCP_ECN;
  uint16_t    totalLength;
  uint16_t    ID;
  uint16_t    Flags_FragmentOffset;
  uint8_t     TTL;
  uint8_t     protocol;
  uint16_t    headerChecksum;
  uint8_t     srcIP[4];
  uint8_t     dstIP[4];
  uint32_t    options[4];
} IPv4Header_t;

typedef struct S_arp{
  uint16_t  HTYPE;    //Hardware type
  uint16_t  PTYPE;    //Protocol type
  uint8_t   HLEN;     //Hardware length. lenght hardware address is 6
  uint8_t   PLEN;     //Protocol length. length of IPv4 addr is 4
  uint16_t  OPER;     //Operation 1 for request, 2 for reply
  uint8_t   SHA[6];   //sender hardware address (MAC)
  uint8_t   SPA[4];   //Sender protocol address (IP)
  uint8_t   THA[6];    //Target hardware address (MAC)
  uint8_t   TPA[4];    //Target protocol address (IP)
}T_arp;

typedef struct S_UDPHeader {
  uint16_t          srcPort;
  uint16_t          dstPort;
  uint16_t          length;
  uint16_t          checksum;
} T_UDPHeader;

#endif
