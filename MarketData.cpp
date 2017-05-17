#include <MarketData.h>
#include <vector>
#include <string>
#include <windows.h>
#include <fstream>
using namespace std;

double toDouble(unsigned __int8 *buffer)
{
  return  double((unsigned __int8)(buffer[0]) << 56 |
                 (unsigned __int8)(buffer[1]) << 48 |
                 (unsigned __int8)(buffer[2]) << 40 |
                 (unsigned __int8)(buffer[3]) << 32 |
                 (unsigned __int8)(buffer[4]) << 24 |
                 (unsigned __int8)(buffer[5]) << 16 |
                 (unsigned __int8)(buffer[6]) << 8  |
                 (unsigned __int8)(buffer[7]));
}

unsigned __int32 toInt32(unsigned __int8 *buffer)
{
  return  unsigned __int32((unsigned __int8)(buffer[0]) << 24 |
                           (unsigned __int8)(buffer[1]) << 16 |
                           (unsigned __int8)(buffer[2]) << 8 |
                           (unsigned __int8)(buffer[3]));
}

unsigned __int16 toInt16(unsigned __int8 *buffer)
{
  return  unsigned __int16((unsigned __int8)(buffer[0]) << 8 |
                           (unsigned __int8)(buffer[1]));
}

bool MarketData::readFromFile()
{
  HEADER   header;
  //unsigned __int32 offset = 0;
  ifstream fileIn("input.dat", ios::in|ios::binary);
  ofstream fileOut("output.txt", ios::out);

  unsigned __int8 buf[10] = {0};
  char Symbol[5] = {'\0'};
  int i = 0;
  std::vector<__int8>::iterator mainIt;
  std::vector<__int8>::iterator it;

  if (!fileIn)
  {
    cerr << "File failed to open" << endl;
    exit(1);
  }
  else
  {
    cout << "File Opened Successfully " << endl;
  }

  /*
  
  |---- Packet Length -----|--- no. of market updates ---|--- QUOTE or TRADE ---|--- QUOTE or TRADE ---| ......
  |------------------- 4 byte header --------------------|

  */
  
  unsigned __int32 hostOrd = 0;

  std::vector<__int8> contents((std::istreambuf_iterator<char>(fileIn)), std::istreambuf_iterator<__int8>());

  cout<<contents.size()<<endl;

  mainIt = contents.begin();
  it = contents.begin();

  while(mainIt != contents.end())
  {
    i = 0;
    for(it = mainIt; it != mainIt + HEADERSIZE; it++)
    {
      buf[i++] = (*it);
    }
  
    hostOrd = toInt32(buf);
    //cout<<(hostOrd)<<endl;
    header.PacketLength = (hostOrd>>16);
    header.NumMarketUpdates = (hostOrd & 0x00FF);
    //cout << header.PacketLength << endl;
    //cout << header.NumMarketUpdates << endl;

    int loop = 0;
    for(loop = 0; loop < header.NumMarketUpdates; loop++)
    {
      memset(buf, 0, 10);
      buf[0] = (*it); it++;
      buf[1] = (*it); it++;

      unsigned __int16 DataLen = toInt16(buf);
      //cout <<"Packet size --> "<< DataLen << endl;

      buf[3] = (*it); it++;  

      char cType = (char)buf[3];
      //cout <<"Market Data Type --> " <<cType<<endl;

      if('Q' == cType)
      {        
        memset(buf, 0, 10);
        for(i = 0; i < 5; it++)
        {
          buf[i++] = (*it);
        }
        buf[i] = '\0';
        
        memset(Symbol, 0, 5);
        for(i = 0; i < 4; i++)
        {
          Symbol[i] = (char)buf[i];
        }
        Symbol[i] = '\0';

        //cout<<"Quote Ticker --> "<<Symbol<<endl;

        buf[0] = (*it); it++;
        buf[1] = (*it); it++;

        unsigned __int16 PriceLevel = toInt16(buf);
        //cout <<"Price Level --> "<< PriceLevel << endl;

        buf[0] = (*it); it++;
        buf[1] = (*it); it++;

        unsigned __int16 PriceLevelSize = toInt16(buf);
        //cout <<"Price Level Size --> "<< PriceLevelSize * 100 << endl;

        for(i = 0; i < 8; it++)
        {
          buf[i++] = (*it);
        }

        double PriceLevelPrice = toDouble(buf);
        //cout <<"Price Level Price --> "<< (PriceLevelPrice / 10000) << endl;

        int offset = DataLen;
        for(offset = 0; offset < (DataLen - 20); offset++)
        {
          it++;
        }
      }
      else
      {        
        memset(buf, 0, 10);
        for(i = 0; i < 5; it++)
        {
          buf[i++] = (*it);
        }
        buf[i] = '\0';
        
        memset(Symbol, 0, 5);
        for(i = 0; i < 4; i++)
        {
          Symbol[i] = (char)buf[i];
        }
        Symbol[i] = '\0';

        //cout<<"Quote Ticker --> "<<Symbol<<endl;
        
        buf[0] = (*it); it++;
        buf[1] = (*it); it++;

        unsigned __int16 TradeSize = toInt16(buf);
        //cout <<"Trade size --> "<< TradeSize * 100<< endl;

        for(i = 0; i < 8; it++)
        {
          buf[i++] = (*it);
        }

        double TradePrice = toDouble(buf) / 10000;
        //cout <<"Trade Price --> "<< TradePrice << endl;

        int offset = DataLen;
        for(offset = 0; offset < (DataLen - 18); offset++)
        {
          it++;
        }
        fileOut << TradeSize * 100 <<" "<< Symbol <<" @ " << TradePrice <<endl;
      }
      
      //cout<<"-------------- Next quote or trade -----------------"<<endl;
    }
    mainIt = mainIt + header.PacketLength;
  }

  fileIn.close();
  fileOut.close();

  return true;
}

bool MarketData::writeToFile()
{
  //ofstream fileOut("output.txt", ios::out);
  //fileOut << "Writing this to a file.\n";
  //fileOut.close();  

  return true;
}

int main()
{
  MarketData mObj;

  mObj.readFromFile();
  //mObj.writeToFile();
}