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

#include "ID3FrameID.hpp" //For the FrameID class definition

using namespace ID3;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  S T A T I C /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3FrameID.h
const std::vector<std::string> FrameID::FRAME_STR_LIST = {
	"AENC", //0
	"APIC", //1
	"ASPI", //2
	"COMM", //3
	"COMR", //4
	"ENCR", //5
	"EQU2", //6
	"EQUA", //7
	"ETCO", //8
	"GEOB", //9
	"GRID", //10
	"IPLS", //11
	"LINK", //12
	"MCDI", //13
	"MLLT", //14
	"OWNE", //15
	"PCNT", //16
	"POPM", //17
	"POSS", //18
	"PRIV", //19
	"RBUF", //20
	"RVA2", //21
	"RVAD", //22
	"RVRB", //23
	"SEEK", //24
	"SIGN", //25
	"SYLT", //26
	"SYTC", //27
	"TALB", //28
	"TBPM", //29
	"TCOM", //30
	"TCON", //31
	"TCOP", //32
	"TDAT", //33
	"TDEN", //34
	"TDLY", //35
	"TDOR", //36
	"TDRC", //37
	"TDRL", //38
	"TDTG", //39
	"TENC", //40
	"TEXT", //41
	"TFLT", //42
	"TIPL", //43
	"TIME", //44
	"TIT1", //45
	"TIT2", //46
	"TIT3", //47
	"TKEY", //48
	"TLAN", //49
	"TLEN", //50
	"TMCL", //51
	"TMED", //52
	"TMOO", //53
	"TOAL", //54
	"TOFL", //55
	"TOLY", //56
	"TOPE", //57
	"TORY", //58
	"TOWN", //59
	"TPE1", //60
	"TPE2", //61
	"TPE3", //62
	"TPE4", //63
	"TPOS", //64
	"TPRO", //65
	"TPUB", //66
	"TRCK", //67
	"TRDA", //68
	"TRSN", //69
	"TRSO", //70
	"TSO2", //71
	"TSOA", //72
	"TSOC", //73
	"TSOP", //74
	"TSOT", //75
	"TSIZ", //76
	"TSRC", //77
	"TSSE", //78
	"TSST", //79
	"TXXX", //80
	"TYER", //81
	"UFID", //82
	"USER", //83
	"USLT", //84
	"WCOM", //85
	"WCOP", //86
	"WOAF", //87
	"WOAR", //88
	"WOAS", //89
	"WORS", //90
	"WPAY", //91
	"WPUB", //92
	"WXXX", //93
	"XXXX", //94 - Unknown ID3v2.2 frame ID after being converted to ID3v2.4
};

///@pkg IDFrameID3.h
const std::unordered_map<std::string, Frames> FrameID::FRAME_STR_ENUM_MAP = [](const std::vector<std::string>& frames) {
	//Initialize the map with a lambda function that goes through the frames
	//vector and adds a pair to the map that consists of the frame ID string and
	//the integer position in the vector, casted to a Frames enum value.
	
	//The map to return
	std::unordered_map<std::string, Frames> conversionMap;
	//Prevent multiple internal array reallocations
	conversionMap.reserve(frames.size());
	
	//Go through the string vector and add each pair
	for(ushort i = 0; i < frames.size(); i++)
		conversionMap.emplace(frames[i], static_cast<Frames>(i));
	return conversionMap;
}(FrameID::FRAME_STR_LIST);

///@pkg ID3FrameID.h
inline Frames FrameID::getFrameName(const std::string& frameID) {
	auto itr = FRAME_STR_ENUM_MAP.find(frameID);
	
	return itr == FRAME_STR_ENUM_MAP.end() ?
	       //If the frame ID is not found return the XXXX ID
	       Frames::FRAME_UNKNOWN_FRAME :
	       //Else if it is found, then return the frame ID enum value
	       itr->second;
}

///@pkg ID3FrameID.h
inline std::string FrameID::getFrameName(const Frames frameID) {
	return static_cast<ushort>(frameID) >= FRAME_STR_LIST.size() ?
	       //If an unknown Frames enum value is given, return the last frame ID
	       //which should be an unknown frame
	       FRAME_STR_LIST[FRAME_STR_LIST.size() - 1] :
	       //Return the frame ID that corresponds to the enum value
	       FRAME_STR_LIST[static_cast<ushort>(frameID)];
}

///@pkg ID3FrameID.h
const std::unordered_map<std::string, FrameID> FrameID::V2_FRAME_CONVERSION_MAP = {
	{"BUF", "RBUF"},
	{"COM", "COMM"},
	{"CNT", "PCNT"},
	{"CRA", "AENC"},
	{"ETC", "ETCO"},
	{"EQU", "EQUA"},
	{"GEO", "GEOB"},
	{"IPL", "TIPL"},
	{"LNK", "LINK"},
	{"MLL", "MLLT"},
	{"PIC", "APIC"},
	{"POP", "POPM"},
	{"RVA", "RVAD"},
	{"REV", "RVRB"},
	{"STC", "SYTC"},
	{"SLT", "USLT"},
	{"TT1", "TIT1"},
	{"TT2", "TIT2"},
	{"TT3", "TIT3"},
	{"TP1", "TPE1"},
	{"TP2", "TPE2"},
	{"TP3", "TPE3"},
	{"TP4", "TPE4"},
	{"TCM", "TCOM"},
	{"TXT", "TOLY"},
	{"TLA", "TLAN"},
	{"TCO", "TCON"},
	{"TAL", "TALB"},
	{"TPA", "TPOS"},
	{"TRK", "TRCK"},
	{"TRC", "TSRC"},
	{"TYE", "TYER"},
	{"TDA", "TDAT"},
	{"TIM", "TIME"},
	{"TRD", "TRDA"},
	{"TMT", "TMED"},
	{"TBP", "TBPM"},
	{"TEN", "TENC"},
	{"TSS", "TSSE"},
	{"TOF", "TOFN"},
	{"TLE", "TLEN"},
	//TSIZ is completely deprecated in ID3v2.4, so don't check the TSI ID
	{"TDY", "TDLY"},
	{"TKE", "TKEY"},
	{"TOT", "TOAL"},
	{"TOA", "TOPE"},
	{"TOL", "TOLY"},
	{"TOR", "TDOR"},
	{"TXX", "TXXX"},
	{"ULT", "USLT"},
	{"WAF", "WOAF"},
	{"WAR", "WOAR"},
	{"WCM", "WCOM"},
	{"WCP", "WCOP"},
	{"WPB", "WPUB"},
	{"WXX", "WXXX"}
};

///@pkg ID3FrameID.h
inline FrameID FrameID::convertOldFrameIDToNew(const std::string& v2FrameID) {
	auto itr = V2_FRAME_CONVERSION_MAP.find(v2FrameID);
	
	return itr == V2_FRAME_CONVERSION_MAP.end() ?
	       //If the Frame ID is not found return the XXXX ID
	       FrameID(FRAME_UNKNOWN_V2_2_FRAME) :
	       //If the frame ID is found, then return the converted frame ID
	       itr->second;
}

///@pkg ID3FrameID.h
const std::vector<std::string> FrameID::FRAME_DESCRIPTIONS = {
	"Audio Encryption", //0 AENC
	"Attached Picture", //1 APIC
	"Audio Seek Point Index", //2 ASPI
	"Comment", //3 COMM
	"Commercial", //4 COMR
	"Encryption Method", //5 ENCR
	"Equalisation", //6 EQU2
	"Equalisation", //7 EQUA
	"Event Timing Codes", //8 ETCO
	"General Encapsulated Object", //9 GEOB
	"Group Identification Registration", //10 GRID
	"Involved People", //11 IPLS
	"Linked Information", //12 LINK
	"Music CD Identifier", //13 MCDI
	"MPEG Location Lookup Table", //14 MLLT
	"Ownership", //15 OWNE
	"Play Counter", //16 PCNT
	"Popularimeter", //17 POPM
	"Position Synchronisation", //18 POSS
	"Private", //19 PRIV
	"Recommended Buffer Size", //20 RBUF
	"Relative Volume Adjustment", //21 RVA2
	"Relative Volume Adjustment", //22 RVAD
	"Reverb", //23 RVRB
	"Seek", //24 SEEK
	"Signature", //25 SIGN
	"Synchronised Lyrics", //26 SYLT
	"Synchronised Tempo Codes", //27 SYTC
	"Album", //28 TALB
	"BPM", //29 TBPM
	"Composer", //30 TCOM
	"Genre", //31 TCON
	"Copyright", //32 TCOP
	"Date", //33 TDAT
	"Encoding Time", //34 TDEN
	"Playlist Delay", //35 TDLY
	"Original Release Time", //36 TDOR
	"Recording Time", //37 TDRC
	"Release Time", //38 TDRL
	"Tagging Time", //39 TDTG
	"Encoded By", //40 TENC
	"Lyricist", //41 TEXT
	"File Type", //42 TFLT
	"Involved People List", //43 TIPL
	"Time", //44 TIME
	"Content Group", //45 TIT1
	"Title", //46 TIT2
	"Description", //47 TIT3
	"Initial Key", //48 TKEY
	"Language", //49 TLAN
	"Length", //50 TLEN
	"Musician Credit List", //51 TMCL
	"Media Type", //52 TMED
	"Mood", //53 TMOO
	"Original Album", //54 TOAL
	"Original Filename", //55 TOFL
	"Original Lyricist", //56 TOLY
	"Original Artist", //57 TOPE
	"Original Release Year", //58 TORY
	"File Owner", //59 TOWN
	"Artist", //60 TPE1
	"Album Artist", //61 TPE2
	"Conductor", //62 TPE3
	"Modified By", //63 TPE4
	"Disc", //64 TPOS
	"Produced Notice", //65 TPRO
	"Publisher", //66 TPUB
	"Track", //67 TRCK
	"Recording Dates", //68 TRDA
	"Internet Radio Station", //69 TRSN
	"Internet Radio Station Owner", //70 TRSO
	"Album Artist Sort Order", //71 TSO2
	"Album Sort Order", //72 TSOA
	"Composer Sort Order", //73 TSOC
	"Artist Sort Order", //74 TSOP
	"Title Sort Order", //75 TSOT
	"Size", //76 TSIZ
	"ISRC", //77 TSRC
	"Encoding Settings", //78 TSSE
	"Set Subtitle", //79 TSST
	"Custom User Information", //80 TXXX
	"Year", //81 TYER
	"Unique File Identifier", //82 UFID
	"Terms of Use", //83 USER
	"Unsynchronised Lyrics", //84 USER
	"Commercial Information URL", //85 WCOM
	"Copyright URL", //86 WCOP
	"Official File URL", //87 WOAF
	"Official Artist URL", //88 WOAR
	"Official Audio Source URL", //89 WOAS
	"Official Internet Radio Station URL", //90 WORS
	"Official Payment URL", //91 WPAY
	"Official Publisher URL", //92 WPUB
	"User-defined URL", //93 WXXX
	"Unknown" //94 XXXX
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  M E M B E R /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///@pkg ID3FrameID.h
FrameID::FrameID() : FrameID(FRAME_UNKNOWN_FRAME) {}

///@pkg ID3FrameID.h
FrameID::FrameID(const char* const frameID) : FrameID(std::string(frameID)) {}

///@pkg ID3FrameID.h
FrameID::FrameID(const std::string& frameID) : enumID(getFrameName(frameID)),
                                               strID(enumID == FRAME_UNKNOWN_FRAME ? "XXXX" : frameID),
                                               strLen(strID.size()) {}

///@pkg ID3FrameID.h
FrameID::FrameID(const std::string& frameID,
                 const ushort version) : enumID(version >= 3 ? getFrameName(frameID) : convertOldFrameIDToNew(frameID).enumID),
                                         strID(enumID == FRAME_UNKNOWN_FRAME ? "XXXX" : frameID),
                                         strLen(strID.size()) {}

///@pkg ID3FrameID.h
FrameID::FrameID(const Frames frameID) : enumID(frameID),
                                         strID(getFrameName(frameID)),
                                         strLen(strID.size()) {}

///@pkg ID3FrameID.h
FrameID::operator const std::string&() const { return strID; }

///@pkg ID3FrameID.h
FrameID::operator Frames() const { return enumID; }

///@pkg ID3FrameID.h
bool FrameID::operator==(const FrameID& frameID) const { return frameID.enumID == enumID; }

///@pkg ID3FrameID.h
bool FrameID::operator!=(const FrameID& frameID) const { return frameID.enumID != enumID; }

///@pkg ID3FrameID.h
bool FrameID::operator==(const Frames frameID) const { return frameID == enumID; }

///@pkg ID3FrameID.h
bool FrameID::operator!=(const Frames frameID) const { return frameID != enumID; }

///@pkg ID3FrameID.h
bool FrameID::operator==(const std::string& frameID) const { return frameID == strID; }

///@pkg ID3FrameID.h
bool FrameID::operator!=(const std::string& frameID) const { return frameID != strID; }

///@pkg ID3FrameID.h
bool FrameID::operator==(const char* const frameID) const { return frameID == strID; }

///@pkg ID3FrameID.h
bool FrameID::operator!=(const char* const frameID) const { return frameID != strID; }

///@pkg ID3FrameID.h
char FrameID::operator[](const size_t pos) const { return strID[pos]; }

///@pkg ID3FrameID.h
size_t FrameID::size() const { return strLen; }

///@pkg ID3FrameID.h
bool FrameID::unknown() const { return enumID == FRAME_UNKNOWN_FRAME; }

///@pkg ID3FrameID.h
bool FrameID::allowsMultiple() const {
	switch(enumID) {
		case FRAME_AUDIO_ENCRYPTION:
		case FRAME_ATTACHED_PICTURE:
		case FRAME_COMMENT:
		case FRAME_COMMERCIAL:
		case FRAME_ENCRYPTION_METHOD_REGISTRATION:
		case FRAME_EQUALIZATION_2:
		case FRAME_GENERAL_ENCAPSULATED_OBJECT:
		case FRAME_GROUP_IDENTIFICATION_REGISTRATION:
		case FRAME_LINKED_INFORMATION:
		case FRAME_POPULARIMETER:
		case FRAME_PRIVATE:
		case FRAME_RELATIVE_VOLUME_ADJUSTMENT_2:
		case FRAME_SIGNATURE:
		case FRAME_SYNCHRONIZED_LYRICS:
		case FRAME_CUSTOM_USER_INFORMATION:
		case FRAME_UNIQUE_FILE_IDENTIFIER:
		case FRAME_TERMS_OF_USE:
		case FRAME_UNSYNCHRONIZED_LYRICS:
		case FRAME_COMMERCIAL_INFORMATION_URL:
		case FRAME_OFFICIAL_ARTIST_URL:
		case FRAME_USER_DEFINED_URL:
			return true;
		default:
			return false;
	}
}

///@pkg ID3FrameID.h
std::string FrameID::description() const {
	return static_cast<ushort>(enumID) >= FRAME_DESCRIPTIONS.size() ?
	       //If the enum value is not in the descriptions list, then return the
	       //last description, which should be Unknown
	       FRAME_DESCRIPTIONS[FRAME_DESCRIPTIONS.size() - 1] :
	       //Return the frame ID that corresponds to the enum value
	       FRAME_DESCRIPTIONS[static_cast<ushort>(enumID)];
}
