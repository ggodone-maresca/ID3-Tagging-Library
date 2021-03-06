/***********************************************************************
 * ID3-Tagging-Library Copyright (C) 2016 Gerard Godone-Maresca        *
 * This library comes with ABSOLUTELY NO WARRANTY; for details open    *
 * the document 'README.txt' found enclosed.                           *
 * This is free software, and you are welcome to redistribute it under *
 * certain conditions.                                                 *
 *                                                                     *
 * @author Gerard Godone-Maresca                                       *
 * @copyright Gerard Godone-Maresca, 2016, GNU Public License v3       *
 * @link https://github.com/ggodone-maresca/ID3-Tagging-Library        *
 **********************************************************************/

#include "ID3FrameFactory.hpp"            //For the class definition
#include "Frames/ID3TextFrame.hpp"        //For TextFrame
#include "Frames/ID3PictureFrame.hpp"     //For PictureFrame and PictureType
#include "Frames/ID3PlayCountFrame.hpp"   //For PlayCountFrame
#include "Frames/ID3EventTimingFrame.hpp" //For EventTimingFrame
#include "ID3Functions.hpp"               //For translating numbers from char arrays to ints and vice verse
#include "ID3Constants.hpp"               //For constants such as HEADER_BYTE_SIZE

using namespace ID3;

///@pkg ID3FrameFactory.h
FrameFactory::FrameFactory(std::istream&  file,
                           const ushort   version,
                           const ulong    tagEnd) : musicFile(&file),
                                                    ID3Ver(version),
                                                    ID3Size(tagEnd) {}

///@pkg ID3FrameFactory.h	                                              
FrameFactory::FrameFactory(const ushort version) : musicFile(nullptr),
                                                   ID3Ver(version),
                                                   ID3Size(0) {}

///@pkg ID3FrameFactory.h	                                              
FrameFactory::FrameFactory() : musicFile(nullptr),
                               ID3Ver(WRITE_VERSION),
                               ID3Size(0) {}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::create(const ulong readpos) const {
	//Validate the file
	if(readpos + HEADER_BYTE_SIZE > ID3Size || musicFile == nullptr || !musicFile->good())
		return FramePtr(new UnknownFrame());
	
	//Seek to the read position
	musicFile->seekg(readpos, std::ifstream::beg);
	if(musicFile->fail()) return FramePtr(new UnknownFrame());
	
	//The Frame class that should be returned
	FrameClass frameType;
	
	//The ByteArray of the frame's bytes read from the file
	ByteArray frameBytes;
	
	//The ID3v2 frame ID that will be read from file
	FrameID id;
	
	//ID3v2.2 and below have a different frame header structure, so they need to
	//be read differently
	if(ID3Ver >= 3) {
		//Read the frame header
		FrameHeader header;
		musicFile->read(reinterpret_cast<char*>(&header), HEADER_BYTE_SIZE);
		
		//Get the size of the frame
		ulong frameSize = byteIntVal(header.size, 4, ID3Ver >= 4);
		
		//Validate the frame size
		if(frameSize == 0 || frameSize + HEADER_BYTE_SIZE > ID3Size)
			return FramePtr(new UnknownFrame());
		
		//Get the frame ID
		id = terminatedstring(header.id, 4);
		
		//Get the class the Frame should be
		frameType = FrameFactory::frameType(id);
		
		//Create the ByteArray with the entire frame contents
		frameBytes = ByteArray(frameSize + HEADER_BYTE_SIZE, '\0');
		musicFile->seekg(readpos, std::ifstream::beg);
		if(musicFile->fail()) return FramePtr(new UnknownFrame(id));
		musicFile->read(reinterpret_cast<char*>(&frameBytes.front()), frameSize + HEADER_BYTE_SIZE);
	} else {
		//The ID3v2.2 frame header has 6 bytes instead of 10
		const ushort OLD_FRAME_HEADER_BYTE_SIZE = sizeof(V2FrameHeader);
		
		//Read the frame header
		V2FrameHeader header;
		musicFile->read(reinterpret_cast<char*>(&header), OLD_FRAME_HEADER_BYTE_SIZE);
		
		//Get the size of the frame
		ulong frameSize = byteIntVal(header.size, 3, false);
		
		//Validate the frame size
		if(frameSize == 0 || frameSize + OLD_FRAME_HEADER_BYTE_SIZE > ID3Size)
			return FramePtr(new UnknownFrame());
		
		//Get the ID3v2.2 frame ID, and then convert it to its ID3v2.4 equivalent
		id = FrameID(terminatedstring(header.id, 4), ID3Ver);
		
		//Get the class the Frame should be
		frameType = FrameFactory::frameType(id);
		
		//Create the ByteArray with room for the entire frame content, if it were
		//a new ID3v2 tag
		frameBytes = ByteArray(frameSize + HEADER_BYTE_SIZE, '\0');
		musicFile->seekg(readpos, std::ifstream::beg);
		if(musicFile->fail()) return FramePtr(new UnknownFrame(id));
		
		//Get the frame bytes, reserving the first four bytes in the ByteArray
		musicFile->read(reinterpret_cast<char*>(&frameBytes.front()+4), frameSize + OLD_FRAME_HEADER_BYTE_SIZE);
		
		//===========================================
		//Reconstruct the header as an ID3v2.4 header
		//===========================================
		
		//Convert the ID to its ID3v2.4 equivalent, and save them to the currently
		//unused first four bytes of the frame
		for(ushort i = 0; i < 4; i++) frameBytes[i] = id[i];
		
		//Convert the ID3v2.2 non-synchsafe 3-byte frame size to the ID3v2.4
		//synchsafe 4-byte frame size
		ByteArray v4Size = intToByteArray(frameSize, 4, true);
		//And save it to the frame bytes
		for(ushort i = 0; i < 4; i++) frameBytes[i+4] = v4Size[i];
		
		//The frame should have the Discard Frame Upon Tag Alter flag
		frameBytes[8] = Frame::FLAG1_DISCARD_UPON_TAG_ALTER_IF_UNKNOWN_V4;
		frameBytes[9] = 0;
	}
	
	//Return the Frame
	switch(frameType) {
		case FrameClass::CLASS_TEXT:
			return FramePtr(new TextFrame(id, ID3Ver, frameBytes));
		case FrameClass::CLASS_NUMERICAL:
			return FramePtr(new NumericalTextFrame(id, ID3Ver, frameBytes));
		case FrameClass::CLASS_DESCRIPTIVE:
			return FramePtr(new DescriptiveTextFrame(id, ID3Ver, frameBytes, frameOptions(id)));
		case FrameClass::CLASS_URL:
			return FramePtr(new URLTextFrame(id, ID3Ver, frameBytes));
		case FrameClass::CLASS_PICTURE:
			return FramePtr(new PictureFrame(ID3Ver, frameBytes));
		case FrameClass::CLASS_PLAY_COUNT:
			return FramePtr(new PlayCountFrame(ID3Ver, frameBytes));
		case FrameClass::CLASS_POPULARIMETER:
			return FramePtr(new PopularimeterFrame(ID3Ver, frameBytes));
		case FrameClass::CLASS_EVENT_TIMING:
			return FramePtr(new EventTimingFrame(ID3Ver, frameBytes));
		case FrameClass::CLASS_UNKNOWN: default:
			return FramePtr(new UnknownFrame(id, ID3Ver, frameBytes));
	}
}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::create(const FrameID&     frameName,
                              const std::string& textContent,
                              const std::string& description,
                              const std::string& language) const {
	FrameClass frameType = FrameFactory::frameType(frameName);
	
	switch(frameType) {
		case FrameClass::CLASS_TEXT:
			return FramePtr(new TextFrame(frameName, textContent));
		case FrameClass::CLASS_NUMERICAL:
			return FramePtr(new NumericalTextFrame(frameName, textContent));
		case FrameClass::CLASS_DESCRIPTIVE:
			return FramePtr(new DescriptiveTextFrame(frameName,
			                                         textContent,
			                                         description,
			                                         language,
			                                         frameOptions(frameName)));
		case FrameClass::CLASS_URL:
			return FramePtr(new URLTextFrame(frameName, textContent));
		case FrameClass::CLASS_PLAY_COUNT:
			return FramePtr(new PlayCountFrame(atoll(textContent.c_str())));
		case FrameClass::CLASS_POPULARIMETER:
			return FramePtr(new PopularimeterFrame(atoll(textContent.c_str()), 0, description));
		case FrameClass::CLASS_EVENT_TIMING:
			return FramePtr(new EventTimingFrame());
		case FrameClass::CLASS_UNKNOWN: default:
			return FramePtr(new UnknownFrame(frameName));
	}
}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::create(const FrameID&                  frameName,
                              const std::vector<std::string>& textContents,
                              const std::string&              description,
                              const std::string&              language) const {
	FrameClass frameType = FrameFactory::frameType(frameName);
	
	switch(frameType) {
		case FrameClass::CLASS_TEXT:
			return FramePtr(new TextFrame(frameName, textContents));
		case FrameClass::CLASS_NUMERICAL:
			return FramePtr(new NumericalTextFrame(frameName, textContents));
		case FrameClass::CLASS_DESCRIPTIVE:
			return FramePtr(new DescriptiveTextFrame(frameName,
			                                         textContents,
			                                         description,
			                                         language,
			                                         frameOptions(frameName)));
		case FrameClass::CLASS_URL:
			return FramePtr(new URLTextFrame(frameName, textContents));
		default:
			return FramePtr(new UnknownFrame(frameName));
	}
}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::create(const FrameID&     frameName,
                              const long long    frameValue,
                              const std::string& description,
                              const std::string& language) const {
	FrameClass frameType = FrameFactory::frameType(frameName);
	
	switch(frameType) {
		case FrameClass::CLASS_NUMERICAL:
			return FramePtr(new NumericalTextFrame(frameName, frameValue));
		case FrameClass::CLASS_PLAY_COUNT:
			return FramePtr(new PlayCountFrame(frameValue));
		case FrameClass::CLASS_POPULARIMETER:
			return FramePtr(new PopularimeterFrame(frameValue, 0, description));
		default:
			return create(frameName, std::to_string(frameValue), description, language);
	}
}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::createPicture(const ByteArray&   pictureByteArray,
			                            const std::string& mimeType,
			                            const std::string& description,
			                            const PictureType  type) const {
	return FramePtr(new PictureFrame(pictureByteArray, mimeType, description, type));
}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::createPlayCount(const unsigned long long count) const {
	return FramePtr(new PlayCountFrame(count));
}

///@pkg ID3FrameFactory.h
FramePtr FrameFactory::createPlayCount(const unsigned long long count,
                                       const uint8_t            rating,
                                       const std::string&       email) const {
	return FramePtr(new PopularimeterFrame(count, rating, email));
}

///@pkg ID3FrameFactory
///@static
FrameClass FrameFactory::frameType(const FrameID& frameID) {
	if(frameID.unknown()) return FrameClass::CLASS_UNKNOWN;
	
	switch(frameID) {
		//Pictures
		case FRAME_PICTURE:
			return FrameClass::CLASS_PICTURE;
		//Frames with descriptions and/or languages
		case FRAME_COMMENT:
		case FRAME_CUSTOM_USER_INFO:
		case FRAME_UNSYNCHRONISED_LYRICS:
		case FRAME_TERMS_OF_USE:
		case FRAME_USER_DEFINED_URL:
			return FrameClass::CLASS_DESCRIPTIVE;
		//Text frames that should contain an integer value
		//NOTE: Track and Disc are not numerical values as they may contain a
		//slash to separate the total number of tracks/discs in the set.
		case FRAME_BPM:
		case FRAME_DATE:
		case FRAME_PLAYLIST_DELAY:
		case FRAME_TIME:
		case FRAME_LENGTH:
		case FRAME_ORIGINAL_RELEASE_YEAR:
		case FRAME_YEAR:
			return FrameClass::CLASS_NUMERICAL;
		//Frames that are essentially text frames, but don't start with a T
		case FRAME_INVOLVED_PEOPLE:
			return FrameClass::CLASS_TEXT;
		//The Play Count frame
		case FRAME_PLAY_COUNT:
			return FrameClass::CLASS_PLAY_COUNT;
		//The Popularimeter
		case FRAME_POPULARIMETER:
			return FrameClass::CLASS_POPULARIMETER;
		//The Event Timing Codes frame
		case FRAME_EVENT_TIMING_CODES:
			return FrameClass::CLASS_EVENT_TIMING;
		//For the rest of the frames, compare the string values as there's too
		//many enum cases
		default:
			switch(frameID[0]) {
				case 'T': return FrameClass::CLASS_TEXT;
				case 'W': return FrameClass::CLASS_URL;
				default:  return FrameClass::CLASS_UNKNOWN;
			}
	}
}

///@pkg ID3FrameFactory
///@static
ushort FrameFactory::frameOptions(const FrameID& frameID) {
	switch(frameID) {
		//These frames have a language field
		case FRAME_UNSYNCHRONISED_LYRICS:
		case FRAME_COMMENT:
			return DescriptiveTextFrame::OPTION_LANGUAGE;
		//This frame always encodes the text content as Latin-1
		case FRAME_USER_DEFINED_URL:
			return DescriptiveTextFrame::OPTION_LATIN1_TEXT;
		//This frame has a language field and no description
		case FRAME_TERMS_OF_USE:
			return DescriptiveTextFrame::OPTION_LANGUAGE | DescriptiveTextFrame::OPTION_NO_DESCRIPTION;
		default:
			return 0;
	}
}
