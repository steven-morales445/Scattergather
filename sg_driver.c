////////////////////////////////////////////////////////////////////////////////
//
//  File           : sg_driver.c
//  Description    : This file contains the driver code to be developed by
//                   the students of the 311 class.  See assignment details
//                   for additional information.
//
//   Author        : Steven Morales
//   Last Modified : 10/7/20
//

// Include Files
#include <string.h>

// Project Includes 
#include <sg_driver.h>

// Defines

//
// Global Data

//
// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : serialize_sg_packet
// Description  : Serialize a ScatterGather packet (create packet)
//
// Inputs       : loc - the local node identifier 
//                rem - the remote node identifier
//                blk - the block identifier
//                op - the operation performed/to be performed on block
//                sseq - the sender sequence number
//                rseq - the receiver sequence number
//                data - the data block (of size SG_BLOCK_SIZE) or NULL
//                packet - the buffer to place the data
//                plen - the packet length (int bytes)
// Outputs      : 0 if successfully created, -1 if failure

SG_Packet_Status serialize_sg_packet( SG_Node_ID loc, SG_Node_ID rem, SG_Block_ID blk, 
        SG_System_OP op, SG_SeqNum sseq, SG_SeqNum rseq, char *data, char *packet, size_t *plen ) 
{   //   VAIRABLES        TYPE       VALIDATION         SIZE(bytes)
    // SG_MAGIC_VALUE - uint32_t    0xfefe = 65278           4
    // loc -            uint64_t         ≠ 0                 8
    // rem -            uint64_t         ≠ 0                 8
    // blk -            uint64_t         ≠ 0                 8
    // op -             int              0-6                 4
    // sseq -           uint16_t         ≠ 0                 2
    // rseq -           uint16_t         ≠ 0                 2
    // data -           char            Di = 1              1024
    // packet -         char             N/A         SG_BASE_PACKET_SIZE
    // plen -           int              N/A         SG_BASE_PACKET_SIZE          


//VALIDATE ALL INPUT PARAMTETERS//
    //create variable to store packet status and assign it 0 or 1-9 given if it has bad data
    SG_Packet_Status packet_status; 
    //create variable to store 0 or 1 determined if data points to null (no data) or otherwise
    uint8_t dataIndicator;
    //create a local reference to SG_Magic_Value 
    uint32_t magic = 0xfefe;

    //Validate Sender Node ID
    if (loc == 0)
    {
        packet_status = SG_PACKT_LOCID_BAD;
        logMessage(LOG_ERROR_LEVEL,"serialize_sg_packet: bad local ID [%u]", loc); 
    }
    //Validate Receiver Node ID
    else if (rem == 0)
    {
        packet_status = SG_PACKT_REMID_BAD;
        logMessage(LOG_ERROR_LEVEL,"serialize_sg_packet: bad remote ID [%u]", rem); 
    }
    //Validate Block ID
    else if (blk == 0)
    {
        packet_status = SG_PACKT_BLKID_BAD;
        logMessage(LOG_ERROR_LEVEL,"serialize_sg_packet: bad block ID [%u]", blk); 
    }
    //Validate Operation
    else if (op < 0 || op > 9)
    {
        packet_status = SG_PACKT_OPERN_BAD;
        logMessage(LOG_ERROR_LEVEL,"serialize_sg_packet: bad op code (%u)", op); 
    }
    //Validate Sender Sequence Number
    else if (sseq == 0)
    {
        packet_status = SG_PACKT_SNDSQ_BAD;
        logMessage(LOG_ERROR_LEVEL,"serialize_sg_packet: bad sender sequence number [%u]", sseq); 
    }
    //Validate Receiver Sequence Num
    else if (rseq == 0)
    {
        packet_status = SG_PACKT_RCVSQ_BAD;
        logMessage(LOG_ERROR_LEVEL,"serialize_sg_packet: bad receiver sequence number [%u]", rseq); 
    }
    //ADD PACKET STATUS 7-8-9 ERROR LATER
    else
    {
        packet_status = SG_PACKT_OK;   
    }
//END VALIDATION SECTION//

    //////////////////////////PRINT CURRENT VALUES////////////////////////////////////////////////////////
    /*logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: local ID is [%u]", loc);
    logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: remote ID is [%u]", rem);
    logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: block ID is [%u]", blk);
    logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: op code is [%u]", op);
    logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: sender sequence number is [%u]", sseq);
    logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: receiver sequence number is [%u]", rseq);
    logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: Address of data is [%p]", &data);
    logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: data is [%u]", data);*/
    //////////////////////////PRINT CURRENT VALUES////////////////////////////////////////////////////////

//CREATE PACKET//
        //NOTE: EACH TIME YOU MEMCPY A VARIABLE, YOU MUST ADD THE OFFEST TO &PACKET[0], FOUND BY ADDING ALL THE BYTES OF THE 
        //ALREADY PUSHED VARIABLES INSIDE PACKET

    //Push magic to packet (4 bytes)[offset = 0]
    memcpy(packet, &magic, sizeof(uint32_t));
    //Push loc to packet (8 bytes)[offset -> 0 + 4 = 4]
    memcpy(packet + 4, &loc, sizeof(SG_Node_ID));
    //Push rem to packet (8 bytes)[offset -> 0 + 4 + 8 = 12]
    memcpy(packet + 12, &rem, sizeof(SG_Node_ID));
    //Push blk to packet (8 bytes)[offset -> 0 + 4 + 8 + 8 = 20]
    memcpy(packet + 20, &blk, sizeof(SG_Block_ID));
    //Push op to packet (4 bytes)[offset -> 0 + 4 + 8 + 8 + 8 = 28]
    memcpy(packet + 28, &op, sizeof(SG_System_OP));
    //Push sseq to packet (2 bytes)[offset -> 0 + 4 + 8 + 8 + 8 + 4 = 32]
    memcpy(packet + 32, &sseq, sizeof(SG_SeqNum));
    //Push rseq to packet (2 bytes)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2 = 34]
    memcpy(packet + 34, &rseq, sizeof(SG_SeqNum));

    //Check data, if null, set dataindicator = 0 and don't add data to the packet
    if (data == 0)
    {       
        dataIndicator = 0;
        //Push dataIndicator to packet (1 byte)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2  + 2 = 36]
        memcpy(packet + 36, &dataIndicator, 1); 
        //Push magic to packet (4 bytes)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2  + 2 + 1 = 37]             
        memcpy(packet + 37, &magic, sizeof(uint32_t));
        //re-assign new length to *plen
        *plen = 37;
        //////////////////////////PRINT CURRENT VALUES////////////////////////////////////////////////////
        /*logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: dataIndicator = 0");
        logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: lengthOfPacket = 37 but = [%u]", *plen);*/
        //////////////////////////PRINT CURRENT VALUES////////////////////////////////////////////////////                          
    }
    //Otherwise, there is data, set dataindicator = 1 and add it to packet
    else
    {
        dataIndicator = 1;
        //Push dataIndicator to packet (1 byte)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2  + 2 = 36]
        memcpy(packet + 36, &dataIndicator, 1); //<-- COULD BE AN ISSUE
        //Push data to packet 1024 (1024 bytes)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2  + 2 + 1 = 37]
        memcpy(packet + 37, data, 1024);
        //Push magic to packet (4 bytes)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2  + 2 + 1 + 1024 = 1061]
        memcpy(packet + 1061, &magic, 4);
        //re-assign new length to *plen
        *plen = 1061;
        //////////////////////////PRINT CURRENT VALUES/////////////////////////////////////////////
        /*logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: dataIndicator = 1");
        logMessage(LOG_INFO_LEVEL, "[INFO]: serialize_sg_packet: plen = 1061 but = [%d]", *plen);*/
        //////////////////////////PRINT CURRENT VALUES/////////////////////////////////////////////
    }   
//END CREATING PACKET//

    //should return 0 if none of the validation checks failed and returned a failure
    return(packet_status); 
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : deserialize_sg_packet
// Description  : De-serialize a ScatterGather packet (unpack packet)
//
// Inputs       : loc - the local node identifier
//                rem - the remote node identifier
//                blk - the block identifier
//                op - the operation performed/to be performed on block
//                sseq - the sender sequence number
//                rseq - the receiver sequence number
//                data - the data block (of size SG_BLOCK_SIZE) or NULL
//                packet - the buffer to place the data
//                plen - the packet length (int bytes)
// Outputs      : 0 if successfully created, else return given enumerated failure #'s 

SG_Packet_Status deserialize_sg_packet( SG_Node_ID *loc, SG_Node_ID *rem, SG_Block_ID *blk, 
        SG_System_OP *op, SG_SeqNum *sseq, SG_SeqNum *rseq, char *data, char *packet, size_t plen ) 
{   //   VAIRABLES        TYPE       VALIDATION         SIZE(bytes)
    // SG_MAGIC_VALUE - uint32_t    0xfefe = 65278           4
    // loc -            uint64_t         ≠ 0                 8
    // rem -            uint64_t         ≠ 0                 8
    // blk -            uint64_t         ≠ 0                 8
    // op -             int              0-6                 4
    // sseq -           uint16_t         ≠ 0                 2
    // rseq -           uint16_t         ≠ 0                 2
    // data -           char            Di = 1              1024
    // packet -         char             N/A         SG_BASE_PACKET_SIZE
    // plen -           int              N/A         SG_BASE_PACKET_SIZE    

   
    //data indicator
    uint8_t dataIndicator;
    //store status of packing 
    SG_Packet_Status packet_status;

//UNPACK PACKETS//
    //Push loc to packet (8 bytes)[offset -> 0 + 4 = 4]
    memcpy(loc,  packet + 4, sizeof(SG_Node_ID));
    //Push rem to packet (8 bytes)[offset -> 0 + 4 + 8 = 12]
    memcpy(rem,  packet + 12 , sizeof(SG_Node_ID));
    //Push blk to packet (8 bytes)[offset -> 0 + 4 + 8 + 8 = 20]
    memcpy(blk,  packet + 20 , sizeof(SG_Block_ID));
    //Push op to packet (4 bytes)[offset -> 0 + 4 + 8 + 8 + 8 = 28]
    memcpy(op,   packet + 28 , sizeof(SG_System_OP));
    //Push sseq to packet (2 bytes)[offset -> 0 + 4 + 8 + 8 + 8 + 4 = 32]
    memcpy(sseq, packet + 32 , sizeof(SG_SeqNum));
    //Push rseq to packet (2 bytes)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2 = 34]
    memcpy(rseq, packet + 34 , sizeof(SG_SeqNum));
    //Push dataIndicator to packet (1 byte)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2  + 2 = 36]
    memcpy(&dataIndicator, packet + 36 , 1); //<-- COULD BE AN ISSUE 

    //There's data in the packet
    if(dataIndicator == 1)
    {
        //Push data to packet 1024 (1024 bytes)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2  + 2 + 1 = 37]
        memcpy(data, packet + 37 , 1024);
        //Push magic to packet (4 bytes)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2  + 2 + 1 + 1024 = 1061]
        //memcpy(&tMagicE,&packet[0] + 1061, sizeof(tMagicE));
    }

    else if (dataIndicator == 0)
    { 
        data = NULL;
        //Push End magic to packet (4 bytes)[offset -> 0 + 4 + 8 + 8 + 8 + 4 + 2  + 2 + 1 = 37]
        //memcpy(&tMagicE,&packet[0] + 37, sizeof(tMagicE));
    }
//END UNPACKING//
    //////////////////////////PRINT CURRENT VALUES////////////////////////////////////////////////////////
    /*logMessage(LOG_INFO_LEVEL, "[INFO]: deserialize_sg_packet: local ID is [%u]", *loc);
    logMessage(LOG_INFO_LEVEL, "[INFO]: deserialize_sg_packet: remote ID is [%u]", *rem);
    logMessage(LOG_INFO_LEVEL, "[INFO]: deserialize_sg_packet: block ID is [%u]", *blk);
    logMessage(LOG_INFO_LEVEL, "[INFO]: deserialize_sg_packet: op code is [%u]", *op);
    logMessage(LOG_INFO_LEVEL, "[INFO]: deserialize_sg_packet: sender sequence number is [%u]", *sseq);
    logMessage(LOG_INFO_LEVEL, "[INFO]: deserialize_sg_packet: receiver sequence number is [%u]", *rseq);
    logMessage(LOG_INFO_LEVEL, "[INFO]: deserialize_sg_packet: data indicator [%u]", dataIndicator);*/
    //////////////////////////PRINT CURRENT VALUES////////////////////////////////////////////////////////

//VALIDATE ALL INPUT PARAMTETERS//
    //Validate Sender Node ID
    if (*loc == 0)
    {
        packet_status = SG_PACKT_LOCID_BAD;
        logMessage(LOG_ERROR_LEVEL,"deserialize_sg_packet: bad local ID [%u]", *loc); 
    }
    //Validate Receiver Node ID
    else if (*rem == 0)
    {
        packet_status = SG_PACKT_REMID_BAD;
        logMessage(LOG_ERROR_LEVEL,"deserialize_sg_packet: bad remote ID [%u]", *rem); 
    }
    //Validate Block ID
    else if (*blk == 0)
    {
        packet_status = SG_PACKT_BLKID_BAD;
        logMessage(LOG_ERROR_LEVEL,"deserialize_sg_packet: bad block ID [%u]", *blk);    
    }
    //Validate Operation
    else if (*op < 0 || *op > 6)
    {
        packet_status = SG_PACKT_OPERN_BAD;
        logMessage(LOG_ERROR_LEVEL,"deserialize_sg_packet: bad op code (%u)", *op); 
    }
    //Validate Sender Sequence Number
    else if (*sseq == 0)
    {
        packet_status = SG_PACKT_SNDSQ_BAD;
        logMessage(LOG_ERROR_LEVEL,"deserialize_sg_packet: bad sender sequence number [%u]", *sseq); 
    }
    //Validate Receiver Sequence Num
    else if (*rseq == 0)
    {
        packet_status = SG_PACKT_RCVSQ_BAD;
        logMessage(LOG_ERROR_LEVEL,"deserialize_sg_packet: bad receiver sequence number [%u]", *rseq); 
    }
    //ADD PACKET STATUS 7-8-9 ERROR LATER
    else
    {
        packet_status = SG_PACKT_OK;   
    }
//END VALIDATION SECTION//

    //Should return 0 which means no corrupt data present inside packet
    return(packet_status);



}
        
        
