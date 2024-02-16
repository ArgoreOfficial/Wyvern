#pragma once

namespace wv 
{
    class iRenderPass
    {
    public:
         iRenderPass();
        virtual ~iRenderPass();

        virtual void execute();

    private:
        
    };
}