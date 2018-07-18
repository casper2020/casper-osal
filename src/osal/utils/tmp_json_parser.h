#pragma once

// TODO TODO TODO get rid of this helper and JSONCPP

#include "json/json.h"
#include "osal/osalite.h"
#include <iostream>

class TmpJsonParser
{
public:
    Json::Value* root_;
    char*        json_text_;
    uint32_t     json_text_len_;
    
    Json::Value* LoadAndParse (const char* a_filename);
    
    TmpJsonParser ();
    virtual ~TmpJsonParser ();
    
    void Close();
};

inline TmpJsonParser::TmpJsonParser ()
{
    json_text_     = NULL;
    json_text_len_ = 0;
    root_          = NULL;
}

inline TmpJsonParser::~TmpJsonParser ()
{
    Close();
}

inline void TmpJsonParser::Close ()
{
    if ( json_text_ != NULL ) {
        free(json_text_);
        json_text_     = NULL;
    }
    json_text_len_ = 0;
    if ( root_ != NULL ) {
        delete root_;
        root_ = NULL;
    }
}

inline Json::Value* TmpJsonParser::LoadAndParse (const char* a_filename)
{
    osal::File   jsonfile(a_filename);
    Json::Reader reader;
    
    Close();
    root_ = new Json::Value();
    if ( root_ == NULL ) {
        return NULL;
    }
    if ( jsonfile.Open(osal::File::EOpenModeRead) != osal::File::EStatusOk ) {
        return NULL;
    }
    
    jsonfile.Size(&json_text_len_);
    json_text_ = (char*) malloc(json_text_len_+1);
    jsonfile.Read(json_text_, json_text_len_, &json_text_len_);
    json_text_[json_text_len_] = 0;
    jsonfile.Close();
    
    if ( reader.parse(json_text_, *root_, false) == true ) {
        return root_;
    }
    
    // Report failures and their locations
    // in the document.
    std::cout<<"Failed to parse Value JSON"<<std::endl <<reader.getFormattedErrorMessages() <<std::endl;
    
    Close();
    return root_;
}
