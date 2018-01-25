#include<vector>
#include <cstddef>
#include <algorithm>
#include <atomic>

template<typename valuetype, typename sizetype>
void counting_sort(const std::vector<valuetype>& value_vec,
				   std::vector<sizetype>& permute_index,
				   std::vector<sizetype>* prefix_array=NULL)
{

	if(value_vec.size() == 0) return;
	valuetype maxval = *std::max_element(value_vec.begin(), value_vec.end());

	std::vector< std::atomic<sizetype> > counter(maxval+1);

	for(size_t i = 0; i < value_vec.size(); i++)
	{
		valuetype val = value_vec[i];
		counter[val]++;
	}

	for(size_t i = 1; i < counter.size(); i++)
	{
		std::cout<<counter[i]<<std::endl;
		counter[i] += counter[i-1];
		std::cout<<counter[i]<<std::endl<<std::endl;
	}
	permute_index.resize(value_vec.size());

	if( prefix_array != NULL )
	{
		prefix_array->resize(counter.size()+1);
		(*prefix_array)[0]=0;
		for(size_t i = 0; i < counter.size(); i++)
		{
			std::cout<<counter[i];
			(*prefix_array)[i+1] = counter[i];
			//prefix_array[i] = counter[i];
		}
	}

	for(size_t i = 0; i < value_vec.size(); i++)
	{
		valuetype val = value_vec[i];
		size_t pos = --counter[val];
		permute_index[pos] = i;
	}

}