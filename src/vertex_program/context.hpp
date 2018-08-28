#ifndef PARALLEL_CONTEXT_HPP
#define PARALLEL_CONTEXT_HPP
#include<graph/graph_basic_types.hpp>
namespace parallelgraph
{
    class icontext
    {
     public:
        virtual void signal(vid_type vid)
        {

        }
    };
    template<typename Engine>
    class context : public icontext
    {
     public:
        typedef Engine engine_type;
        context(engine_type& engine):engine(engine){}
        void signal(vid_type vid)
        {
            engine.signal(vid);
        }
     private:
        engine_type& engine;
    };
}
#endif
