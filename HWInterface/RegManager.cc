/*

    FileName :                    RegManager.cc
    Content :                     RegManager class, permit connection & r/w registers
    Programmer :                  Nicolas PIERRE
    Version :                     1.3
    Date of creation :            06/06/14
    Support :                     mail to : nicolas.pierre@cern.ch

*/
#include <uhal/uhal.hpp>
#include <boost/format.hpp>
#include "RegManager.h"

#define DEV_FLAG    1

namespace Ph2_HwInterface
{

    RegManager::RegManager(const char *puHalConfigFileName)
    {
        fUHalConfigFileName = puHalConfigFileName;

        uhal::ConnectionManager cm( fUHalConfigFileName ); // Get connection

        for(int i=0;i<1;i++)
        {
            fBoardMap.insert(std::pair<uint8_t,uhal::HwInterface*>(i,new uhal::HwInterface(cm.getDevice( boost::str( boost::format("board%1%") % i )))));
        }

    }


    RegManager::~RegManager()
    {
        for(std::map<uint8_t,uhal::HwInterface*>::iterator cIt = fBoardMap.begin(); cIt != fBoardMap.end(); cIt++)
        {
            delete cIt->second;
        }
    }


    bool RegManager::WriteReg(const std::string& pRegNode, const uint32_t& pVal)
    {
        fBoard->getNode(pRegNode).write(pVal);
        fBoard->dispatch();

        // Verify if the writing is done correctly
        if (DEV_FLAG)
        {
            uhal::ValWord<uint32_t> reply = fBoard->getNode(pRegNode).read();
            fBoard->dispatch();

            uint32_t comp = (uint32_t) reply ;

            if(comp==pVal)
            {
                std::cout << "Values written correctly !" << comp << "=" << pVal << std::endl;
                return true;
            }

            std::cout << "\nERROR !!\nValues are not consistent : \nExpected : " << pVal << "\nActual : " << comp << std::endl;
        }

        return false;
    }


    bool RegManager::WriteStackReg(std::vector< std::pair<std::string,uint32_t> >& pVecReg)
    {
        for(std::vector< std::pair<std::string,uint32_t> >::iterator cIt = pVecReg.begin(); cIt != pVecReg.end(); cIt++)
        {
            fBoard->getNode(cIt->first).write(cIt->second);
        }
        fBoard->dispatch();

        if (DEV_FLAG)
        {
            int cNbErrors = 0;
            uint32_t comp;

            for(std::vector< std::pair<std::string,uint32_t> >::iterator cIt = pVecReg.begin(); cIt != pVecReg.end(); cIt++)
            {
                uhal::ValWord<uint32_t> reply = fBoard->getNode(cIt->first).read();
                fBoard->dispatch();

                comp = (uint32_t) reply ;

                if(comp==(cIt->second))
                {
                    std::cout << "Values written correctly !" << comp << "=" << cIt->second << std::endl;
                }
            }

            if(cNbErrors == 0)
            {
                std::cout << "All values written correctly !" << std::endl;
                return true;
            }

            std::cout << "\nERROR !!\n" << cNbErrors << " have not been written correctly !" << std::endl;
        }

        return false;
    }


    bool RegManager::WriteBlockReg(const std::string& pRegNode, const std::vector< uint32_t >& pValues)
    {
        fBoard->getNode(pRegNode).writeBlock(pValues);
        fBoard->dispatch();

        bool cWriteCorr = true;

          //Verifying block
          if (DEV_FLAG)
          {
              int cErrCount = 0;

              uhal::ValVector<uint32_t> cBlockRead = fBoard->getNode(pRegNode).readBlock(pValues.size());
              fBoard->dispatch();

              //Use size_t and not an iterator as op[] only works with size_t type
              for(std::size_t i = 0; i != cBlockRead.size(); i++ )
              {
                  if(cBlockRead[i]!=pValues[i])
                  {
                      cWriteCorr = false;
                      cErrCount++;
                  }
              }

              std::cout << "Block Write finished !!\n" << cErrCount << " values failed to write !" << std::endl;
          }

        return cWriteCorr;
    }


    uhal::ValWord<uint32_t> RegManager::ReadReg(const std::string& pRegNode)
    {
        uhal::ValWord<uint32_t> cValRead = fBoard->getNode(pRegNode).read();
        fBoard->dispatch();

        if (DEV_FLAG)
        {
            uint32_t read = (uint32_t) cValRead ;
            std::cout << "\nValue in register ID " << pRegNode << " : " << read << std::endl;
        }

        return cValRead;
    }


    uhal::ValVector<uint32_t> RegManager::ReadBlockReg(const std::string& pRegNode, const uint32_t& pBlockSize)
    {
        uint32_t read;

        uhal::ValVector<uint32_t> cBlockRead = fBoard->getNode(pRegNode).readBlock(pBlockSize);
        fBoard->dispatch();

        if (DEV_FLAG)
        {
            std::cout << "\nValues in register block " << pRegNode << " : " << std::endl;

            //Use size_t and not an iterator as op[] only works with size_t type
            for(std::size_t i = 0; i != cBlockRead.size(); i++ )
            {
                read = (uint32_t) cBlockRead[i];
                std::cout << " " << read << " " << std::endl;
            }
        }

        return cBlockRead;
    }

    void RegManager::ChooseBoard(uint8_t pBoardId)
    {
      fBoard = fBoardMap[pBoardId];
    }

}
