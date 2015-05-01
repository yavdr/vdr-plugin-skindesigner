#ifndef __DVBAPISERVICES_H
#define __DVBAPISERVICES_H

struct sDVBAPIEcmInfo {
  //in parameters
  uint16_t sid;
  //out parameters
  uint16_t caid;
  uint16_t pid;
  uint32_t prid;
  uint32_t ecmtime;
  cString cardsystem;
  cString reader;
  cString from;
  cString protocol;
  int8_t hops;
};

#endif //__DVBAPISERVICES_H