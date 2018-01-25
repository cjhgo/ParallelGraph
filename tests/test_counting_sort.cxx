#include <iostream>

#include"counting_sort.hpp"

int main(int argc, char const *argv[])
{
	std::vector<int> data = {1,4,3,1,6,4,4,3,6,6,6,6};
	std::vector<size_t> permute;
	std::vector<size_t> prefix;
	counting_sort(data, permute, &prefix);
	for(auto x : permute)
		std::cout<<x<<"#"<<data[x]<<std::endl;
	std::cout<<std::endl;
	for(auto x : prefix)
		std::cout<<x;

	return 0;
}