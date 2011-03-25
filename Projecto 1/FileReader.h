/* 
 * File:   FileReader.h
 * Author: guilhermerodrigues
 *
 * Created on March 25, 2011, 2:02 PM
 */

#ifndef FILEREADER_H
#define	FILEREADER_H

#include <vector>

class FileReader {
public:
    FileReader();
    FileReader(const FileReader& orig);
    virtual ~FileReader();

    std::vector<int*>* read(const char*);

private:

};

#endif	/* FILEREADER_H */

