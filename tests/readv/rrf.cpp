#include <string>
#include <iostream>
#include <fstream>
#include <Eigen/Dense>


typedef Eigen::VectorXd factor_type;


int main(int argc, char const *argv[])
{
    factor_type b;
    b.resize(3);
    b<<1,2,3;
    std::cout<<b;

    b<<4,9,7;
    std::cout<<b;

    std::cout<<std::endl;
    std::string file_path = "/home/chen/ict/ParallelGraph/debug/apps/lbp/smallsynth_vdata.tsv";
    std::ifstream infile(file_path);
    int vid=0;
    std::string a;
    b.resize(5);
    while(infile >>a>>b(0)>>b(1)>>b(2)>>b(3)>>b(4))
    {
    	std::cout<<a<<std::endl;
    	std::cout<<b<<std::endl;
    	std::cout<<"end line\n";
    	b.resize(5);
    	std::cout<<b<<std::endl;
    	std::cout<<"end lineline\n\n";


    }
    return 0;
}