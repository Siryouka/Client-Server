
#ifndef THREADLOAD_H
#define THREADLOAD_H


#include <list>
#include <string>
class PayloadArgs{
    
    public:
        PayloadArgs();
        ~PayloadArgs();

        void setTarget(const std::string &t);
        void setSearchSection(const std::list<std::string> &l);

     
        std::string target_;

        std::list<std::string> search_section_;
};

class Payload{
    public:
        Payload();
        ~Payload();
       
        PayloadArgs args_;

        std::list<std::string> hits_;
};

#endif