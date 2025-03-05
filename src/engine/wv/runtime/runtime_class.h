#pragma once

namespace wv {

struct RuntimeProperties;

class RuntimeClass 
{
public:
    virtual void queryProperties( RuntimeProperties* _pOutProperties ) = 0;
};    

}