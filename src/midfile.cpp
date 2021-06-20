#include <iostream>
#include <fstream>

#include "log.h"

class Midfile {
	private:
		std::ifstream midstream;
		std::uint8_t *file;
		int size;
		int pos = 0;
		int format;
		int numberOfTracks;
		int division;

		std::uint32_t getdword();
		std::uint16_t getword();
		std::uint8_t getbyte();
		int compareString(std::string s);

	public:
		Midfile(char filename[]);
		bool is_open();
		int read();
		int parseHeader();        
};

Midfile::Midfile(char filename[]) {
	midstream.open(filename, std::ios::binary);
}

std::uint8_t Midfile::getbyte() {
	return file[pos++];
}

std::uint16_t Midfile::getword() {
	return file[pos++] <<  8 |
		   file[pos++];
}

std::uint32_t Midfile::getdword() {
	return file[pos++] << 24 |
		   file[pos++] << 16 |
		   file[pos++] <<  8 |
		   file[pos++];
}

bool Midfile::is_open() {
	return midstream.is_open();
}

int Midfile::read() {
	size = midstream.tellg();
	midstream.seekg(0, std::ios::end);
	size = (int)midstream.tellg() - size;

	if(size == 0) {
		log::error("File cannot be empty");
		return 1;
	}
	file = (uint8_t*) malloc(size);
	if(file == NULL) {
		log::error("Memory could not be allocated");
		return 1;
	}
	midstream.seekg(std::ios::beg);
	midstream.read((char*)file, size);

	midstream.close();
	log::debug("File read into memory");
	return 0;
}

int Midfile::parseHeader() {
	if(compareString("MThd")) {
		log::error("File is not a valid MIDI file");
		return 3;
	}

	if(getdword() != 6) {
		log::error("Invalid header lenght");
		return 3;
	}

	format = getword();
	switch(format) {
		case 0:
			log::debug("File format 0: Single track file format");
			log::error("Single track file format not supported yet");
			return 3;
		case 1:
			log::debug("File format 1: Multiple track file format");
			break;
		case 2:
			log::error("File format 2: Multiple fong file format");
			log::error("Multiple song file format not supported yet");
			return 3;
		default:
			log::error("Invalid file format: " + std::to_string(format));
			return 3;
	}

	numberOfTracks = getword();
	if(numberOfTracks < 1) {
		log::debug("Number of tracks: " + std::to_string(numberOfTracks));
		log::error("File has to contain at least one track");
		return 3;
	}

	division = (std::int16_t)getword();
	log::debug("Divisions: " + log::hex_to_string(division) + ", " + std::to_string(division));
	if(division < 0) {
		log::error("SMPTE compatible units not supported yet");
		return 3;
	} else if (division == 0) {
		log::error("Division cannot be 0");
		return 3;
	}

	return 0;
}

int Midfile::compareString(std::string s) {
	
	for (char c : s) {
		if(c != file[pos++]) {
			return 1;
		}
	}
	
	return 0;
}