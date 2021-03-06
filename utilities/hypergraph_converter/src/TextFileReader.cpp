
#ifndef _TEXTFILE_READER_CPP
#define _TEXTFILE_READER_CPP

// ### TextFileReader.cpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 01/12/2004: Last Modified
//
// ###

#include "TextFileReader.hpp"

int TextFileReader::maxLength = 10000000;
int TextFileReader::maxPinsInChunk = 10000000;

TextFileReader::TextFileReader() {
  buffer.reserve(maxLength);
  length = 0;
}

TextFileReader::~TextFileReader() {}

void TextFileReader::getLine(std::ifstream &input_stream) {
  input_stream.getline(buffer.data(), maxLength);
  length = strlen(buffer.data());
}

#endif
