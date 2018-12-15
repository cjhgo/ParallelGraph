#include<vector>

namespace parallelgraph{
	template<typename valuetype, typename sizetype=size_t>
	class csr_storage
	{
	public:
		csr_storage(){};
		void swap(std::vector<valuetype>& value_vec,
				  std::vector<sizetype>& valueptr_vec)
		{
			values.swap(value_vec);
			value_ptrs.swap(valueptr_vec);
		}
	// private:
		std::vector<valuetype> values;
		std::vector<sizetype> value_ptrs;

	};


}