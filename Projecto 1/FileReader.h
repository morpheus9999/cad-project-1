/* 
 * File:   FileReader.h
 * Author: guilhermerodrigues
 *
 * Created on March 25, 2011, 2:02 PM
 */

#ifndef FILEREADER_H
#define	FILEREADER_H

class FileReader {
public:
    FileReader();
    FileReader(const FileReader& orig);
    virtual ~FileReader();

    void read();

private:

};

#endif	/* FILEREADER_H */

