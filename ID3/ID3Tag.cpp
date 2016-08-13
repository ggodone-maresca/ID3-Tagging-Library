/***********************************************************************
 * ID3-Tagging-Library Copyright (C) 2016 Gerard Godone-Maresca        *
 * This library comes with ABSOLUTELY NO WARRANTY; for details open    *
 * the document 'README.txt' found enclosed.                           *
 * This is free software, and you are welcome to redistribute it under *
 * certain conditions.                                                 *
 *                                                                     *
 * @author Gerard Godone-Maresca                                       *
 **********************************************************************/

#include <iostream>  //For printing
#include <exception> //For exceptions
#include <cstring>   //For memcmp
#include <regex>     //For regular expressions

#include "ID3.h"
#include "ID3Functions.h"
#include "ID3Frame.h"

using namespace ID3;

///@pkg ID3.h
Tag::Tag(std::ifstream& file) : Tag::Tag() {
	if(!file.is_open())
		return;
	file.seekg(0, std::ifstream::end);
	filesize = file.tellg();
	getFile(file, false);
}

///@pkg ID3.h
Tag::Tag(const std::string& fileLoc) : Tag::Tag() {
	std::ifstream file;
	
	//Check if the file is an MP3 file
	if(!std::regex_search(fileLoc, std::regex("\\.mp3$", std::regex::icase |
	                                                     std::regex::ECMAScript)))
		return;
	
	try {
		file.open(fileLoc, std::ios::binary | std::ios::ate);
		filesize = file.tellg();
		getFile(file);
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag(const Glib::ustring& fileLoc): " << e.what() << std::endl;
	}
	
	std::cout << "ID3 version: " << getVersionString(true) << std::endl;
}

///@pkg ID3.h
Tag::Tag() : isNull(true) {}
             
///@pkg ID3.h
std::string Tag::title() const {
	return getFrameText(Frames::TITLE);
}

///@pkg ID3.h
std::string Tag::genre(bool process) const {
	std::string genreString = getFrameText(Frames::GENRE);
	if(process) {
		std::regex findV1Genre("^\\(\\d+\\)");
		std::smatch v1Genre;
		if(std::regex_search(genreString, v1Genre, findV1Genre)) {
			std::string genreIntStr = v1Genre.str();
			int genreInt = atoi(genreIntStr.substr(1, genreIntStr.length() - 1).c_str());
			genreString = std::regex_replace(genreString, findV1Genre, "");
			if(genreString.length() <= 0)
				genreString = getGenreString(genreInt);
		}
	}
	return genreString;
}

///@pkg ID3.h
std::string Tag::artist() const {
	return getFrameText(Frames::ARTIST);
}

///@pkg ID3.h
std::string Tag::album() const {
	return getFrameText(Frames::ALBUM);
}

///@pkg ID3.h
std::string Tag::year(bool process) const {
	std::string yearString = getFrameText(Frames::YEAR);
	if(process && std::to_string(atoi(yearString.c_str())) != yearString)
		return "";
	return yearString;
}

///@pkg ID3.h
std::string Tag::track(bool process) const {
	std::string trackString = getFrameText(Frames::TRACK);
	if(process) {
		size_t slashPos = trackString.find_first_of('/');
		if(slashPos != std::string::npos) {
			trackString = trackString.substr(0, slashPos);
		}
		if(std::to_string(atoi(trackString.c_str())) != trackString)
			return "";
	}
	return trackString;
}

///@pkg ID3.h
std::string Tag::trackTotal(bool process) const {
	std::string trackString = getFrameText(Frames::TRACK);
	size_t slashPos = trackString.find_first_of('/');
	if(slashPos == std::string::npos)
		return "";
	trackString = trackString.substr(slashPos + 1);
	if(process && std::to_string(atoi(trackString.c_str())) != trackString)
			return "";
	return trackString;
}

///@pkg ID3.h
std::string Tag::disc(bool process) const {
	std::string discString = getFrameText(Frames::DISC);
	if(process) {
		size_t slashPos = discString.find_first_of('/');
		if(slashPos != std::string::npos) {
			discString = discString.substr(0, slashPos);
		}
		if(std::to_string(atoi(discString.c_str())) != discString)
			return "";
	}
	return discString;
}

///@pkg ID3.h
std::string Tag::discTotal(bool process) const {
	std::string discString = getFrameText(Frames::DISC);
	size_t slashPos = discString.find_first_of('/');
	if(slashPos == std::string::npos)
		return "";
	discString = discString.substr(slashPos + 1);
	if(process && std::to_string(atoi(discString.c_str())) != discString)
			return "";
	return discString;
}

///@pkg ID3.h
std::string Tag::composer() const {
	return getFrameText(Frames::COMPOSER);
}

///@pkg ID3.h
std::string Tag::bpm(bool process) const {
	std::string bpmString = getFrameText(Frames::BPM);
	if(process && std::to_string(atoi(bpmString.c_str())) != bpmString)
		return "";
	return bpmString;
}

///@pkg ID3.h
std::string Tag::getVersionString(bool verbose) const {
	std::string versionString;
	
	if(tagsSet.v1) versionString = "v1";
	else if(tagsSet.v1_1) versionString = "v1.1";
	if(tagsSet.v1Extended) versionString += " v1Extended";
	if(tagsSet.v2) {
		versionString += versionString.size() ? " " : "";
		versionString += "v2." + std::to_string(v2TagInfo.majorVer) + ".";
		versionString += std::to_string(v2TagInfo.minorVer);
		if(verbose) {
			versionString += " (" + std::to_string(v2TagInfo.size) + "B";
			if(v2TagInfo.flagUnsynchronisation) versionString += " -unsynchronisation";
			if(v2TagInfo.flagExtHeader) versionString += " -extendedheader";
			if(v2TagInfo.flagExperimental) versionString += " -experimental";
			if(v2TagInfo.flagFooter) versionString += " -footer";
			versionString += ")";
		}
	}
	
	return versionString;
}

///@pkg ID3.h
const bool Tag::null() const {
	return isNull;
}

///@pkg ID3.h
void Tag::getFile(std::ifstream& file, bool close) {
	if(!file.good())
		return;
	
	isNull = false;
	
	getFileV1(file);
	getFileV2(file);
	
	try {
		if(close) file.close();
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::getFile(const std::ifstream& file) closing the file: " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::getFileV1(std::ifstream& file) {
	V1::Tag tags;
	V1::ExtendedTag extTags;
	bool extTagsSet;
	
	if(filesize < V1::BYTE_SIZE)
		return;
	
	try {
		file.seekg(-V1::BYTE_SIZE, std::ifstream::end);
		if(file.fail())
			return;
		
		file.read(reinterpret_cast<char *>(&tags), V1::BYTE_SIZE);
		
		if(memcmp(tags.header, "TAG", 3) != 0)
			return;
		
		//Get the bytes for the extended tags
		if(filesize > V1::BYTE_SIZE + V1::EXTENDED_BYTE_SIZE) {
			file.seekg(-V1::BYTE_SIZE-V1::EXTENDED_BYTE_SIZE, std::ifstream::end);
			extTagsSet = !file.fail();
			if(extTagsSet) file.read(reinterpret_cast<char *>(&extTags), V1::EXTENDED_BYTE_SIZE);
			extTagsSet = memcmp(extTags.header, "TAG+", 4) == 0;
		}
		
		setTags(tags);
		if(extTagsSet) setTags(extTags);
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::getFileV1(const std::ifstream& file): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::getFileV2(std::ifstream& file) {
	Header tagsHeader;
	
	if(filesize < HEADER_BYTE_SIZE)
		return;
	
	try {
		file.seekg(0, std::ifstream::beg);
		if(file.fail())
			return;
		
		file.read(reinterpret_cast<char *>(&tagsHeader), HEADER_BYTE_SIZE);
		
		if(memcmp(tagsHeader.header, "ID3", 3) != 0)
			return;
			
		tagsSet.v2 = true;
		
		v2TagInfo.majorVer = tagsHeader.majorVer;
		v2TagInfo.minorVer = tagsHeader.minorVer;
		if(v2TagInfo.majorVer < MIN_SUPPORTED_VERSION ||
		   v2TagInfo.majorVer > MAX_SUPPORTED_VERSION ||
		   v2TagInfo.minorVer != SUPPORTED_MINOR_VERSION) {
			std::cout << "File uses ID3v2 version " << v2TagInfo.majorVer
			          << "." << v2TagInfo.minorVer << ", which does not"
			          << " have read support in this program." << std::endl;
			tagsSet.v2 = false;
			return;
		}
		
		v2TagInfo.size = uchar_arr_binary_num(tagsHeader.size, 4, true);
		if((unsigned int)tagsHeader.flags & FLAG_UNSYNCHRONISATION == FLAG_UNSYNCHRONISATION)
			v2TagInfo.flagUnsynchronisation = true;
		if((unsigned int)tagsHeader.flags & FLAG_EXT_HEADER == FLAG_EXT_HEADER)
			v2TagInfo.flagExtHeader = true;
		if((unsigned int)tagsHeader.flags & FLAG_EXPERIMENTAL == FLAG_EXPERIMENTAL)
			v2TagInfo.flagExperimental = true;
		if((unsigned int)tagsHeader.flags & FLAG_FOOTER == FLAG_FOOTER)
			v2TagInfo.flagFooter = true;
			
		if(v2TagInfo.size > filesize) {
			if(!tagsSet.v1 && !tagsSet.v1_1 && !tagsSet.v1Extended)
				isNull = true;
			return;
		}
		
		//The position to start reading from the file
		int frameStartPos = HEADER_BYTE_SIZE;
		
		//Skip over the extended header
		if(v2TagInfo.flagExtHeader) {
			ExtHeader extHeader;
			file.seekg(frameStartPos, std::ifstream::beg);
			if(!file.fail()) {
				file.read(reinterpret_cast<char *>(&extHeader), HEADER_BYTE_SIZE);
				//Only the ID3v2.4.0 standard page says that the extended header's size is synchsafe.
				//I do not know if that is accurate or not, but I will take their word for it.
				frameStartPos += HEADER_BYTE_SIZE + uchar_arr_binary_num(extHeader.size, 4, v2TagInfo.majorVer >= 4);
			}
		}
		
		//Loop over the ID3 tags, and stop once all ID3 frames have been
		//reached or a frame is null. Add every frame to the frames map.
		while(frameStartPos + 10 < v2TagInfo.size && frameStartPos < filesize) {
			Frame frame(file, frameStartPos, v2TagInfo.majorVer, filesize);
			frameStartPos = frame.end();
			if(frame.null())
				break;
			frames[frame.frame()] = frame;
		}
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::getFileV2(const std::ifstream& file): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::setTags(const V1::Tag& tags, bool zeroCheck) {
	if(zeroCheck && tags.comment[28] == '\0') {
		V1::P1Tag correctID3VerTags;
		std::memcpy(&correctID3VerTags, &tags, sizeof correctID3VerTags);
		setTags(correctID3VerTags, false);
		return;
	}
	
	tagsSet.v1 = true;
	
	try {
		frames["TIT2"] = Frame("TIT2", terminatedstring(tags.title, 30));
		frames["TPE1"] = Frame("TPE1", terminatedstring(tags.artist, 30));
		frames["TALB"] = Frame("TALB", terminatedstring(tags.album, 30));
		frames["TYER"] = Frame("TYER", terminatedstring(tags.year, 4));
		//frames["COMM"] = Frame("COMM", terminatedstring(tags.comment, 30));
		frames["TCON"] = Frame("TCON", getGenreString(tags.genre));
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(const ID3::v1Tag& tags, bool zeroCheck): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::setTags(const V1::P1Tag& tags, bool zeroCheck) {
	if(zeroCheck && tags.zero != '\0') {
		V1::Tag correctID3VerTags;
		std::memcpy(&correctID3VerTags, &tags, sizeof correctID3VerTags);
		setTags(correctID3VerTags, false);
		return;
	}
	
	tagsSet.v1_1 = true;
	
	try {
		frames["TIT2"] = Frame("TIT2", terminatedstring(tags.title, 30));
		frames["TPE1"] = Frame("TPE1", terminatedstring(tags.artist, 30));
		frames["TALB"] = Frame("TALB", terminatedstring(tags.album, 30));
		frames["TYER"] = Frame("TYER", terminatedstring(tags.year, 4));
		//frames["COMM"] = Frame("COMM", terminatedstring(tags.comment, 28));
		frames["TRCK"] = Frame("TRCK", std::to_string(tags.trackNum));
		frames["TCON"] = Frame("TCON", getGenreString(tags.genre));
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(const ID3::v1_1Tag& tags, bool zeroCheck): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
void Tag::setTags(const V1::ExtendedTag& tags) {
	try {
		unsigned int speed;
		
		tagsSet.v1Extended = true;
		
		frames["TIT2"] = Frame("TIT2", terminatedstring(tags.title, 60));
		frames["TPE1"] = Frame("TPE1", terminatedstring(tags.artist, 60));
		frames["TALB"] = Frame("TALB", terminatedstring(tags.album, 60));
		frames["TCON"] = Frame("TCON", terminatedstring(tags.genre, 30));
		/*startTime = atoi(tags.startTime);
		endTime = atoi(tags.endTime);
		speed = tags.speed;
		
		switch(speed) {
			case 1: { playbackSpeed = V1::ExtendedSpeeds::SLOW; break; }
			case 2: { playbackSpeed = V1::ExtendedSpeeds::MEDIUM; break; }
			case 3: { playbackSpeed = V1::ExtendedSpeeds::FAST; break; }
			case 4: { playbackSpeed = V1::ExtendedSpeeds::HARDCORE; break; }
			case 0: default: { playbackSpeed = V1::ExtendedSpeeds::UNSET; break; }
		}*/
	} catch(const std::exception& e) {
		std::cerr << "Error in ID3::Tag::setTags(const ID3::v1ExtendedTag& tags): " << e.what() << std::endl;
	}
}

///@pkg ID3.h
std::string Tag::getFrameText(Frames frameID) const {
	const std::string frameIDStr = getFrameName(frameID);
	const std::unordered_map<std::string,Frame>::const_iterator result = frames.find(frameIDStr);
	if(result == frames.end()) return "";
	const Frame& frameObj = result->second;
	if(frameObj.null()) return "";
	else                return frameObj.text();
}

///@pkg ID3.h
Tag::TagsOnFile::TagsOnFile() : v1(false),
                                v1_1(false),
                                v1Extended(false),
                                v2(false) {}

///@pkg ID3.h
Tag::TagInfo::TagInfo() : majorVer(-1),
                          minorVer(-1),
                          flagUnsynchronisation(false),
                          flagExtHeader(false),
                          flagExperimental(false),
                          flagFooter(false),
                          size(-1) {}
