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

#ifndef ID3_H
#define ID3_H

#include <fstream>       //For std::ifstream
#include <vector>        //For std::vector
#include <unordered_map> //For std::unordered_map and std::pair
#include <memory>        //For std::shared_ptr
#include <functional>

#include "ID3Frame.h"
#include "ID3PictureFrame.h"

/**
 * The ID3 namespace defines everything related to reading and writing
 * ID3 tags. The only supported versions for reading are ID3v1, ID3v1.1,
 * ID3v1 Extended, ID3v2.3.0, and ID3v2.4.0.
 * 
 * Items relating to ID3v1 are kept in the namespace ID3::V1. V2 related
 * items are kept within the ID3 namespace.
 * 
 * All strings are stored in UTF-8 text encoding.
 * 
 * ID3 class knowledge:
 *     The ID3::Tag class knows the C++ type equivalent of what each frame
 *     stores, and any special formatting the frame has, as well as the ID3
 *     header, footer, and ID3v1.
 *     The ID3::FrameFactory class knows the frame header and how to create a
 *     Frame object.
 *     The ID3::Frame class' children know how to parse and store the bytes of
 *     their frame(s), but not if the data is invalid or not.
 * 
 * ID3-Tagging-Library places a focus on compatibility when reading. As long as
 * there aren't any fundamental differences between ID3v2 versions (such as
 * whether the frame size is synchsafe or not), it will read the information
 * regardless of version. For example, ID3v2.4 tags will be read on ID3v2.3
 * files and vice versa, and ID3v2.4 text encodings are supported in ID3v2.3 frames.
 * 
 * ID3v2.3.0 standard: @link http://id3.org/id3v2.3.0
 * ID3v2.4.0 standard: @link http://id3.org/id3v2.4.0-structure
 * 
 * @todo Add write support.
 * @todo Test it on a greater variety of ID3 files, and unit tests.
 * @todo Add support for saving multiple Frames from the same file.
 * @todo Read the ID3v2 Extended Header.
 */
namespace ID3 {
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	////////////////////////////// T Y P E D E F S //////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	typedef std::vector<uint8_t> ByteArray;
	typedef std::shared_ptr<Frame> FramePtr;
	typedef std::unordered_multimap<std::string, FramePtr> FrameMap;
	typedef std::pair<std::string, FramePtr> FramePair;
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// E N U M S /////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * ID3v1 enums.
	 */
	/*namespace V1 {
		**
		 * An enum of speeds defined in ID3v1 Extended.
		 */
		/*enum ExtendedSpeeds {
			UNSET    = 0,
			SLOW     = 1,
			MEDIUM   = 2,
			FAST     = 3,
			HARDCORE = 4
		};
	}*/
	
	/**
	 * An enum containing the highest value for each star rating for the
	 * Popularimeter (POPM) frame in ID3v2.
	 * 
	 * @todo Properly implement this into a POPM Frame class.
	 */
	/*enum class POPMRatingCutoff {
		ZERO  = 0,   //0-0
		ONE   = 31,  //1-31
		TWO   = 95,  //32-95
		THREE = 159, //96-159
		FOUR  = 223, //160-223
		FIVE  = 255  //224-255
	};*/
	
	/**
	 * An enum of different frames used in ID3v2. Use
	 * ID3::getFrameName(ID3::Frames) to get the frame name used in the standard.
	 * 
	 * The enum values are grouped by each frame, and each frame has the same int
	 * value. Each group has at least one value with the name "FRAME_XXXX", where
	 * XXXX is a descriptive title for the frame's function. The last enum value
	 * of the group is "FRAMEID_XXXX", where XXXX is the ID3v2 frame ID. If the
	 * frame is new in ID3v2.4 there will also be a value with the name
	 * "V4FRAMEID_XXXX". If the frame has been deprecated in ID3v2.4, then there
	 * will be "V3FRAMEID_XXXX_SEE_YYYY", where YYYY is the ID3v2.4 frame ID that
	 * the ID3v2.3 frame has been deprecated in favor of, or there will be
	 * "V3FRAMEID_XXXX_DEPRECATED" if there is no replacement frame in ID3v2.4.
	 * The frame groups are sorted by their frame ID values.
	 * 
	 * @see http://id3.org/id3v2.3.0
	 * @see http://id3.org/id3v2.4.0-frames
	 * @todo Add values for each frame ID.
	 */
	enum Frames : ushort {
		FRAME_AUDIO_ENCRYPTION = 0,
		FRAMEID_AENC           = 0,
		
		FRAME_ATTACHED_IMAGE   = 1,
		FRAME_ATTACHED_PICTURE = 1,
		FRAME_IMAGE            = 1,
		FRAME_PICTURE          = 1,
		FRAMEID_APIC           = 1,
		
		FRAME_AUDIO_SEEK_POINT_INDEX = 2,
		FRAME_SEEK_POINT_INDEX       = 2,
		V4FRAMEID_ASPI               = 2,
		FRAMEID_ASPI                 = 2,
		
		FRAME_COMMENT = 3,
		FRAMEID_COMM  = 3,
		
		FRAME_COMMERCIAL = 4,
		FRAMEID_COMR     = 4,
		
		FRAME_ENCRYPTION_METHOD_REGISTRATION = 5,
		FRAMEID_ENCR                         = 5,
		
		FRAME_EQUALISATION_2 = 6,
		FRAME_EQUALIZATION_2 = 6,
		V4FRAMEID_EQU2       = 6,
		FRAMEID_EQU2         = 6,
		
		FRAME_EQUALISATION      = 7,
		FRAME_EQUALIZATION      = 7,
		V3FRAMEID_EQUA_SEE_EQU2 = 7,
		FRAMEID_EQUA            = 7,
		
		FRAME_EVENT_TIMINGS      = 8,
		FRAME_EVENT_TIMING_CODES = 8,
		FRAME_TIMINGS            = 8,
		FRAMEID_ETCO             = 8,
		
		FRAME_ENCAPSULATED_OBJECT         = 9,
		FRAME_GENERAL_ENCAPSULATED_OBJECT = 9,
		FRAMEID_GEOB                      = 9,
		
		FRAME_GROUP_ID                          = 10,
		FRAME_GROUP_ID_REGISTRATION             = 10,
		FRAME_GROUP_IDENTIFICATION_REGISTRATION = 10,
		FRAMEID_GRID                            = 10,
		
		FRAME_INVOLVED_PEOPLE   = 11,
		V3FRAMEID_IPLS_SEE_TIPL = 11,
		V3FRAMEID_IPLS_SEE_TMCL = 11,
		FRAMEID_IPLS            = 11,
		
		FRAME_LINKED_INFO        = 12,
		FRAME_LINKED_INFORMATION = 12,
		FRAMEID_LINK             = 12,
		
		FRAME_MUSIC_CD_IDENTIFIER = 13,
		FRAMEID_MCDI              = 13,
		
		FRAME_LOCATION_LOOKUO            = 14,
		FRAME_LOCATION_LOOKUP_TABLE      = 14,
		FRAME_MPEG_LOCATION_LOOKUP       = 14,
		FRAME_MPEG_LOCATION_LOOKUP_TABLE = 14,
		FRAMEID_MLLT                     = 14,
		
		FRAME_OWNERSHIP = 15,
		FRAMEID_OWNE    = 15,
		
		FRAME_PLAY_COUNTER = 16,
		FRAMEID_PCNT       = 16,
		
		FRAME_POPULARIMETER = 17,
		FRAMEID_POPM        = 17,
		
		FRAME_POSITION_SYNCHRONISATION = 18,
		FRAME_POSITION_SYNCHRONIZATION = 18,
		FRAMEID_POSS                   = 18,
		
		FRAME_PRIVATE = 19,
		FRAMEID_PRIV  = 19,
		
		FRAME_RECOMMENDED_BUFFER_SIZE = 20,
		FRAMEID_RBUF                  = 20,
		
		FRAME_RELATIVE_VOLUME_ADJUSTMENT_2 = 21,
		V4FRAMEID_RVA2                     = 21,
		FRAMEID_RVA2                       = 21,
		
		FRAME_RELATIVE_VOLUME_ADJUSTMENT = 22,
		V3FRAMEID_RVAD_SEE_RVA2          = 22,
		FRAMEID_RVAD                     = 22,
		
		FRAME_REVERB = 23,
		FRAMEID_RVRB = 23,
		
		FRAME_SEEK     = 24,
		V4FRAMEID_SEEK = 24,
		FRAMEID_SEEK   = 24,
		
		FRAME_SIGNATURE = 25,
		V4FRAMEID_SIGN  = 25,
		FRAMEID_SIGN    = 25,
		
		FRAME_SYNCHED_LYRICS      = 26,
		FRAME_SYNCHED_TEXT        = 26,
		FRAME_SYNCHRONISED_LYRICS = 26,
		FRAME_SYNCHRONIZED_LYRICS = 26,
		FRAME_SYNCHRONISED_TEXT   = 26,
		FRAME_SYNCHRONIZED_TEXT   = 26,
		FRAMEID_SYLT              = 26,
		
		FRAME_SYNCHED_TEMPO_CODES      = 27,
		FRAME_SYNCHRONISED_TEMPO_CODES = 27,
		FRAME_SYNCHRONIZED_TEMPO_CODES = 27,
		FRAMEID_SYTC                   = 27,
		
		FRAME_ALBUM       = 28,
		FRAME_MOVIE_TITLE = 28,
		FRAME_SHOW_TITLE  = 28,
		FRAMEID_TALB      = 28, //ID3::Tag::album()
		
		FRAME_BPM    = 29,
		FRAMEID_TBPM = 29, //ID3::Tag::bpm()
		
		FRAME_COMPOSER = 30,
		FRAMEID_TCOM   = 30, //ID3::Tag::composer()
		
		FRAME_CONTENT_TYPE = 31,
		FRAME_GENRE        = 31,
		FRAMEID_TCON       = 31, //ID3::Tag::genre()
		
		FRAME_COPYRIGHT = 32,
		FRAMEID_TCOP    = 32,
		
		FRAME_DATE                = 33,
		FRAME_RECORDING_TIME_DATE = 33,
		V3FRAMEID_TDAT_SEE_TDRC   = 33,
		FRAMEID_TDAT              = 33,
		
		FRAME_ENCODING_TIME = 34,
		V4FRAMEID_TDEN      = 34,
		FRAMEID_TDEN        = 34,
		
		FRAME_PLAYLIST_DELAY = 35,
		FRAMEID_TDLY         = 35,
		
		FRAME_ORIGINAL_RELEASE_TIME = 36,
		V4FRAMEID_TDOR              = 36,
		FRAMEID_TDOR                = 36,
		
		FRAME_RECORDING_TIME = 37,
		V4FRAMEID_TDRC       = 37,
		FRAMEID_TDRC         = 37,
		
		FRAME_RELEASE_TIME = 38,
		V4FRAMEID_TDRL     = 38,
		FRAMEID_TDRL       = 38,
		
		FRAME_TAGGING_TIME = 39,
		V4FRAMEID_TDTG     = 39,
		FRAMEID_TDTG       = 39,
		
		FRAME_ENCODED_BY = 40,
		FRAMEID_TENC     = 40,
		
		FRAME_LYRICIST    = 41,
		FRAME_TEXT_WRITER = 41,
		FRAMEID_TEXT      = 41,
		
		FRAME_FILETYPE = 42,
		FRAMEID_TFLT   = 42,
		
		FRAME_INVOLVED_PEOPLE_LIST = 43,
		V4FRAMEID_TIPL             = 43,
		FRAMEID_TIPL               = 43,
		
		FRAME_TIME                = 44,
		FRAME_RECORDING_TIME_TIME = 44,
		V3FRAMEID_TIME_SEE_TDRC   = 44,
		FRAMEID_TIME              = 44,
		
		FRAME_CONTENT_GROUP = 45,
		FRAMEID_TIT1        = 45,
		
		FRAME_CONTENT_DESCRIPTION = 46,
		FRAME_NAME                = 46,
		FRAME_TITLE               = 46,
		FRAME_SONG_NAME           = 46,
		FRAMEID_TIT2              = 46, //ID3::Tag::title()
		
		FRAME_DESCRIPTION = 47,
		FRAME_SUBTITLE    = 47,
		FRAMEID_TIT3      = 47,
		
		FRAME_INITIAL_KEY         = 48,
		FRAME_INITIAL_MUSICAL_KEY = 48,
		FRAME_MUSICAL_KEY         = 48,
		FRAMEID_TKEY              = 48,
		
		FRAME_LENGTH = 49,
		FRAMEID_TLEN = 49,
		
		FRAME_MUSICIAN_CREDIT_LIST = 50,
		V4FRAMEID_TMCL             = 50,
		FRAMEID_TMCL               = 50,
		
		FRAME_MEDIA_TYPE = 51,
		FRAMEID_TMED     = 51,
		
		FRAME_MOOD     = 52,
		V4FRAMEID_TMOO = 52,
		FRAMEID_TMOO   = 52,
		
		FRAME_ORIGINAL_ALBUM       = 53,
		FRAME_ORIGINAL_MOVIE_TITLE = 53,
		FRAME_ORIGINAL_SHOW_TITLE  = 53,
		FRAME_ORIGINAL_TALB        = 53,
		FRAMEID_TOAL               = 53,
		
		FRAME_ORIGINAL_FILENAME = 54,
		FRAMEID_TOFN            = 54,
		
		FRAME_ORIGINAL_LYRICIST    = 55,
		FRAME_ORIGINAL_TEXT_WRITER = 55,
		FRAME_ORIGINAL_TEXT        = 55,
		FRAMEID_TOLY               = 55,
		
		FRAME_ORIGINAL_ARTIST    = 56,
		FRAME_ORIGINAL_PERFORMER = 56,
		FRAME_ORIGINAL_TPE1      = 56,
		FRAMEID_TOPE             = 56,
		
		FRAME_ORIGINAL_RELEASE_YEAR = 57,
		FRAME_ORIGINAL_YEAR         = 57,
		FRAME_ORIGINAL_TYER         = 57,
		V3FRAMEID_TORY_SEE_TDOR     = 57,
		FRAMEID_TORY                = 57,
		
		FRAME_FILEOWNER = 58,
		FRAME_LICENSEE  = 58,
		FRAMEID_TOWN    = 58,
		
		FRAME_ARTIST           = 59,
		FRAME_LEAD_ARTIST      = 59,
		FRAME_LEAD_PERFORMER   = 59,
		FRAME_PERFORMER        = 59,
		FRAME_PERFORMING_GROUP = 59,
		FRAME_SOLOIST          = 59,
		FRAMEID_TPE1           = 59, //ID3::Tag::artist()
		
		FRAME_ACCOMPANIEMENT = 60,
		FRAME_ALBUM_ARTIST   = 60,
		FRAME_BAND           = 60,
		FRAME_ORCHESTRA      = 60,
		FRAMEID_TPE2         = 60, //ID3::Tag::albumArtist()
		
		FRAME_CONDUCTOR = 61,
		FRAMEID_TPE3    = 61,
		
		FRAME_INTERPRETED_BY = 62,
		FRAME_MODIFIED_BY    = 62,
		FRAME_REMIXED_BY     = 62,
		FRAMEID_TPE4         = 62,
		
		FRAME_DISC     = 63,
		FRAME_SET_PART = 63,
		FRAMEID_TPOS   = 63, //ID3::Tag::disc(bool), ID3::Tag::discTotal(bool)
		
		FRAME_PRODUCED_NOTICE = 64,
		V4FRAMEID_TPRO        = 64,
		FRAMEID_TPRO          = 64,
		
		FRAME_PUBLISHER = 65,
		FRAMEID_TPUB    = 65,
		
		FRAME_SET_POSITION = 66,
		FRAME_TRACK        = 66,
		FRAME_TRACK_NUMBER = 66,
		FRAMEID_TRCK       = 66, //ID3::Tag::track(bool), ID3::Tag::trackTotal(bool)
		
		FRAME_RECORDING_DATES      = 67,
		FRAME_RECORDING_TIME_DATES = 67,
		V3FRAMEID_TRDA_SEE_TDRC    = 67,
		FRAMEID_TRDA               = 67,
		
		FRAME_RADIO_STATION               = 68,
		FRAME_INTERNET_RADIO_STATION      = 68,
		FRAME_INTERNET_RADIO_STATION_NAME = 68,
		FRAMEID_TRSN                      = 68,
		
		FRAME_RADIO_STATION_OWNER          = 69,
		FRAME_INTERNET_RADIO_STATION_OWNER = 69,
		FRAMEID_TRSO                       = 69,
		
		FRAME_ACCOMPANIEMENT_SORT_ORDER = 70,
		FRAME_ALBUM_ARTIST_SORT_ORDER   = 70,
		FRAME_BAND_SORT_ORDER           = 70,
		FRAME_ORCHESTRA_SORT_ORDER      = 70,
		UNOFFICIAL_FRAMEID_TSO2         = 70,
		FRAMEID_TSO2                    = 70,
		
		FRAME_ALBUM_SORT_ORDER       = 71,
		FRAME_MOVIE_TITLE_SORT_ORDER = 71,
		FRAME_SHOW_TITLE_SORT_ORDER  = 71,
		V4FRAMEID_TSOA               = 71,
		FRAMEID_TSOA                 = 71,
		
		FRAME_COMPOSER_SORT_ORDER = 72,
		UNOFFICIAL_FRAMEID_TSOC   = 72,
		FRAMEID_TSOC              = 72,
		
		FRAME_ARTIST_SORT_ORDER    = 73,
		FRAME_PERFORMER_SORT_ORDER = 73,
		FRAME_SOLOIST_SORT_ORDER   = 73,
		V4FRAMEID_TSOP             = 73,
		FRAMEID_TSOP               = 73,
		
		FRAME_CONTENT_DESCRIPTION_SORT_ORDER = 74,
		FRAME_NAME_SORT_ORDER                = 74,
		FRAME_TITLE_SORT_ORDER               = 74,
		FRAME_SONG_NAME_SORT_ORDER           = 74,
		V4FRAMEID_TSOT                       = 74,
		FRAMEID_TSOT                         = 74,
		
		FRAME_SIZE                = 75,
		V3FRAMEID_TSIZ_DEPRECATED = 75,
		FRAMEID_TSIZ              = 75,
		
		FRAME_ISRC   = 76,
		FRAMEID_TSRC = 76,
		
		FRAME_ENCODING_SETTINGS = 77,
		FRAMEID_TSSE            = 77,
		
		FRAME_SET_SUBTITLE = 78,
		V4FRAMEID_TSST     = 78,
		FRAMEID_TSST       = 78,
		
		FRAME_CUSTOM_USER_INFO = 79,
		FRAME_USER_INFO        = 79,
		FRAMEID_TXXX           = 79,
		
		FRAME_YEAR                = 80,
		FRAME_RECORDING_TIME_YEAR = 80,
		V3FRAMEID_TYER_SEE_TDRC   = 80,
		FRAMEID_TYER              = 80, //ID3::Tag::year()
		
		FRAME_UNIQUE_FILE_IDENTIFIER = 81,
		FRAMEID_UFID                 = 81,
		
		FRAME_TERMS_OF_USE = 82,
		FRAME_TOU          = 82,
		FRAMEID_USER       = 82,
		
		FRAME_LYRICS                = 83,
		FRAME_TEXT_TRANSCRIPTION    = 83,
		FRAME_TRANSCRIPTION         = 83,
		FRAME_UNSYNCHED_LYRICS      = 83,
		FRAME_UNSYNCHRONISED_LYRICS = 83,
		FRAME_UNSYNCHRONIZED_LYRICS = 83,
		FRAMEID_USLT                = 83,
		
		FRAME_COMMERCIAL_INFO_URL        = 84,
		FRAME_COMMERCIAL_INFORMATION_URL = 84,
		FRAME_URL_COMMERCIAL_INFO        = 84,
		FRAME_URL_COMMERCIAL_INFORMATION = 84,
		FRAMEID_WCOM                     = 84,
		
		FRAME_COPYRIGHT_URL         = 85,
		FRAME_LEGAL_INFO_URL        = 85,
		FRAME_LEGAL_INFORMATION_URL = 85,
		FRAME_URL_COPYRIGHT         = 85,
		FRAME_URL_LEGAL_INFO        = 85,
		FRAME_URL_LEGAL_INFORMATION = 85,
		FRAMEID_WCOP                = 85,
		
		FRAME_OFFICIAL_FILE_URL             = 86,
		FRAME_URL_OFFICIAL_FILE_INFO        = 86, 
		FRAME_URL_OFFICIAL_FILE_INFORMATION = 86,
		FRAMEID_WOAF                        = 86,
		
		FRAME_OFFICIAL_ARTIST_URL    = 87,
		FRAME_OFFICIAL_PERFORMER_URL = 87,
		FRAME_URL_OFFICIAL_ARTIST    = 87,
		FRAME_URL_OFFICIAL_PERFORMER = 87,
		FRAMEID_WOAR                 = 87,
		
		FRAME_OFFICIAL_AUDIO_SOURCE_URL = 88,
		FRAME_URL_OFFICIAL_AUDIO_SOURCE = 88,
		FRAMEID_WOAS                    = 88,
		
		FRAME_OFFICIAL_INTERNET_RADIO_STATION_URL = 89,
		FRAME_URL_OFFICIAL_INTERNET_RADIO_STATION = 89,
		FRAMEID_WORS                              = 89,
		
		FRAME_PAYMENT_URL = 90,
		FRAME_URL_PAYMENT = 90,
		FRAMEID_WPAY      = 90,
		
		FRAME_OFFICIAL_PUBLISHER_URL = 91,
		FRAME_URL_OFFICIAL_PUBLISHER = 91,
		FRAMEID_WPUB                 = 91,
		
		FRAME_USER_DEFINED_URL = 92,
		FRAME_URL_USER_DEFINED = 92,
		FRAMEID_WXXX           = 92
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// S T R U C T S ///////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * ID3v1 structs.
	 */
	namespace V1 {
		/**
		 * A 128-bit struct that captures the structure of an ID3v1 tag.
		 */
		struct Tag {
			char header[3];
			char title[30];
			char artist[30];
			char album[30];
			char year[4];
			char comment[30];
			uint8_t genre;
		};
		
		/**
		 * A 128-bit struct that captures the structure of an ID3v1.1 tag.
		 */
		struct P1Tag {
			char header[3];
			char title[30];
			char artist[30];
			char album[30];
			char year[4];
			char comment[28];
			char zero;
			uint8_t trackNum;
			uint8_t genre;
		};
		
		/**
		 * A 227-bit struct that captures the structure of an extended ID3v1 tag.
		 * ID3v1 Extended tags are placed before the standard ID3v1 tag.
		 */
		struct ExtendedTag {
			char header[4];
			char title[60];
			char artist[60];
			char album[60];
			uint8_t speed;
			char genre[30];
			char startTime[6];
			char endTime[6];
		};
	}
	
	/**
	 * A 10-bit struct that captures the structure of the ID3v2 header.
	 */
	struct Header {
		char header[3];
		uint8_t majorVer;
		uint8_t minorVer;
		uint8_t flags;
		uint8_t size[4]; //A synchsafe integer in ID3v2.4+
	};
	
	/**
	 * A 10-bit struct that captures the structure of the ID3v2.3 extended header.
	 */
	struct V3ExtHeader {
		uint8_t size[4];
		uint8_t flags[2];
		uint8_t paddingSize[4];
	};
	
	/**
	 * An 8-bit struct that captures the structure of the ID3v2.4 extended header.
	 */
	struct V4ExtHeader {
		uint8_t size[4]; //A synchsafe integer
		uint8_t flagBytes;
		uint8_t flags;
	};
	
	/**
	 * A struct that contains information about a picture embedded in ID3v2 tags.
	 * Every field of this struct is constant, you must create a new Picture to
	 * modify a field.
	 * The null variable will be set to true if given an invalid MIME type. The
	 * picture data is not checked to verify that it is a valid image.
	 */
	struct Picture {
		/**
		 * Create a Picture struct. Every parameter is optional, but unless given
		 * a valid MIME type the struct will be null.
		 * 
		 * Defined in ID3PictureFrame.cpp.
		 * 
		 * @param pictureByteArray The uint8_t vector of the PNG or JPG image.
		 * @param mimeType The MIME type.
		 * @param pictureDescription The description.
		 * @param pictureType The picture type defined in the ID3v2 specification
		 *                    for the APIC field. Defaults to FRONT_COVER.
		 */
		Picture(const ByteArray&   pictureByteArray=ByteArray(),
			     const std::string& mimeType="",
			     const std::string& pictureDescription="",
			     const PictureType  pictureType=PictureType::FRONT_COVER);
		const std::string MIME;
		const PictureType type;
		const std::string description;
		const ByteArray   data;
		const bool        null;
	};
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// C L A S S E S ///////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	/**
	 * A class that, given a file or filename, will read its ID3 tags.
	 * Call Tag::null() after instantiation to check if the file was
	 * properly read. Files must be .mp3 files.
	 * 
	 * Defined in ID3Tag.cpp.
	 */	
	class Tag {
		public:
			/**
			 * Constructor that takes a filename and opens the file.
			 * 
			 * @param fileLoc The file path.
			 */
			Tag(const std::string& fileLoc);
			
			/**
			 * Constructor that takes an ifstream file object and reads the ID3 data.
			 * NOTE: This does NOT close the file. You must call file_obj.close() after
			 *       instantiating an ID3::Tag object with this constructor.
			 * NOTE: The file must be open to read the tags.
			 * NOTE: This constructor does not check to see if the file
			 *       is an MP3 file or not.
			 * 
			 * @param file The ifstream file object.
			 */
			Tag(std::ifstream& file);
			
			/**
			 * A constructor to set default values.
			 * Creating a frame with this constructor will result in a
			 * "null" frame.
			 */
			Tag();
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			//////////////////////// S T A R T   F R A M E ////////////////////////
			////////////////// G E T T E R S   &   S E T T E R S //////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
			/**
			 * Check if a frame exists.
			 * 
			 * @param frameName A Frames enum variable that represents
			 *                  an ID3v2 frame ID.
			 * @return If the Frame exists or not.
			 */
			bool frameExists(Frames frameName) const;
			
			/**
			 * Get the text content of a frame.
			 * 
			 * NOTE: Not all frames support text content. If the given
			 *       frame name does not, or the music file does not
			 *       contain an ID3v2 frame of that type, an empty
			 *       string will be returned.
			 * 
			 * NOTE: No formatting will be done, so if the frame has special
			 *       formatting then you should call the relevant method instead.
			 * 
			 * @param frameName A Frames enum variable that represents
			 *                  an ID3v2 frame ID.
			 * @return The text content, or "" if the frame is not found,
			 *         "null", or not a text frame.
			 */
			std::string textContent(Frames frameName) const;
			
			/**
			 * Get the text content of a frame, split up into a vector for each
			 * individual string in the frame.
			 * 
			 * NOTE: Not all frames support text content. If the given frame name
			 *       does not, or the music file does not contain an ID3v2 frame of
			 *       that type, the vector will contain just an empty string. Also,
			 *       multiple values do not make sense for all text frames. For
			 *       those frames, use ID3::Tag::textContent(Frames) instead.
			 * 
			 * NOTE: No formatting will be done, so if the frame has special
			 *       formatting then you should call the relevant method instead.
			 * 
			 * NOTE: If the file uses ID3v2.3, then the multiple value separator
			 *       is a slash instead of a null character, and is only supported
			 *       in the Composer, Lyricist, Original Lyricist, Original Artist,
			 *       and Artist frames. If requesting another frame from an ID3v2.3
			 *       file, then the vector will not separate the text content by
			 *       the slash separator.
			 * 
			 * NOTE: The returned vector will always hold at least one string that
			 *       is either an empty string or the frame text content.
			 * 
			 * @param frameName A Frames enum variable that represents
			 *                  an ID3v2 frame ID.
			 * @return A string vector of the text content split by the multiple
			 *         value separator. The vector will not contain empty strings,
			 *         unless there isn't a text value or it is empty.
			 */
			std::vector<std::string> textContents(Frames frameName) const;
			
			/**
			 * Get the title tag.
			 * 
			 * @return The title of the music file, or "" if there is
			 *         no title set.
			 */
			std::string title() const;
			
			/**
			 * Get the genre tag.
			 * 
			 * @param process If true, this will process the genre string according
			 *                to the specification of the TCOM frame. It will look
			 *                for a number surrounded by parenthesis. If found, the
			 *                substring will be removed, and if no text follows the
			 *                substring, the number will be parsed and the
			 *                corresponding ID3v1 genre will be returned.
			 *                If false, the raw genre string as it appears on file
			 *                will be returned.
			 * @return The genre of the music file, or "" if there is
			 *         no title set.
			 */
			std::string genre(bool process=true) const;
			
			/**
			 * Get the genre tag as a vector of genre string(s).
			 * 
			 * @see ID3::Tag::genre(bool)
			 * @see ID3::Tag::textContents(Frames)
			 */
			std::vector<std::string> genres(bool process=true) const;
			
			/**
			 * Get the artist tag.
			 * 
			 * @return The artist of the music file, or "" if there is
			 *         no title set.
			 */
			std::string artist() const;
			
			/**
			 * Get the artist tag as a vector of artist string(s).
			 * 
			 * @see ID3::Tag::artist()
			 * @see ID3::Tag::textContents(Frames)
			 */
			std::vector<std::string> artists() const;
			
			/**
			 * Get the album tag.
			 * 
			 * @return The album of the music file, or "" if there is
			 *         no title set.
			 */
			std::string album() const;
			
			/**
			 * Get the album tag as a vector of album string(s).
			 * 
			 * @see ID3::Tag::album()
			 * @see ID3::Tag::textContents(Frames)
			 */
			std::vector<std::string> albums() const;
			
			/**
			 * Get the album artist/band/orchestra/accompaniment tag.
			 * 
			 * @return The Album artist of the music file, or "" if
			 *         there is no tag set.
			 */
			std::string albumArtist() const;
			
			/**
			 * Get the album artist/band/orchestra/accompaniment tag as a vector of
			 * album artist string(s).
			 * 
			 * @see ID3::Tag::albumArtist()
			 * @see ID3::Tag::textContents(Frames)
			 */
			std::vector<std::string> albumArtists() const;
			
			/**
			 * Get the year tag.
			 * 
			 * @return The year of the music file, or "" if there is
			 *         no year set.
			 */
			std::string year() const;
			
			/**
			 * Get the track tag.
			 * 
			 * @param process If true, this method will return an empty
			 *                string if the frame value is not an integer.
			 *                It will also ignore any text after a slash
			 *                ('/') in the frame content, 
			 *                If false, the raw track string as it
			 *                appears in the TRCK frame on file will be
			 *                returned.
			 * @return The track of the music file, or "" if there is
			 *         no track set.
			 * @see ID3::Tag::trackTotal(bool)
			 */
			std::string track(bool process=true) const;
			
			/**
			 * Get the total number of tracks in the set of the original recording.
			 * This is taken from the TRCK frame, and consists of text
			 * located after a slash ('/') in the frame content.
			 * 
			 * @param process If true, this method will return an empty
			 *                string if the value is not an integer.
			 *                If false, the raw string as it appears
			 *                on file after a slash will be returned.
			 * @return The track total of the music file, or "" if there
			 *         is no track total set.
			 * @see ID3::Tag::track(bool)
			 */
			std::string trackTotal(bool process=true) const;
			
			/**
			 * Get the disc tag.
			 * 
			 * @param process If true, this method will return an empty
			 *                string if the frame value is not an integer.
			 *                It will also ignore any text after a slash
			 *                ('/') in the frame content, 
			 *                If false, the raw disc string as it
			 *                appears in the TPOS frame on file will be
			 *                returned.
			 * @return The disc number of the music file, or "" if there
			 *         is no disc number set.
			 * @see ID3::Tag::discTotal(bool)
			 */
			std::string disc(bool process=true) const;
			
			/**
			 * Get the total number of discs in the set of the original recording.
			 * This is taken from the TPOS frame, and consists of text
			 * located after a slash ('/') in the frame content.
			 * 
			 * @param process If true, this method will return an empty
			 *                string if the value is not an integer.
			 *                If false, the raw string as it appears
			 *                on file after a slash will be returned.
			 * @return The disc total of the music file, or "" if there is
			 *         no disc total set.
			 * @see ID3::Tag::disc(bool)
			 */
			std::string discTotal(bool process=true) const;
			
			/**
			 * Get the composer tag.
			 * 
			 * @return The composer of the music file, or "" if there is
			 *         no composer set.
			 */
			std::string composer() const;
			
			/**
			 * Get the composer tag as a vector of composer string(s).
			 * 
			 * @see ID3::Tag::composer()
			 * @see ID3::Tag::textContents(Frames)
			 */
			std::vector<std::string> composers() const;
			
			/**
			 * Get the BPM tag.
			 * 
			 * @return The bpm of the music file, or "" if there is
			 *         no bpm set.
			 */
			std::string bpm() const;
			
			/**
			 * Get the attached picture.
			 * 
			 * @return The attached picture, encapsulated in a ID3::Picture struct.
			 *         If there is no attached picture, or it has an improper
			 *         MIME type, the null field will be true.
			 */
			Picture picture() const;
			
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			////////////////////////// E N D   F R A M E //////////////////////////
			////////////////// G E T T E R S   &   S E T T E R S //////////////////
			///////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			
			/**
			 * Returns a string representation of the ID3 versions used in the file.
			 * 
			 * @param verbose If true, it will print the flags of the ID3v2 version.
			 * @return The string representation of the versions used.
			 */
			std::string getVersionString(bool verbose=false) const;
			
			/**
			 * @returns true if the given file is a valid MP3 file, false
			 *          otherwise.
			 */
			const bool null() const;
			
			/**
			 * Print all the tag information.
			 */
			void print() const;
			
		protected:
			/**
			 * A struct that records what ID3 versions a file contains.
			 */
			struct TagsOnFile {
				TagsOnFile();
				bool v1;
				bool v1_1;
				bool v1Extended;
				bool v2;
			};
			
			/**
			 * A struct that records the ID3v2 information.
			 */
			struct TagInfo {
				TagInfo();                  //Constructor
				ushort majorVer;             //ID3v2 major version
				ushort minorVer;             //ID3v2 minor version
				bool flagUnsynchronisation; //Unsynchronization flag
				bool flagExtHeader;         //Extended header flag
				bool flagExperimental;      //Experimental flag
				bool flagFooter;            //Footer flag
				ulong size;                 //Tag size
				ulong totalSize;            //Total tag size (tag size + header size
				                            // + extended header size + footer size)
				ulong paddingStart;         //The byte in which padding starts
			};
			
			/**
			 * The filesize of the music file.
			 */
			ulong filesize;
			
			/**
			 * The filename (if not getting the file via an ifstream object).			 
			 */
			std::string filename;
			
			/**
			 * A constructor helper method that gets the tag information from the given file.
			 * 
			 * @param file The file object.
			 */
			void readFile(std::ifstream& file);
			
			/**
			 * A constructor helper method that reads the ID3v1 tags from
			 * the file.
			 * 
			 * @param file The file object.
			 */
			void readFileV1(std::ifstream& file);
			
			/**
			 * A constructor helper method that reads the ID3v2 tags from
			 * the file.
			 * 
			 * @param file The file object.
			 */
			void readFileV2(std::ifstream& file);
			
			/**
			 * A constructor helper method that gets a v1 tag struct and sets the class'
			 * variables to the information in the struct.
			 * 
			 * @param tags The ID3v1 tag struct.
			 * @param zeroCheck If true, this will check to see if the tags is actually
			 *                  set to ID3v1.1.
			 */
			void setTags(const V1::Tag& tags, bool zeroCheck=true);
			
			/**
			 * A constructor helper method that gets a v1.1 tag struct and sets the class'
			 * variables to the information in the struct.
			 * 
			 * @param tags The ID3v1.1 tag struct.
			 * @param zeroCheck If true, this will check to see if the tags is actually
			 *                  set to ID3v1.
			 */
			void setTags(const V1::P1Tag& tags, bool zeroCheck=true);
			
			/**
			 * A constructor helper method that gets a v1 Extended tag struct and sets the class'
			 * variables to the information in the struct. This will override any information
			 * previously set from a v1 or v1.1 tag.
			 * 
			 * @param tags The ID3v1 Extended tag struct.
			 */
			void setTags(const V1::ExtendedTag& tags);
			
			/**
			 * This will be true if the given file is a valid MP3 file, and
			 * false otherwise. Call null() to check the value externally.
			 * 
			 * @see ID3::Tag::null()
			 */
			bool isNull;
			
			/**
			 * A TagsOnFile struct that records all the ID3 versions
			 * that were found on the file.
			 */
			TagsOnFile tagsSet;
			
			/**
			 * A V2TagInfo struct that records the ID3v2 internal
			 * information that were found on the file.
			 */
			TagInfo v2TagInfo;
			
			/**
			 * A map of all frames created from ID3v1 tags and read from
			 * ID3v2 frames.
			 */
			FrameMap frames;
	};
}

#endif
