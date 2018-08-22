quegr#include<queue>
#include<vector>
#include<iostream>
int main(int argc, char const *argv[])
{
	std::queue<int> qu;
	for (int i = 0; i < 6; ++i)
	{
		qu.push(i);
	}
	std::cout<<qu.front()<<std::endl;
	std::cout<<qu.back()<<std::endl;
	std::vector<int> temp{1,1,0,0};
	std::vector<std::vector<int>> v;
	v.push_back(temp);
	for(auto x : v[0])
		std::cout<<x;
	for(auto& x : v[0])
		std::cout<<x;
	return 0;
}