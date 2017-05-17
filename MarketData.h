#include <iostream>

#define HEADERSIZE  4

typedef struct header
{
  __int16 PacketLength;
  __int16 NumMarketUpdates;

}HEADER;

struct QuoteData
{
  __int16 PacketLength;
  char    type;
  char    Symbol[5];
  __int16 PriceLevel;
  __int16 PriceLevelSize;
  double  PriceLevelPrice;

};

struct TradeData
{
  __int16 PacketLength;
  char    type;
  char    Symbol[5];
  __int16 TradeSize;
  double  TradePrice;

};

class MarketData
{
  //int *buf;

public:
  MarketData(){}
  ~MarketData(){}

  bool readFromFile();
  bool writeToFile();
};