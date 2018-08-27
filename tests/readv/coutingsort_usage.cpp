#include<iostream>
#include<util/counting_sort.hpp>
int main(int argc, char const *argv[])
{
    std::vector<int> data={1,3,9,8,4,6,7,3,4,3,5,2,7,8,2,1};
    std::vector<size_t> permute;
    std::vector<size_t> prefix_sum;
    size_t maxval=9;
    counting_sort(data, permute, &prefix_sum, maxval);
    for(auto e: data)
    {
        std::cout<<e<<"\t";
    }
    std::cout<<std::endl;    
    for(auto e: permute)
    {
        std::cout<<e<<"\t";
    }
    std::cout<<std::endl;

    for(auto e: prefix_sum)
    {
        std::cout<<e<<"\t";
    }
    std::cout<<std::endl;
    for(auto e: permute)
    {
        std::cout<<data[e]<<"\t";
    }
    std::cout<<std::endl;
}