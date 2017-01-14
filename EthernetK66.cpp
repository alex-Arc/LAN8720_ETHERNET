////////////////////////////////////////////////////////////////////////////////
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

#include "LAN8720_ETHERNET.h"
#include "IPAddress.h"
#include "etherStructs.h"
#include "core_pins.h"

LAN8720Config_t LAN8720Class::config;
size_t LAN8720Class::_len;

void LAN8720Class::init(void) {
  MPU_RGDAAC0 |= 0x007C0000;
	SIM_SCGC2 |= SIM_SCGC2_ENET;
	CORE_PIN3_CONFIG =  PORT_PCR_MUX(4); // RXD1
	CORE_PIN4_CONFIG =  PORT_PCR_MUX(4); // RXD0
	CORE_PIN24_CONFIG = PORT_PCR_MUX(2); // REFCLK
	CORE_PIN25_CONFIG = PORT_PCR_MUX(4); // RXER
	CORE_PIN26_CONFIG = PORT_PCR_MUX(4); // RXDV
	CORE_PIN27_CONFIG = PORT_PCR_MUX(4); // TXEN
	CORE_PIN28_CONFIG = PORT_PCR_MUX(4); // TXD0
	CORE_PIN39_CONFIG = PORT_PCR_MUX(4); // TXD1
	CORE_PIN16_CONFIG = PORT_PCR_MUX(4); // MDIO
	CORE_PIN17_CONFIG = PORT_PCR_MUX(4); // MDC
	SIM_SOPT2 |= SIM_SOPT2_RMIISRC | SIM_SOPT2_TIMESRC(3);
	// ENET_EIR	1356	Interrupt Event Register
	// ENET_EIMR	1359	Interrupt Mask Register
	// ENET_RDAR	1362	Receive Descriptor Active Register
	// ENET_TDAR	1363	Transmit Descriptor Active Register
	// ENET_ECR	1363	Ethernet Control Register
	// ENET_RCR	1369	Receive Control Register
	// ENET_TCR	1372	Transmit Control Register
	// ENET_PALR/UR	1374	Physical Address
	// ENET_RDSR	1378	Receive Descriptor Ring Start
	// ENET_TDSR	1379	Transmit Buffer Descriptor Ring
	// ENET_MRBR	1380	Maximum Receive Buffer Size
	//		1457	receive buffer descriptor
	//		1461	transmit buffer descriptor

	memset(config.rx_ring, 0, sizeof(config.rx_ring));
	memset(config.tx_ring, 0, sizeof(config.tx_ring));

  for (int i=0; i < RXSIZE; i++) {
		config.rx_ring[i].flags.all = 0x8000; // empty flag
		config.rx_ring[i].buffer = config.rxbufs + i * 128;
	}
	config.rx_ring[RXSIZE-1].flags.all = 0xA000; // empty & wrap flags
	for (int i=0; i < TXSIZE; i++) {
		config.tx_ring[i].buffer = config.txbufs + i * 128;
	}
	config.tx_ring[TXSIZE-1].flags.all = 0x2000; // wrap flag

	ENET_EIMR = 0;
	ENET_MSCR = ENET_MSCR_MII_SPEED(15);  // 12 is fastest which seems to work
	ENET_RCR = ENET_RCR_NLC | ENET_RCR_MAX_FL(1522) | ENET_RCR_CFEN |
		ENET_RCR_CRCFWD | ENET_RCR_PADEN | ENET_RCR_RMII_MODE |
		/* ENET_RCR_FCE | ENET_RCR_PROM | */ ENET_RCR_MII_MODE;
	ENET_TCR = ENET_TCR_ADDINS | /* ENET_TCR_RFC_PAUSE | ENET_TCR_TFC_PAUSE | */
		ENET_TCR_FDEN;
	ENET_PALR = (config.mac_h << 8) | ((config.mac_l >> 16) & 255);
	ENET_PAUR = ((config.mac_l << 16) & 0xFFFF0000) | 0x8808;
	ENET_OPD = 0x10014;
	ENET_IAUR = 0;
	ENET_IALR = 0;
	ENET_GAUR = 0;
	ENET_GALR = 0;
	ENET_RDSR = (uint32_t)config.rx_ring;
	ENET_TDSR = (uint32_t)config.tx_ring;
	ENET_MRBR = 512;
	ENET_TACC = ENET_TACC_SHIFT16;
	//ENET_TACC = ENET_TACC_SHIFT16 | ENET_TACC_IPCHK | ENET_TACC_PROCHK;
	ENET_RACC = ENET_RACC_SHIFT16 | ENET_RACC_PADREM;

	ENET_ECR = 0xF0000000 | ENET_ECR_DBSWP | ENET_ECR_EN1588 | ENET_ECR_ETHEREN;
	ENET_RDAR = ENET_RDAR_RDAR;
	ENET_TDAR = ENET_TDAR_TDAR;
}

void LAN8720Class::begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet) {
  for(int i = 0; i < 4; i++) {
    config.ip[i] = ip[i];
    config.subnetMask[i] = subnet[i];
  }
  memcpy(config.mac, mac, 6);
  config.mac_h = mac[0]<<2 | mac[1]<<1 | mac[2];
  config.mac_l = mac[3]<<2 | mac[4]<<1 | mac[5];

  LAN8720Class::init();
}

int LAN8720Class::begin(uint8_t *mac, unsigned long timeout, unsigned long responseTimeout) {
  config.mac_h = mac[0]<<2 | mac[1]<<1 | mac[2];
  config.mac_l = mac[3]<<2 | mac[4]<<1 | mac[5];

  LAN8720Class::init();

  return -1;
}

int LAN8720Class::nextPacket(void) {
  //TODO: we will always lose first packet, is that so bad?
  volatile enetbufferdesc_t *buf;
  buf = config.rx_ring + config.rxnum;
  if (config.rxnum < RXSIZE-1) {
		buf->flags.all = 0x8000;
		config.rxnum++;
	} else {
		buf->flags.all = 0xA000;
		config.rxnum = 0;
	}

  buf = config.rx_ring + config.rxnum;

  if ((buf->flags.all & 0xF73F) == 0) {
    if (buf->etherType == EtherType::IPv4) {
      return getIPv4Protocol();
    }else {
      if (getEtherType() == EtherType::ARP) {
        ARPresponse();
        return nextPacket();
      }else {
        return 0;
      }
    }
  }
  return -1;
}

uint16_t LAN8720Class::getEtherType(void) {
  volatile enetbufferdesc_t *buf;
  buf = config.rx_ring + config.rxnum;

  ethernetHeader_t *header = (ethernetHeader_t *)buf->buffer;
  return header->type;
}

int LAN8720Class::ARPresponse(void) {
  volatile enetbufferdesc_t *buf;
  buf = config.rx_ring + config.rxnum;

  arpHeader_t *header = (arpHeader_t *)buf->buffer+sizeof(ethernetHeader_t);
  if (header->HTYPE == 0x100 && header->PTYPE == EtherType::IPv4 && header->OPER == 0x0100) {
    if (memcmp(header->TPA, config.ip, 4) == 0) {
      beginPacket(header->THA, EtherType::ARP);
      arpHeader_t *ptr = (arpHeader_t*) calloc(1, sizeof(arpHeader_t));
      ptr->HTYPE = 0x0100;     //ethernet
      ptr->PTYPE = EtherType::IPv4;
      ptr->HLEN = 6;
      ptr->PLEN = 4;
      ptr->OPER = 0x0200;  //1 for request, 2 for reply.
      memcpy(ptr->SHA, config.mac, 6);
      memcpy(ptr->SPA, config.ip, 4);
      memcpy(ptr->THA, header->SHA, 6);
      memcpy(ptr->TPA, header->TPA, 4);
      writePacket(ptr, sizeof(arpHeader_t));
      endPacket();
    }
    return 0;
  }
  return -1;
}

uint8_t LAN8720Class::getIPv4Protocol(void) {
  volatile enetbufferdesc_t *buf;
  buf = config.rx_ring + config.rxnum;

  IPv4Header_t *header = (IPv4Header_t *)buf->buffer+sizeof(ethernetHeader_t);
  return header->protocol;
}

int LAN8720Class::beginPacket(uint8_t* dstMAC, uint16_t etherType) {
	if (config.txnum < TXSIZE-1) {
		config.txnum++;
	} else {
		config.txnum = 0;
	}

  enetbufferdesc_t *_txbuf = config.tx_ring + config.txnum;
  _len = 0;

  if ((_txbuf->flags.all & 0x8000) == 0) {
    ethernetHeader_t *prt = (ethernetHeader_t*) calloc(1, sizeof(ethernetHeader_t));
    prt->type = etherType;
    memcpy(prt->dstMAC, dstMAC, 6);
    writePacket(prt, sizeof(ethernetHeader_t));
    return 1;
  }
  return -1;
}

size_t LAN8720Class::writePacket(uint8_t byte) {
  uint8_t* buf = (uint8_t*)_txbuf->buffer+_len;
  memcpy(buf, &byte, 1);
  _len++;
  return 1;
}

size_t LAN8720Class::writePacket(void *buffer, size_t len) {
  uint8_t* buf = (uint8_t*)_txbuf->buffer+_len;
  memcpy(buf, buffer, len);
  _len += len;
  return 1;
}

int LAN8720Class::endPacket(void) {
  _txbuf->length = _len;
  _txbuf->flags.all |= 0x8C00;
}

IPAddress LAN8720Class::localIP() {
  return IPAddress(config.ip);
}

IPAddress LAN8720Class::subnetMask() {
  return IPAddress(config.subnetMask);
}

IPAddress LAN8720Class::gatewayIP() {
  return IPAddress(config.gateway);
}

uint8_t* LAN8720Class::getMAC(void) {
  return config.mac;
}
