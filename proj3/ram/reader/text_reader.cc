#ifndef TEXT_READER_CC
#define TEXT_READER_CC

#include "./text_reader.h"

#include <fstream> // for files
#include <iterator> // for iterating thru file into one big str
#include <iostream> // for std::cout / std::int
#include <sstream> // for splitting lines


TextReader::TextReader(){
    
}

TextReader::TextReader(const std::string &file_name){
    this->readTextFile(file_name);
}


TextReader::~TextReader(){

}

//bool returning functions
bool TextReader::readTextFile(const std::string &file_name){
   
    std::ifstream ifs(file_name);
    if(ifs.fail()){
        return false;
    }
    
    
    ifs.open(file_name);
    // text content = all of text of file as one big str
    this->text_content = std::string((std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));
    return true;
}

//void returning functions
void TextReader::writeStrToFile(const std::string &content, const std::string &file_name) const{
    
    std::ofstream out(file_name);
    out << content;
    out.close();
}

void TextReader::quarterList(const std::list<std::string> &l, std::vector<std::list<std::string>> &overwrite_vls){
    const int FOUR {4};

    // return vector
    std::vector<std::list<std::string>> ret (FOUR);

    // quarters a list
    int listSize = l.size();

    // see how many even divisions
    int equalDivs = listSize / FOUR;

    int rem = listSize % FOUR;

    int currIndex{0};
    int currList {0};

    for(auto const &str: l){
        if(currIndex != 0 && currIndex % equalDivs == 0){
            // hit a new division, go to next vector
            //
            if(currList != FOUR-1){
                currList ++;
            }
        }
        // else, on current vector and keep going
    
        ret[currList].push_back(str);
        currIndex++;
    }

    // update overwrite list
    
    overwrite_vls = ret;

}


//list returning functions
std::list<std::string> TextReader::getSplitLines(const std::string &content) const {
    // returns a list of strings that are delimited by split
    std::list<std::string> lines;
    std::stringstream ss(content);

    std::string buff;
    while(std::getline(ss,buff,'\n')){
        lines.push_back(buff);
    }
    return lines;
}

// gets list of target str matches
std::list<std::string> TextReader::searchListForTarget(const std::list<std::string> contentList, const std::string target) const{
    // finds lines that include a target word
    std::list<std::string> finds;

    size_t found;
    for(auto const &i : contentList){
        found = i.find(target);
        if(found != std::string::npos){
            finds.push_back(i);
        }
    }
    return finds;
}

// gets list of target str matches
std::list<std::string> TextReader::getListTargetMatches(const std::string &target) const{
    std::list<std::string> split_lines = getSplitLines(this->text_content);

    return searchListForTarget(split_lines,target);

}


//str returning functions
std::string TextReader::getTextContent() const {
    return this->text_content;
}

#endif