#ifndef TEXT_READER_H
#define TEXT_READER_H

#include <string> 
#include <list> 
#include <vector> 

class TextReader{
    public:
     
        TextReader();

     
        TextReader(const std::string &fileName);

      
        ~TextReader();

      
        bool readTextFile(const std::string &file_name);


        std::list<std::string> getSplitLines(const std::string &content) const;

       
        std::list<std::string> searchListForTarget(const std::list<std::string> contentList, const std::string target) const;

      
        std::list<std::string> getListTargetMatches(const std::string &content) const;

     
        std::string getTextContent() const;

     
        void writeStrToFile(const std::string &content, const std::string &file_name) const;
       
        void quarterList(const std::list<std::string> &l, std::vector<std::list<std::string>> &veclistStr);

    private:

        std::string text_content;

};

#endif