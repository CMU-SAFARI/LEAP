/*
 * RefDB.cc
 *
 *  Created on: May 30, 2013
 *      Author: hxin
 */
#include "RefDB.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

RefDB::RefDB() { curChromoNum = -1; }

RefDB::~RefDB() {}

void RefDB::loadRefFile(string refName) {
	if (refFile.is_open()) {
	  refFile.close();
	  refPos.clear();
	}

	refFile.open(refName.c_str());

	if (!refFile.is_open()) {
	  LOG(ERROR, "RefFile open failed\n");
	  exit(1);
	}

	string refMetaName = "." + refName + ".meta";
	fstream refMetaFile(refMetaName.c_str(), ios_base::in);

	if (refMetaFile.is_open()) {
	  unsigned long int tempPos;
	  while (refMetaFile >> tempPos) {
	    refPos.push_back((streampos)tempPos);
	  }

	  LOG(INFO, "refPos.size(): %lu\n", refPos.size());

	} else {
	  refMetaFile.open(refMetaName.c_str(), ios_base::out | ios_base::trunc);

	  string temp;
	  streampos tempRefPos;

	  while (refFile.good()) {
	    tempRefPos = refFile.tellg();
	    getline(refFile, temp);
	    if (temp[0] == '>') {
	      refPos.push_back(tempRefPos);
	      refMetaFile << (unsigned long int)tempRefPos << endl;
	    }
	  }
	}

	refMetaFile.close();
}

bool RefDB::loadChromo(int chromoNum) {
	if (curChromoNum == chromoNum) return true;

	curChromoNum = chromoNum;

	LOG(INFO, "RefDB loading chromosome: %d\n", chromoNum);

	if (chromoNum >= refPos.size()) return false;

	chromosome.clear();
	refFile.clear();
	refFile.seekg(refPos[chromoNum]);

	getline(refFile, chromoName);

	string temp;

	while (refFile.good() && (temp.size() == 0 || temp[0] != '<')) {
	  chromosome.append(temp);
	  getline(refFile, temp);
	  if (temp[0] == '>') {
	    LOG(INFO, "Done loading chromosome.\n");
	    break;
	  }
	}

	return true;
}

void RefDB::unloadAll() {
	refPos.clear();
	chromosome.clear();
	refFile.close();
}

unsigned int RefDB::getChromoLength() { return chromosome.size(); }

unsigned int RefDB::getNumOfChromo() { return refPos.size(); }

string RefDB::getChromoName() { return chromoName; }

string RefDB::searchString(string target, int length, int& chromoNum, int& pos) {
	while (true) {
	  //cout << refPos.size() << endl;
	  if (chromoNum >= refPos.size() ) 
	    return string("");

	  loadChromo(chromoNum);
	  pos = chromosome.find(target, pos + 1);
	  if (pos != string::npos) {
	    return chromosome.substr(pos, length);
	  }
	  else
	    chromoNum++;
	}
}

string RefDB::getRefSeq(unsigned int refLoc, unsigned int length) {
	// If oversize, return empty seed.
	if (refLoc >= chromosome.size()) return string("");

	if (refLoc + length > chromosome.size())
	  return (chromosome.substr(refLoc, length) +
	          string(refLoc + length - chromosome.size(), 'N'));

	return chromosome.substr(refLoc, length);
}

char RefDB::getRefBase(unsigned int refLoc, unsigned int offset) {
	if (refLoc + offset >= chromosome.size()) return 'N';
	return chromosome[refLoc + offset];
}
