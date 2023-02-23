
#ifndef THREADLOAD_CC
#define THREADLOAD_CC

#include "./threadload.h"

PayloadArgs::PayloadArgs(){
    // empty ctor
}


void PayloadArgs::setTarget(const std::string &target){
    this->target_ = target;
}


void PayloadArgs::setSearchSection(const std::list<std::string> &search_section){
    this->search_section_ = search_section;
}

PayloadArgs::~PayloadArgs(){
   
}

Payload::Payload(){
   
}
Payload::~Payload(){
   
}

#endif