////////////////////////////////////////////////////////////////////////////////
// LAN8720_ETHERNET
//
// MIT License
//
// Copyright (c) 2016 Alex Christoffer Rasmussen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////


#ifndef __LAN8720_ETHERNET_h__
#define __LAN8720_ETHERNET_h__

#include "IPAddress.h"
#include "etherStructs.h"

#define RXSIZE 12
#define TXSIZE 10

typedef struct {
	uint16_t length;
  union {
    uint16_t all;
    struct {
      uint16_t TR   : 1;  //frame must be discarded. Set if the receive frame is truncated (frame length >TRUNC_FL).
      uint16_t OV   : 1;  //This field is valid only if the L field is set. A receive FIFO overrun occurred
      uint16_t CR   : 1;  //This field is valid only if the L field is set. Receive CRC or frame error.
      uint16_t Reserved0 : 1;  //Reserved
      uint16_t NO   : 1;  //This field is valid only if the L field is set. Receive non-octet aligned frame
      uint16_t LG   : 1;  //This field is valid only if the L field is set. A frame length greater than RCR[MAX_FL] was recognized.
      uint16_t MC   : 1;  //Set if is multicast and not BC.
      uint16_t BC   : 1;  //Set if is broadcast (FFFF_FFFF_FFFF).
      uint16_t M    : 1;  //This field is valid only if the L and PROM bits are set. 0 The frame was received because of an address recognition hit. 1 The frame was received because of promiscuous mode.
      uint16_t Reserved1 : 2;  //Reserved
      uint16_t L    : 1;  //last frame
      uint16_t RO2  : 1;
      uint16_t W    : 1;  //Warp
      uint16_t RO1  : 1;  //Receive software ownership. This field is reserved for use by software. This read/write field is not modified by hardware, nor does its value affect hardware.
      uint16_t E    : 1;  //0 data in buffer. 1 buffer empty.
    };
  }flags;
	void *buffer;
  union {
    uint32_t all;
    struct {
      uint8_t Reserved0  : 7;
      uint8_t INT   : 1;
      uint8_t UC    : 1;        //unicast frame
      uint8_t CE    : 1;        //Collision error The frame is invalid. Only valid if the L field is set.
      uint8_t PE    : 1;        //PHY error The frame is invalid. Only valid if the L field is set.
      uint8_t Reserved1  : 4;
      uint8_t ME    : 1;        //Erroe in memory or receive FIFO overflow. Only valid if the L field is set.
      uint8_t FRAG  : 1;        //This is a IPv4 fragment frame. Only valid if the L field is set.
      uint8_t IPV6  : 1;        //IPV6 tag. Only valid if the L field is set.
      uint8_t VLAN  : 1;        //VLAN tag. Only valid if the L field is set.
      uint8_t Reserved2  : 1;
      uint8_t PCR  : 1;         //Protocol checksum error. Only valid if the L field is set.
      uint8_t ICE  : 1;         //IP header checksum error. Only valid if the L field is set.
      uint8_t Reserved3  : 2;
      uint8_t Reserved4  : 5;
      uint8_t VPCP : 3;         //VLAN priority code 0-7
    };
  }moreflags;
	uint16_t checksum;
	uint8_t ipProtocol;
  uint8_t etherType :4;
	uint32_t dmadone;
	uint32_t timestamp;
	uint32_t unused1;
	uint32_t unused2;
} enetbufferdesc_t;

typedef struct LAN8720Config_s {
  uint8_t ip[4];
  uint8_t subnetMask[4];
  uint8_t netPrefix[4];
  uint8_t gateway[4];
  uint8_t dns[4];
  uint32_t mac_h;
  uint32_t mac_l;
  uint8_t mac[6];
  enetbufferdesc_t rx_ring[RXSIZE] __attribute__ ((aligned(16)));
  enetbufferdesc_t tx_ring[TXSIZE] __attribute__ ((aligned(16)));
  uint32_t rxbufs[RXSIZE*128] __attribute__ ((aligned(16)));
  uint32_t txbufs[TXSIZE*128] __attribute__ ((aligned(16)));
  int rxnum;
  int txnum;
} LAN8720Config_t;

class LAN8720Class {
private:
  static LAN8720Config_t config;
  static void init(void);
  static size_t _len;
  static enetbufferdesc_t *_txbuf;
public:
  // LAN8720Class();
  static int begin(uint8_t *mac, unsigned long timeout = 60000, unsigned long responseTimeout = 4000);
  //static void begin(uint8_t *mac, IPAddress ip);
  //static void begin(uint8_t *mac, IPAddress ip, IPAddress dns);
  //static void begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway);
  static void begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);

  int nextPacket(void);
  int ARPresponse(void);
  uint16_t getEtherType(void);
  uint8_t  getIPv4Protocol(void);

  int beginPacket(uint8_t* dstMAC, uint16_t etherType);
  size_t writePacket(uint8_t byte);
  size_t writePacket(void *buffer, size_t len);
  int endPacket(void);

  static IPAddress localIP();
  static IPAddress subnetMask();
  static IPAddress gatewayIP();
//  static IPAddress dnsServerIP() { return _dnsServerAddress; }
  uint8_t* getMAC(void);

  //friend class DhcpClass;
};

extern LAN8720Class Ethernet;

#endif
